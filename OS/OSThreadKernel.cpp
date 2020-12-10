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

/*!
* @brief File was heavily refractored and re-documented by William Redenbaugh in 2020. As much as I've gone through and flushed out the 
* code as best as I could, props to the original creator of the TeensyThreads system. I heavily refractored it to match what I want my systeh
* to do, but the code is still entirely based of Fernando Trias, so props to that guy! Check out his stuff. 
*
*/


#include "OSThreadKernel.h"


/*!
* @brief Thread that calculates remainder stuff. 
* @param void *params
*/
static void remainder_thread(void *params); 

/*!
* @brief Remaind thread stack space and statically allocated array 
*/
static const int remainder_thread_stack_space = 256; 
uint8_t remainder_thread_stack[remainder_thread_stack_space]; 

/*!
*   @brief Current thread that we have context of.
*   @note This was contained in TeensyThreads' Thread class as current_thread
*/
os_thread_id_t current_thread_id = 0 ;

/*!
*   @brief Keeps track of how many threads we have at the moment
*   @note This was contained in TeensyThreads' Thread class as thread_count
*/
int thread_count = 0;

/*!
* @brief The default stack size of a thread being generated on the OS
* @note Can be defined as a preprocessor command
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

extern volatile uint32_t systick_millis_count;

/*!
  * @brief Pointer array to all the available system threads
  * The maximum number of threads is hard-coded. Alternatively, we could implement
  * a linked list which would mean using up less memory for a small number of
  * threads while allowing an unlimited number of possible threads. This would
  * probably not slow down thread switching too much, but it would introduce
  * complexity and possibly bugs. So to simplifiy for now, we use an array.
  * But in the future, a linked list might be more appropriate.
*/
static thread_t system_threads[MAX_THREADS];

/*!
* @brief Thread priority object that allows us to have an organized thread priority 
*/
static PriorityQueuePointerNaive thread_priorities; 

// These variables are used by the assembly context_switch() function.
// They are copies or pointers to data in Threads and thread_t
// and put here seperately in order to simplify the code.
extern "C" {

  /*!
  *   @brief Current count of system tick
  *   @note in TeensyThreads this was currentCount
  */
  static int current_use_systick;      

  /*!
  *   @brief The current thread in the program. 
  *   @note Comes preinitialized. In TeensyThreads this was currentActive
  */
  int current_active_state;          

  /*!
  *   @brief Current count of system tick
  *   @note in TeensyThreads this was currentCount
  */
  int current_tick_count;

  /*!
  *   @brief The current register stack on the program
  *   @note In TeensyThreads this was *currentThread
  */
  static thread_t *current_thread;  

  /*!
  * @note inherited from TeensyThreads as currentSave
  * @note This points to the saved stack of the registers for the thread. 
  */
  void *current_save;

  /*!
  * @brief Whether or not we have reached the top of the stack(aka stack overflow)
  * @note in TeensyThreads this was currentMSP, try to avoid getting here. 
  */
  int current_msp;             

  /*!
  *   @brief Current program thread stack pointer 
  *   @note in TeensyThreads this was currentSP
  */
  void *current_sp;
  
}

/*!
* @brief Main Thread zero stack. 
*/
extern unsigned long _estack;   // the main thread 0 stack

/*!
*   @brief interrupt service routine that deals with saving the system tick
*/
os_isr_function_t save_systick_isr;

/*!
*   @brief saving the supervisor call interrupt service routine
*/
os_isr_function_t save_svcall_isr;

/*
 * Store the PIT timer flag register for use in assembly
 */
volatile uint32_t *context_timer_flag;

#if defined(__IMXRT1062__)

/*!
* @brief General purpose timer built into Teensy4 that triggers context switch. 
* @note Since there are potentially 2 spare timers, we have two almost exact copies of these ISR for each general purpose timer. just changing the clear set bit 
*/
static void __attribute((naked, noinline)) gpt1_isr() {
  GPT1_SR |= GPT_SR_OF1;  // clear set bit
  __asm volatile ("dsb"); // see github bug #20 by manitou48
  __asm volatile("b context_switch");
}

