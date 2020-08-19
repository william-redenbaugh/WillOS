/*
 * Threads.cpp - Library for threading on the Teensy.
 *
 *******************
 * 
 * Copyright 2017 by Fernando Trias.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
 * and associated documentation files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, 
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or 
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *******************
 */

/*
* Brief: File was heavily refractored and re-documented by William Redenbaugh in 2020. As much as I've gone through and flushed out the 
* code as best as I could, props to the original creator of the TeensyThreads system. I heavily refractored it to match what I want my systeh
* to do, but the code is still entirely based of Fernando Trias, so props to that guy! Check out his stuff. 
*
*/


#include "OSThreadKernel.h"

unsigned int time_start;
unsigned int time_end;

/*
*   @brief Current thread that we have context of.
*   @notes This was contained in TeensyThreads' Thread class as current_thread
*/
os_thread_id_t current_thread_id = 0 ;

/*
*   @brief Keeps track of how many threads we have at the moment
*   @notes This was contained in TeensyThreads' Thread class as thread_count
*/
int thread_count = 0;

/*
* @brief The default stack size of a thread being generated on the OS
* @notes Can be defined as a preprocessor command
*/
#ifndef EXTERN_OS_DEFAULT_STACK_SIZE
int OS_DEFAULT_STACK_SIZE = 4092;
#else 
int OS_DEFAULT_STACK_SIZE = EXTERN_OS_DEFAULT_STACK_SIZE;
#endif 

#ifndef EXTERN_OS_DEFAULT_TICKS
int OS_DEFAULT_TICKS = 10;
#else 
int OS_DEFAULT_TICKS = EXTERN_OS_DEFAULT_TICKS;
#endif 

//ADDED, total time to spend asleep
volatile int substractor = 0;

extern volatile uint32_t systick_millis_count;

//ADDED, per task sleep time info
struct scheduler_info{
  volatile int sleep_time_till_end_tick;
} task_info[MAX_THREADS];

/*
  * @brief Pointer array to all the available system threads
  * The maximum number of threads is hard-coded. Alternatively, we could implement
  * a linked list which would mean using up less memory for a small number of
  * threads while allowing an unlimited number of possible threads. This would
  * probably not slow down thread switching too much, but it would introduce
  * complexity and possibly bugs. So to simplifiy for now, we use an array.
  * But in the future, a linked list might be more appropriate.
*/
thread_t *system_threads[MAX_THREADS];

// These variables are used by the assembly context_switch() function.
// They are copies or pointers to data in Threads and thread_t
// and put here seperately in order to simplify the code.
extern "C" {

/*
*   @brief Current count of system tick
*   @notes in TeensyThreads this was currentCount
*/
static int current_use_systick;      

/*
*   @brief The current thread in the program. 
*   @notes Comes preinitialized. In TeensyThreads this was currentActive
*/
int current_active_state;          

/*
*   @brief Current count of system tick
*   @notes in TeensyThreads this was currentCount
*/
int current_tick_count;

/*
*   @brief The current register stack on the program
*   @notes In TeensyThreads this was *currentThread
*/
static thread_t *current_thread;  

/*
* @notes inherited from TeensyThreads as currentSave
* @notes My current assumption is that this is the current program counter saved state of the thread
*/
void *current_save;

/*
* @brief Whether or not we have reached the top of the stack(aka stack overflow)
* @notes in TeensyThreads this was currentMSP, try to avoid getting here. 
*/
int current_msp;             

/*
*   @brief Current program thread stack pointer 
*   @notes in TeensyThreads this was currentSP
*/
void *current_sp;
  
}

/*
* @brief Main Thread zero stack. 
*/
extern unsigned long _estack;   // the main thread 0 stack

/*
*   @brief interrupt service routine that deals with saving the system tick
*/
os_isr_function_t save_systick_isr;

/*
*   @brief saving the supervisor call interrupt service routine
*/
os_isr_function_t save_svcall_isr;

/*
 * Store the PIT timer flag register for use in assembly
 */
volatile uint32_t *context_timer_flag;

/*
* @brief ISR that we can use later!
*/
extern "C" void unused_interrupt_vector(void);

/*
*   @brief subroutine called from assembly code to load next thread's context and registers
*   @notes Only to be used in assembly context switching code. Was loadNextThread in TeensyThreads 
*   @params none
*   @returns none
*/
extern "C" void load_next_thread_asm() {
  os_get_next_thread();
}

/*
* @brief ISR that triggers when we overflow our stack, our thread ends. 
*/
extern "C" void stack_overflow_default_isr() { 
  current_thread->flags = THREAD_ENDED;
}

/*
* @brief ISR helper function that helps trigger the end of our thread when stack overflows
*/
extern "C" void stack_overflow_isr(void)       __attribute__ ((weak, alias("stack_overflow_default_isr")));

/*
* @brief Declaration of system
*/
extern "C" void systick_isr();

/*
*   @brief Defined in assembly code
 */
extern "C" void context_switch_pit_isr();

/*
* @brief ISR that helps deal with intcrementing our system ticks
* @notes Pushes registers onto stack, deals with saving system tick, then pops registers back on!
*/
void __attribute((naked, noinline)) threads_systick_isr(void){
  if (save_systick_isr) {
    asm volatile("push {r0-r4,lr}");
    (*save_systick_isr)();
    asm volatile("pop {r0-r4,lr}");
  }
  if (current_use_systick) 
    __asm volatile("b context_switch");
  
  __asm volatile("bx lr");
}

/*
* @brief ISR dealing with the Supervisor call on the threads. 
* @notes Requires further investigation
*/
void __attribute((naked, noinline)) threads_svcall_isr(void){
  if (save_svcall_isr) {
    asm volatile("push {r0-r4,lr}");
    (*save_svcall_isr)();
    asm volatile("pop {r0-r4,lr}");
  }

  // Get the right stack so we can extract the PC (next instruction)
  // and then see the SVC calling instruction number
  __asm volatile("TST lr, #4 \n"
                 "ITE EQ \n"
                 "MRSEQ r0, msp \n"
                 "MRSNE r0, psp \n");
  register unsigned int *rsp __asm("r0");
  unsigned int svc = ((uint8_t*)rsp[6])[-2];
  if (svc == (WILL_OS_SVC_NUM)) {
    __asm volatile("b context_switch_direct");
  }
  else if (svc == WILL_OS_SVC_NUM_ACTIVE) {
    current_active_state = OS_STARTED;
    __asm volatile("b context_switch_direct_active");
  }
  __asm volatile("bx lr");
}

/*
* @brief General purpose timer built into Teensy4 that triggers context switch. 
* @notes Since there are potentially 2 spare timers, we have two almost exact copies of these ISR for each general purpose timer. just changing the clear set bit 
*/
static void __attribute((naked, noinline)) gpt1_isr() {
  GPT1_SR |= GPT_SR_OF1;  // clear set bit
  __asm volatile ("dsb"); // see github bug #20 by manitou48
  __asm volatile("b context_switch");
}

/*
* @brief General purpose timer built into Teensy4 that triggers context switch. 
* @notes Since there are potentially 2 spare timers, we have two almost exact copies of these ISR for each general purpose timer. just changing the clear set bit 
*/
static void __attribute((naked, noinline)) gpt2_isr() {
  GPT2_SR |= GPT_SR_OF1;  // clear set bit
  __asm volatile ("dsb"); // see github bug #20 by manitou48
  __asm volatile("b context_switch");
}