/*!
* @brief General purpose timer built into Teensy4 that triggers context switch. 
* @note Since there are potentially 2 spare timers, we have two almost exact copies of these ISR for each general purpose timer. just changing the clear set bit 
*/
static void __attribute((naked, noinline)) gpt2_isr() {
  GPT2_SR |= GPT_SR_OF1;  // clear set bit
  __asm volatile ("dsb"); // see github bug #20 by manitou48
  __asm volatile("b context_switch");
}

/*!
*   @brief Intializes the unused General Purpose Timers in the Teensy 4. 
*   @note if we can't set this up, then currently Teensy 4 will not initialize Will-OS
*   @param microseconds between each context switch
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

#endif

/*!
* @brief ISR that we can use later!
*/
extern "C" void unused_interrupt_vector(void);

/*!
*   @brief subroutine called from assembly code to load next thread's context and registers
*   @note Only to be used in assembly context switching code. Was loadNextThread in TeensyThreads 
*   @param none
*   @returns none
*/
extern "C" void load_next_thread_asm() {
  os_get_next_thread();
}

/*!
* @brief ISR that triggers when we overflow our stack, our thread ends. 
*/
extern "C" void stack_overflow_default_isr() { 
  current_thread->flags = THREAD_ENDED;
}

/*!
* @brief ISR helper function that helps trigger the end of our thread when stack overflows
*/
extern "C" void stack_overflow_isr(void)       __attribute__ ((weak, alias("stack_overflow_default_isr")));

/*!
* @brief Declaration of system
*/
extern "C" void systick_isr();

/*!
*   @note Defined in assembly code
 */
extern "C" void context_switch_pit_isr();

/*!
* @brief ISR that helps deal with intcrementing our system ticks
* @note Pushes registers onto stack, deals with saving system tick, then pops registers back on!
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

/*!
* @brief ISR dealing with the Supervisor call on the threads. 
* @note Requires further investigation
*/
extern "C" void SVC_Handler (void){
  // Get the right stack so we can extract the PC (next instruction)
  // and then see the SVC calling instruction number
  
  __asm volatile("TST lr, #4 \n"
                 "ITE EQ \n"
                 "MRSEQ r0, msp \n"
                 "MRSNE r0, psp \n");

  __asm volatile("b context_switch_direct");
  __asm volatile("bx lr");
}

/*!
*   @brief allows our program to "yield" out of current subroutine
*   @note 
*/
extern "C" void _os_yield(void){
  
  //threads_svcall_isr(); 
  __asm volatile("svc %0" : : "i"(WILL_OS_SVC_NUM));
}

/*!
* @brief Sleeps the thread through a hypervisor call. 
* @note Sleeps the thread for the alloted time, and wakes up once the thread is ready
* @param int milliseconds since last system tick
* @returns none
*/
extern void os_thread_sleep_ms(int millisecond){
  int start_del = millis();

  // So the operating system knows when to start back up the next thread. 
  current_thread->interval = millisecond; 
  current_thread->previous_millis = millis(); 

  // Signals that thread is sleeping, and must be awoken once ready. 
  current_thread->flags = THREAD_SLEEPING; 

  _os_yield(); 
}

/*!
* @brief Sets up our zero thread. 
* @note only to be called at setup
* @note We do still setup the thread, but we leave it alone, as there seems to be an issue with this thread. Will look into further into the future
*/
inline void os_setup_thread_zero(void){
  // fill thread 0, which is always THREAD_running
  // system_threads[0] = new thread_t();
  system_threads[0].flags = THREAD_EMPTY;
  //system_threads[0].ticks = OS_DEFAULT_TICKS;
  //system_threads[0].stack = (uint8_t*)&_estack - DEFAULT_STACK0_SIZE;
  system_threads[0].stack_size = DEFAULT_STACK0_SIZE;
  thread_count++;
}