/*
*   @brief Intializes the unused General Purpose Timers in the Teensy 4. 
*   @notes if we can't set this up, then currently Teensy 4 will not initialize Will-OS
*   @params microseconds between each context switch
*/
bool t4_gpt_init(unsigned int microseconds){
  // Initialization code derived from @manitou48.
  // See https://github.com/manitou48/teensy4/blob/master/gpt_isr.ino
  // See https://forum.pjrc.com/threads/54265-Teensy-4-testing-mbed-NXP-MXRT1050-EVKB-(600-Mhz-M7)?p=193217&viewfull=1#post193217
  // keep track of which GPT timer we are using
  static int gpt_number = 0;
  // not configured yet, so find an inactive GPT timer
  if (gpt_number == 0) {
    if (! NVIC_IS_ENABLED(IRQ_GPT1)) {
      attachInterruptVector(IRQ_GPT1, &gpt1_isr);
      NVIC_SET_PRIORITY(IRQ_GPT1, 255);
      NVIC_ENABLE_IRQ(IRQ_GPT1);
      gpt_number = 1;
    }
    else if (! NVIC_IS_ENABLED(IRQ_GPT2)) {
      attachInterruptVector(IRQ_GPT2, &gpt2_isr);
      NVIC_SET_PRIORITY(IRQ_GPT2, 255);
      NVIC_ENABLE_IRQ(IRQ_GPT2);
      gpt_number = 2;
    }
    else {
      // if neither timer is free, we fail
      return false;
    }
  }
  switch (gpt_number) {
    case 1:
      CCM_CCGR1 |= CCM_CCGR1_GPT1_BUS(CCM_CCGR_ON) ;  // enable GPT1 module
      GPT1_CR = 0;                                    // disable timer
      GPT1_PR = 23;                                   // prescale: divide by 24 so 1 tick = 1 microsecond at 24MHz
      GPT1_OCR1 = microseconds - 1;                   // compare value
      GPT1_SR = 0x3F;                                 // clear all prior status
      GPT1_IR = GPT_IR_OF1IE;                         // use first timer
      GPT1_CR = GPT_CR_EN | GPT_CR_CLKSRC(1) ;        // set to peripheral clock (24MHz)
      break;
    case 2:
      CCM_CCGR1 |= CCM_CCGR1_GPT1_BUS(CCM_CCGR_ON) ;  // enable GPT1 module
      GPT2_CR = 0;                                    // disable timer
      GPT2_PR = 23;                                   // prescale: divide by 24 so 1 tick = 1 microsecond at 24MHz
      GPT2_OCR1 = microseconds - 1;                   // compare value
      GPT2_SR = 0x3F;                                 // clear all prior status
      GPT2_IR = GPT_IR_OF1IE;                         // use first timer
      GPT2_CR = GPT_CR_EN | GPT_CR_CLKSRC(1) ;        // set to peripheral clock (24MHz)
      break;
    
    // We weren't able to setup the GP1 module properly :(
    default:
      return false;
  }
  return true;
}

/*
*   @brief allows our program to "yield" out of current subroutine
*   @notes 
*/
extern "C" void _os_yield(void){
    __asm volatile("svc %0" : : "i"(WILL_OS_SVC_NUM));
}

/*
* @brief Sleeps the thread through a hypervisor call. 
* @notes Checks in roughly every milliscond until thread is ready to start THREAD_running again
* @params int milliseconds since last system tick
* @returns none
*/
extern void os_thread_delay_ms(int millisecond){
  int start_del = millis();
  // So let the hypervisor take us away from this thread, and check 
  // Each millisecond 
  while((int)millis() - start_del < millisecond)
    _os_yield();
}

/*
* @brief Sets up our zero thread. 
* @notes only to be called at setup
*/
void  __attribute__((always_inline)) os_setup_thread_zero(void){
  // fill thread 0, which is always THREAD_running
  system_threads[0] = new thread_t();
  system_threads[0]->flags = THREAD_RUNNING;
  system_threads[0]->ticks = OS_DEFAULT_TICKS;
  system_threads[0]->stack = (uint8_t*)&_estack - DEFAULT_STACK0_SIZE;
  system_threads[0]->stack_size = DEFAULT_STACK0_SIZE;
  thread_count++;
}

/*
* @brief Setting up the General purpose timers that we use for the teensy 4
* @notes 
*/
inline void __attribute__((always_inline))os_setup_t4_isr_timers(void){
  // commandeer SVCall & use GTP1 Interrupt
  save_svcall_isr = _VectorsRam[11];
  if (save_svcall_isr == unused_interrupt_vector) save_svcall_isr = 0;
  _VectorsRam[11] = threads_svcall_isr;

  current_use_systick = 0; // disable Systick calls
  t4_gpt_init(1000);       // tick every millisecond
}

/*
*   @brief Used to startup the Will-OS "Kernel" of sorts
*   @notes Must be called before you do any multithreading with the willos kernel
*   @params none
*   @returns none
*/
void threads_init(void){
  // initilize thread slots to THREAD_empty
  for(int i=0; i<MAX_THREADS; i++) {
    system_threads[i] = NULL;
  }

  os_setup_thread_zero();

  // initialize context_switch() globals from thread 0, which is MSP and always THREAD_running
  current_thread = system_threads[0];        // thread 0 is active
  current_save = &system_threads[0]->save;
  current_msp = 1;
  current_sp = 0;
  current_tick_count = OS_DEFAULT_TICKS;
  current_active_state = OS_FIRST_RUN;
  
  os_setup_t4_isr_timers();
}