/*!
*   @brief Used to startup the Will-OS "Kernel" of sorts
*   @note Must be called before you do any multithreading with the willos kernel
*   @param none
*   @returns none
*/
void threads_init(void){
  // initilize thread slots to THREAD_empty
  for(int i=0; i<MAX_THREADS; i++) 
    system_threads[i].flags = THREAD_EMPTY; 

  os_setup_thread_zero();

  // initialize context_switch() globals from thread 0, which is MSP and always THREAD_running
  current_thread = &system_threads[0];        // thread 0 is active
  current_save = &system_threads[0].save;
  current_msp = 1;
  current_sp = 0;
  current_tick_count = OS_DEFAULT_TICKS;
  current_active_state = OS_FIRST_RUN;

  //Code to get teensy working properly. 
#if defined(__IMXRT1062__)
  save_svcall_isr = _VectorsRam[11];
  if (save_svcall_isr == unused_interrupt_vector) save_svcall_isr = 0;
  _VectorsRam[11] = SVC_Handler; 
  //current_use_systick = 0; // disable Systick calls
  //t4_gpt_init(200);       // tick every millisecond
#endif
  os_add_thread(&remainder_thread, NULL, 0, remainder_thread_stack_space, remainder_thread_stack);

// If we want the void loop thread to still work
#if defined(ARDUINO_LOOP_THREAD)
  os_add_thread(&loop, NULL, 254, NULL, -1); 
#endif
}

/*!
*   @brief Starts the entire Will-OS Kernel
*   @note Try to avoid stopping the kernel whenever possible. 
*   @note Most important when you have a preemptive kernel, as it enables interrupt actions
*   @param none
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

/*!
*   @brief Stops the entire Will-OS Kernel
*   @note Try to avoid stopping the kernel for as short of operations as possible 
*   @note Most important when you have a premptive kernel, as it disables interrupt actions.
*   @param none
*   @returns int original state of machine
*/
int os_stop(void) {
  __disable_irq();
  int old_state = current_active_state;
  current_active_state = OS_STOPPED;
  __enable_irq();
  return old_state;
} 

/*!
* @brief Checks a thread, and based of it's flags will change it's behavior. 
* @note If the scheduler sees a thread sleeping, it's going to see if it needs to be woken up
* @note If the scheduler sees a thread blocked via a mutex that has timed out, then it's going to remove that thread from it's respective semaphore and set it as thread running
* @note If the scheduler sees a thread blocked via a mutex that either has no timeout or hasn't reached it's timeout BUT has an unlocked semaphore. 
*/
static inline void check_thread_flags(thread_t *thread){
  switch (thread->flags)
  {
  case THREAD_SLEEPING:
    // If the thread was sleeping, and we are waiting on the thread to complete. 
    if((millis() - thread->previous_millis) >= thread->interval)
      thread->flags = THREAD_RUNNING; // We wake up the thread. 
    break;
  
  case THREAD_BLOCKED_SEMAPHORE: 
    // If there are entrants available for the semaphore, then we run the thread. 
    if(thread->semaphore_max_count >= *thread->mutex_semaphore)
      thread->flags = THREAD_RUNNING; 
    break; 

  case THREAD_BLOCKED_SEMAPHORE_TIMEOUT:
    // If there are entrants available or our timeout has occoured
    if(thread->semaphore_max_count >= *thread->mutex_semaphore || ((millis() - thread->previous_millis) >= thread->interval))
      thread->flags = THREAD_RUNNING;
    break;

  case THREAD_BLOCKED_MUTEX: 
    // If the mutex has been release, then we grab it for the thread
    if(thread->mutex_semaphore == 0)
      thread->flags = THREAD_RUNNING; 
    break; 

  case THREAD_BLOCKED_MUTEX_TIMEOUT: 
    // If the mutex has been released or the timeout has occoured, we take care of the thread. 
    if(thread->mutex_semaphore == 0 || ((millis() - thread->previous_millis) >= thread->interval))
      thread->flags = THREAD_RUNNING; 
    break; 

  case THREAD_BLOCKED_SIGNAL: 
    // If a thread is blocked by a signal and waiting for a particular bit to be set, we run the next thread!
    if(thread->signal_bits_compare && *thread->signal_bit)
      thread->flags = THREAD_RUNNING; 
    break; 

  case THREAD_BLOCKED_SIGNAL_TIMEOUT: 
    // If either the thread times out or the signal is set, we run this as next thread. 
    if(thread->signal_bits_compare && *thread->signal_bit || ((millis() - thread->previous_millis) >= thread->interval))
      thread->flags = THREAD_RUNNING; 
    break; 
  
  default: 
    break;
  }
}