/*
*   @brief Starts the entire Will-OS Kernel
*   @notes Try to avoid stopping the kernel whenever possible. 
*   @params none
*   @returns int original state of machine
*/
int os_start(int prev_state){
  __disable_irq();
  
  int old_state = current_active_state;
  
  if (prev_state == -1) 
    prev_state = OS_STARTED;

  current_active_state = prev_state;
  __enable_irq();
  
  return old_state;
}

/*
*   @brief Stops the entire Will-OS Kernel
*   @notes Try to avoid stopping the kernel whenever possible. 
*   @params none
*   @returns int original state of machine
*/
int os_stop(void) {
  __disable_irq();
  int old_state = current_active_state;
  current_active_state = OS_STOPPED;
  __enable_irq();
  return old_state;
}

/*
*   @brief Increments to next thread for context switching
*   @notes Not to be called externally!
*   @params  none
*   @returns none
*/
inline void os_get_next_thread() {

  // First, save the current_sp set by context_switch
  current_thread->sp = current_sp;

  // did we overflow the stack (don't check thread 0)?
  // allow an extra 8 bytes for a call to the ISR and one additional call or variable
  if (current_thread_id && ((uint8_t*)current_thread->sp - current_thread->stack <= 8)) {
    stack_overflow_isr();
  }

  // Find the next THREAD_running thread
  while(1) {
    current_thread_id++;
    if (current_thread_id >= MAX_THREADS) {
      current_thread_id = 0; // thread 0 is MSP; always active so return
      break;
    }
    if (system_threads[current_thread_id] && system_threads[current_thread_id]->flags == THREAD_RUNNING) break;
  }

  current_tick_count = system_threads[current_thread_id]->ticks;
  current_thread = system_threads[current_thread_id];
  current_save = &system_threads[current_thread_id]->save;
  current_msp = (current_thread_id==0?1:0);
  current_sp = system_threads[current_thread_id]->sp;
}

/*
*   @brief  deletes a thread from the system. 
*   @notes  be careful, since this also ends the system kernel and isr's
*   @params none
*   @return none
*/
void os_del_process(void){
  // Stopping the Will-OS system
  int old_state = os_stop();

  // Pointer to the thread we are using.
  thread_t *me = system_threads[current_thread_id];

  thread_count--;

  // Setting our current thread to an "ended" state
  me->flags = THREAD_ENDED; 

  // Restart the will-os kernel 
  os_start(old_state);

  // Just keep sitting until context change has been called by ISR again. 
  while(1); 
}

/*
*   @brief Given a thread pointer, arguements towards thread pointer, stack address to thread pointer, and stack size, we can setup the isr interrupts for thread. 
*   @notes Called whenever we add another thread. 
*   @params will_os_thread_func_t thread pointer to program counter start of thread
*   @params void *arg address pointer of arguements to pass into will-os
*   @params void *stack_addr pointer to the begining of the stack address. 
*   @params int stack_size size of the stack for this thread
*   @returns pointer to the new threadstack. 
*/
void *os_loadstack(thread_func_t p, void * arg, void *stackaddr, int stack_size){

  interrupt_stack_t * process_frame = (interrupt_stack_t *)((uint8_t*)stackaddr + stack_size - sizeof(interrupt_stack_t) - 8);
  // Clearing up and setting all the registers.
  process_frame->r0 = (uint32_t)arg;
  process_frame->r1 = 0;
  process_frame->r2 = 0;
  process_frame->r3 = 0;
  process_frame->r12 = 0;
  process_frame->lr = (uint32_t)os_del_process;
  process_frame->pc = ((uint32_t)p);
  process_frame->xpsr = 0x1000000;
  uint8_t *ret = (uint8_t*)process_frame;
  return (void*)ret;
}