/*!
*   @brief Increments to next thread for context switching
*   @note Not to be called externally!
*   @param  none
*   @returns none
*/
inline void os_get_next_thread() {

  // First, save the current_sp set by context_switch
  current_thread->sp = current_sp;

  // did we overflow the stack (don't check thread 0)?
  // allow an extra 8 bytes for a call to the ISR and one additional call or variable
  if (current_thread_id && ((uint8_t*)current_thread->sp - current_thread->stack <= 8)) 
    stack_overflow_isr();

  static PriorityQueueNaiveNode *current_node; 
  
  // Whichever thread we want to use.
  thread_t *thread; 

  // We look at the highest priority node first
  current_node = thread_priorities.peek_top_node();

  // Iterate through until we have a thread we need to pick up
  while(1){
    // Casting general pointer as a thread pointer
    thread = (thread_t*)current_node->ptr; 

    // Checking to see what the threads are doing. 
    check_thread_flags(thread); 

    // The highest priority thread runs first!
    if(thread->flags == THREAD_RUNNING)
      break; 
    
    // If nothing works out, we move on to the next thread. 
    current_node = current_node->next; 
  } 

  // So the astute may realize here, that there's no termination code. 
  // The reason is because the lowest priority thread is the remainder thread, and that thread does literally nothing but exit out 
  // And check for something that's ready again
 
  // Load up all the important registers from memory back into the operating system.
  current_tick_count = thread->ticks; 
  current_thread = thread; 
  current_save = &(thread->save); 
  current_msp = 0; 
  current_sp = thread->sp; 
}

/*!
*   @brief  deletes a thread from the system. 
*   @note  be careful, since this also ends the system kernel and isr's
*   @param none
*   @return none
*/
void os_del_process(void){
  // Stopping the Will-OS system
  int old_state = os_stop();

  // Pointer to the thread we are using.
  thread_t *me = &system_threads[current_thread_id];

  thread_count--;

  // Setting our current thread to an "ended" state
  me->flags = THREAD_ENDED; 

  // Restart the will-os kernel 
  os_start(old_state);

  // Just keep sitting until context change has been called by ISR again. 
  while(1); 
}

/*!
*   @brief Given a thread pointer, arguements towards thread pointer, stack address to thread pointer, and stack size, we can setup the isr interrupts for thread. 
*   @note Called whenever we add another thread. 
*   @param will_os_thread_func_t thread pointer to program counter start of thread
*   @param void *arg address pointer of arguements to pass into will-os
*   @param void *stack_addr pointer to the begining of the stack address. 
*   @param int stack_size size of the stack for this thread
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

/*!
* @brief Adds a thread to Will-OS Kernel
* @note Paralelism at it's finest!
* @param will_os_thread_func_t thread(pointer to thread function call begining of program counter)
* @param void *arg(pointer arguement to parameters for thread)
* @param void *stack(pointer to begining of thread stack)
* @param int stack_size(size of the allocated threadstack)
* @returns none
*/
os_thread_id_t os_add_thread(thread_func_t p, void * arg, uint8_t thread_priority, int stack_size, void *stack){
  int old_state = os_stop();

  // If we don't allocate a valid stack size, then we just choose the default stack size
  if (stack_size == -1) 
    stack_size = OS_DEFAULT_STACK_SIZE;
  
  for (int i=1; i < MAX_THREADS; i++) {
    
    if (system_threads[i].flags == THREAD_ENDED || system_threads[i].flags == THREAD_EMPTY) { // free thread
      thread_t *tp = &system_threads[i]; // working on this thread
      
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
      
      // How important is this thread? 
      // Note that since we put that in the priority thread, 
      tp->thread_priority = thread_priority; 
      
      // If the operating system was started before, we restart the OS
      if (old_state == OS_STARTED || old_state == OS_FIRST_RUN) 
        os_start();

      // Inserts thread into a priority heap, allowing us to organize threads by priority.       
      thread_priorities.insert((void*)tp, (uint16_t)thread_priority); 

      return i;
    }
  }
  
  if (old_state == OS_STARTED) 
    os_start();
  
  return -1;
}

/*!
* @brief Adds a thread to Will-OS Kernel
* @note Paralelism at it's finest!
* @param will_os_thread_func_t thread(pointer to thread function call begining of program counter)
* @param void *arg(pointer arguement to parameters for thread)
* @param void *stack(pointer to begining of thread stack)
* @param int stack_size(size of the allocated threadstack)
* @returns none
*/
os_thread_id_t os_add_thread(thread_func_t p, void * arg, int stack_size, void *stack){return os_add_thread(p, arg, 128, stack_size, stack);}

/*!
*   @brief Allows us to change the Will-OS System tick. 
*   @note If you want more precision in your system ticks, take care of this here. 
*   @param int tick_microseconds
*   @returns none
*/
bool os_set_microsecond_timer(int tick_microseconds){
  //return t4_gpt_init(tick_microseconds);

  return false; 
  
}

/*!
* @brief Sets the state of a thread to suspended. 
* @brief If thread doesn't exist, then 
* @param Which thread are we trying to get our state for
* @returns os_thread_id_t
*/
os_thread_id_t os_suspend_thread(os_thread_id_t target_thread_id){
  if(target_thread_id < thread_count){
    system_threads[target_thread_id].flags = THREAD_SUSPENDED;   
    return target_thread_id;
  }
  // Otherwise tell system that thread doesn't exist. 
  return THREAD_DNE;  
}

/*!
* @brief Sets the state of a thread to resumed. 
* @brief If thread doesn't exist or hasn't been run before, then 
* @param Which thread are we trying to get our state for
* @returns os_thread_id_t
*/
os_thread_id_t os_resume_thread(os_thread_id_t target_thread_id){
  if(target_thread_id < thread_count){
    system_threads[target_thread_id].flags = THREAD_RUNNING;   
    return target_thread_id; 
  }
  // Otherwise tell system that thread doesn't exist. 
  return THREAD_DNE;  
}

/*!
* @brief Sets the state of a thread to be killed 
* @brief If thread doesn't exist or hasn't been run before, then
* @note One of the biggest limitations of this atm, is that we don't deallocate the thread stack space until we create another thread. 
* @note in the future we should work on this 
* @note Meaning  
* @param Which thread are we trying to get our state for
* @returns os_thread_id_t
*/
os_thread_id_t os_kill_thread(os_thread_id_t target_thread_id){
  if(target_thread_id < thread_count)
    system_threads[target_thread_id].flags = THREAD_ENDED;   
  // Otherwise tell system that thread doesn't exist. 
  return THREAD_DNE;  
}

/*!
* @brief Gets the state of a thread. 
* @brief If thread doesn't exist, then 
* @param Which thread are we trying to get our state for
* @returns os_thread_id_t
*/
thread_state_t os_get_thread_state(os_thread_id_t target_thread_id){
  if(target_thread_id < thread_count)
    return system_threads[target_thread_id].flags;
  return THREAD_DNE;
}

/*!
* @returns The current thread's ID. 
*/
os_thread_id_t os_current_id(void){
  return current_thread_id; 
}

/*!
* @return Current pointer to thread information
*/
thread_t *_os_current_thread(void){
  return current_thread; 
}