/*
* @brief Adds a thread to Will-OS Kernel
* @notes Paralelism at it's finest!
* @params will_os_thread_func_t thread(pointer to thread function call begining of program counter)
* @params void *arg(pointer arguement to parameters for thread)
* @param void *stack(pointer to begining of thread stack)
* @param int stack_size(size of the allocated threadstack)
* @returns none
*/
os_thread_id_t os_add_thread(thread_func_t p, void * arg, int stack_size, void *stack){
  int old_state = os_stop();

  // If we don't allocate a valid stack size, then we just choose the default stack size
  if (stack_size == -1) 
    stack_size = OS_DEFAULT_STACK_SIZE;
  
  for (int i=1; i < MAX_THREADS; i++) {
    
    // If there is no thread(aka NULL pointer), then we will it up with a new unintialized spot
    if (system_threads[i] == NULL) 
      system_threads[i] = new thread_t();

    if (system_threads[i]->flags == THREAD_ENDED || system_threads[i]->flags == THREAD_EMPTY) { // free thread
      thread_t *tp = system_threads[i]; // working on this thread
      
      // If there was a previously allocated stack and it was allocated by the previous innstance 
      if (tp->stack && tp->my_stack) 
        delete[] tp->stack;
      
      // If there is no stack allocated, then we allocate our own
      if (stack == NULL) {
        stack = new uint8_t[stack_size];
        tp->my_stack = 1;
      }
      // Otherwise our stack is defined by something else!
      else 
        tp->my_stack = 0;

      // Preconfigure all the propper variables
      tp->stack = (uint8_t*)stack;
      tp->stack_size = stack_size;
      void *psp = os_loadstack(p, arg, tp->stack, tp->stack_size);
      tp->sp = psp;
      tp->ticks = OS_DEFAULT_TICKS;
      tp->flags = THREAD_RUNNING;
      tp->save.lr = 0xFFFFFFF9;

      current_active_state = old_state;
      thread_count++;
      
      // If the operating system was started before, we restart the OS
      if (old_state == OS_STARTED || old_state == OS_FIRST_RUN) 
        os_start();
      
      return i;
    }
  }
  
  if (old_state == OS_STARTED) 
    os_start();
  
  return -1;
}

/*
*   @brief Allows us to change the Will-OS System tick. 
*   @note If you want more precision in your system ticks, take care of this here. 
*   @params int tick_microseconds
*   @returns none
*/
bool os_set_microsecond_timer(int tick_microseconds){
  return t4_gpt_init(tick_microseconds);
}

/*
* @brief Sets the state of a thread to suspended. 
* @brief If thread doesn't exist, then 
* @params Which thread are we trying to get our state for
* @returns will_thread_state_t
*/
os_thread_id_t os_suspend_thread(os_thread_id_t target_thread_id){
  if(target_thread_id < thread_count){
    system_threads[target_thread_id]->flags = THREAD_SUSPENDED;   
    return target_thread_id;
  }
  // Otherwise tell system that thread doesn't exist. 
  return THREAD_DNE;  
}

/*
* @brief Sets the state of a thread to resumed. 
* @brief If thread doesn't exist or hasn't been run before, then 
* @params Which thread are we trying to get our state for
* @returns will_thread_state_t
*/
os_thread_id_t os_resume_thread(os_thread_id_t target_thread_id){
  if(target_thread_id < thread_count){
    system_threads[target_thread_id]->flags = THREAD_RUNNING;   
    return target_thread_id; 
  }
  // Otherwise tell system that thread doesn't exist. 
  return THREAD_DNE;  
}

/*
* @brief Sets the state of a thread to be killed 
* @brief If thread doesn't exist or hasn't been run before, then
* @notes One of the biggest limitations of this atm, is that we don't deallocate the thread stack space until we create another thread. 
* @notes in the future we should work on this 
* @notes Meaning  
* @params Which thread are we trying to get our state for
* @returns will_thread_state_t
*/
os_thread_id_t os_kill_thread(os_thread_id_t target_thread_id){
  if(target_thread_id < thread_count)
    system_threads[target_thread_id]->flags = THREAD_ENDED;   
  // Otherwise tell system that thread doesn't exist. 
  return THREAD_DNE;  
}

/*
* @brief Gets the state of a thread. 
* @brief If thread doesn't exist, then 
* @params Which thread are we trying to get our state for
* @returns will_thread_state_t
*/
thread_state_t os_get_thread_state(os_thread_id_t target_thread_id){
  if(target_thread_id < thread_count)
    return system_threads[target_thread_id]->flags;
  return THREAD_DNE;
}

/*
* @returns The current thread's ID. 
*/
os_thread_id_t os_current_id(void){
  return current_thread_id; 
}