/*!
* @returns the current remaining stack left for a thread, based off their ID. 
*/
int os_get_stack_used(os_thread_id_t target_thread_id){
  if(target_thread_id < thread_count)
    return system_threads[target_thread_id].stack + system_threads[target_thread_id].stack_size - (uint8_t*)system_threads[target_thread_id].sp;
  return -1; 
}

/*!
* @brief Allows us to send signals to each thread by setting a bitmask
* @note This uses preset flags to allow us to set and clear clags in a thread
* @param thread_signal_t thread_signal(there are 32 thread signals per thread)
*/
void os_thread_signal(thread_signal_t thread_signal){
  system_threads[current_thread_id].thread_set_flags |= (1 << (uint32_t)thread_signal); 
}

/*!
* @brief Allows us to send signals to each thread by clearing a bitmask
* @note This uses preset flags to allow us to set and clear clags in a thread
* @param thread_signal_t thread_signal(there are 32 thread signals per thread)
*/
void os_thread_clear(thread_signal_t thread_signal){
  system_threads[current_thread_id].thread_set_flags &= ~(1 << (uint32_t)thread_signal);
}

/*!
* @brief Allows us to send signals to each thread by setting a bitmask
* @note This uses preset flags to allow us to set and clear clags in a thread
* @param thread_signal_t thread_signal(there are 32 thread signals per thread)
* @param os_thread_id_t target_thread_id which thread we want to signal
*/
bool os_signal_thread(thread_signal_t thread_signal, os_thread_id_t target_thread_id){
  if(target_thread_id < thread_count){
    system_threads[target_thread_id].thread_set_flags |= (1 << (uint32_t)thread_signal); 
    return true; 
  }
  return false; 
}

/*!
* @brief Allows us to send signals to each thread by clearing a bitmask
* @note This uses preset flags to allow us to set and clear clags in a thread
* @param thread_signal_t thread_signal(there are 32 thread signals per thread)
* @param os_thread_id_t target_thread_id in which we want to clear flags
*/
bool os_signal_thread_clear(thread_signal_t thread_signal, os_thread_id_t target_thread_id){
  if(target_thread_id < thread_count){
    system_threads[target_thread_id].thread_set_flags &= ~(1 << (uint32_t)thread_signal);
    return true; 
  }
  return false; 
}

/*!
* @brief We can check if there are bits that are signaled
* @param thread_signal which bits we want to check,
* @param target_thread_id which thread we 
* @return if those bits are set or not
*/  
thread_signal_status_t os_checkbits_thread(thread_signal_t thread_signal, os_thread_id_t target_thread_id){
  if(target_thread_id < thread_count){
    if(OS_CHECK_BIT(system_threads[target_thread_id].thread_set_flags, (uint32_t)thread_signal))
      return THREAD_SIGNAL_SET; 
    return THREAD_SIGNAL_CLEAR; 
  }
  return THREAD_SIGNAL_DNE;
}

/*!
* @brief We can check if there are bits that are signaled
* @param which bits we want to check,
* @return if those bits are set or not
*/  
thread_signal_status_t os_thread_checkbits(thread_signal_t thread_signal){
  if(OS_CHECK_BIT(system_threads[current_thread_id].thread_set_flags, (uint32_t)thread_signal))
      return THREAD_SIGNAL_SET; 
    return THREAD_SIGNAL_CLEAR; 
}

/*!
* @brief Hangs thread until either timeout or until thread signal bits have been set
* @note Best not to 
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

/*!
* @brief Hangs thread until signal has been cleared, no timeout
* @param thread_signal_t thread_signal
*/
void os_thread_waitbits_notimeout(thread_signal_t thread_signal){
  while(1){
    if(os_thread_checkbits(thread_signal) == THREAD_SIGNAL_SET)
      return; 
    _os_yield();  
  }
}

/*!
* @brief Thread that calculates remainder stuff, and sits around 
* @param void *params
*/
static void remainder_thread(void *params){
  for(;;){
    _os_yield();
  } 
}