/*
* @returns the current remaining stack left for a thread, based off their ID. 
*/
int os_get_stack_used(os_thread_id_t target_thread_id){
  if(target_thread_id < thread_count)
    return system_threads[target_thread_id]->stack + system_threads[target_thread_id]->stack_size - (uint8_t*)system_threads[target_thread_id]->sp;
  return -1; 
}

/*
* @brief Allows us to send signals to each thread by setting a bitmask
* @notes This uses preset flags to allow us to set and clear clags in a thread
* @params thread_signal_t thread_signal(there are 32 thread signals per thread)
*/
void os_thread_signal(thread_signal_t thread_signal){
  system_threads[current_thread_id]->thread_set_flags |= (1 << (uint32_t)thread_signal); 
}

/*
* @brief Allows us to send signals to each thread by clearing a bitmask
* @notes This uses preset flags to allow us to set and clear clags in a thread
* @params thread_signal_t thread_signal(there are 32 thread signals per thread)
*/
void os_thread_clear(thread_signal_t thread_signal){
  system_threads[current_thread_id]->thread_set_flags &= ~(1 << (uint32_t)thread_signal);
}

/*
* @brief Allows us to send signals to each thread by setting a bitmask
* @notes This uses preset flags to allow us to set and clear clags in a thread
* @params thread_signal_t thread_signal(there are 32 thread signals per thread)
* @params os_thread_id_t target_thread_id which thread we want to signal
*/
bool os_signal_thread(thread_signal_t thread_signal, os_thread_id_t target_thread_id){
  if(target_thread_id < thread_count){
    system_threads[target_thread_id]->thread_set_flags |= (1 << (uint32_t)thread_signal); 
    return true; 
  }


  return false; 
}

/*
* @brief Allows us to send signals to each thread by clearing a bitmask
* @notes This uses preset flags to allow us to set and clear clags in a thread
* @params thread_signal_t thread_signal(there are 32 thread signals per thread)
* @params os_thread_id_t target_thread_id in which we want to clear flags
*/
bool os_signal_thread_clear(thread_signal_t thread_signal, os_thread_id_t target_thread_id){
  if(target_thread_id < thread_count){
    system_threads[target_thread_id]->thread_set_flags &= ~(1 << (uint32_t)thread_signal);
    return true; 
  }
  return false; 
}

/*
* @brief We can check if there are bits that are signaled
* @params thread_signal which bits we want to check,
* @params target_thread_id which thread we 
* @return if those bits are set or not
*/  
thread_signal_status_t os_checkbits_thread(thread_signal_t thread_signal, os_thread_id_t target_thread_id){
  if(target_thread_id < thread_count){
    if(OS_CHECK_BIT(system_threads[target_thread_id]->thread_set_flags, (uint32_t)thread_signal))
      return THREAD_SIGNAL_SET; 
    return THREAD_SIGNAL_CLEAR; 
  }
  return THREAD_SIGNAL_DNE;
}

/*
* @brief We can check if there are bits that are signaled
* @params which bits we want to check,
* @return if those bits are set or not
*/  
thread_signal_status_t os_thread_checkbits(thread_signal_t thread_signal){
  if(OS_CHECK_BIT(system_threads[current_thread_id]->thread_set_flags, (uint32_t)thread_signal))
      return THREAD_SIGNAL_SET; 
    return THREAD_SIGNAL_CLEAR; 
}

/*
* @brief Hangs thread until either timeout or until thread signal bits have been set
* @notes Best not to 
*
*/
thread_signal_status_t os_thread_waitbits(thread_signal_t thread_signal, uint32_t timeout_ms){
  if(os_thread_checkbits(thread_signal) == THREAD_SIGNAL_SET)
    return THREAD_SIGNAL_SET; 

  uint32_t start = millis();
  
  while(1){
    if(os_thread_checkbits(thread_signal) == THREAD_SIGNAL_SET)
      return THREAD_SIGNAL_SET;
  
    if(timeout_ms && (millis() - start > timeout_ms))
      return THREAD_SIGNAL_TIMEOUT;
  
    _os_yield();
  }
}

/*
* @brief Hangs thread until signal has been cleared, no timeout
* @params thread_signal_t thread_signal
*/
void os_thread_waitbits_notimeout(thread_signal_t thread_signal){
  while(1){
    if(os_thread_checkbits(thread_signal) == THREAD_SIGNAL_SET)
      return; 
    _os_yield();  
  }
}