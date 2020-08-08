/*
* By: William Redenbuagh
* Heavily Derived from TeensyThreads by Fernando Trias. Many pieces of the code are 1:1. So you have that guy to thank!
*/
#include "will_os.h"

// Currenly the only platform supported by Will-OS is the Teensy 4
#ifdef __IMXRT1062__


/*
*   @brief Ensures that all memory access appearing before this program point are taken care of.   
*   @notes To understand more, visit: https://www.keil.com/support/man/docs/armasm/armasm_dom1361289870356.htm
*/
#define flush_cpu_mem_pipeline() __asm__ volatile("DMB");

/*
*   @brief Supervisor call number, used for yielding the program
*/
const int WILL_OS_SVC_NUM = 33; 

/*
*   @brief allows our program to "yield" out of current subroutine
*   @notes 
*/
extern "C" void will_os_yield(void){
    __asm volatile("svc %0" : : "i"(WILL_OS_SVC_NUM));
}

/*
*   @brief Supervisor call number, used for yielding the program
*/
const int WILL_OS_SVC_NUM_ACTIVE = 34; 

/*
*   @brief allows our program to "yield" out of current subroutine
*   @notes 
*/
extern "C" void will_os_yield_start(void){
    __asm volatile("svc %0" : : "i"(WILL_OS_SVC_NUM_ACTIVE));
}

/*
*   @brief Default amount of ticks
*/
int WILL_OS_DEFAULT_TICKS = 10; 

/*
*   @brief Default stack size of thread zero, set to 2^14 bytes
*/
const int WILL_OS_DEFAULT_STACK0_SIZE = 16384;

/*
*   @brief Default stack size of any thread should non be provided
*/
int WILL_OS_DEFAULT_STACK_SIZE = 2048;

/*
 * @brief Use unused Teensy4 GPT timers for context switching
 */
extern "C" void unused_interrupt_vector(void);

/*
*   @brief GPT1 ISR function, which calls on timer call
*   @notes Used exclusively for ISR context switching purposes.
*/
static void __attribute((naked, noinline)) gpt1_isr() {
  GPT1_SR |= GPT_SR_OF1;                // Clear set bit.
  __asm volatile ("dsb");               // See github bug #20 by manitou48.
  __asm volatile("b will_os_context_switch");   // Branch to context switch assembly code. 
}

/*
*   @brief GPT1 ISR function, which calls on timer call
*   @notes Used exclusively for ISR context switching purposes.
*/
static void __attribute((naked, noinline)) gpt2_isr() {
  GPT2_SR |= GPT_SR_OF1;                // Clear set bit.
  __asm volatile ("dsb");               // see github bug #20 by manitou48
  __asm volatile("b will_os_context_switch");   // Branch to context switch assembly code.
}

/*
*   @brief Intializes the unused General Purpose Timers in the Teensy 4. 
*   @notes if we can't set this up, then currently Teensy 4 will not initialize Will-OS
*   @params microseconds between each context switch
*/
bool t4_unused_gpt_init(unsigned int microseconds){

  // Initialization code derived from @manitou48.
  // See https://github.com/manitou48/teensy4/blob/master/gpt_isr.ino
  // See https://forum.pjrc.com/threads/54265-Teensy-4-testing-mbed-NXP-MXRT1050-EVKB-(600-Mhz-M7)?p=193217&viewfull=1#post193217

  // Keep track of which GPT timer we are using.
  static int gpt_number = 0;

  // Not configured yet, so find an inactive GPT timer.
  if (gpt_number == 0) {
    // Not sure if we
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
      // If neither timer is free, we fail.
      return false;
    }
  }

    // Which GPT timer we are going to configure. 
    switch (gpt_number){
    case 1:
      CCM_CCGR1 |= CCM_CCGR1_GPT1_BUS(CCM_CCGR_ON) ;  // Enable GPT1 module.
      GPT1_CR = 0;                                    // Disable timer.
      GPT1_PR = 23;                                   // Prescale: divide by 24 so 1 tick = 1 microsecond at 24MHz.
      GPT1_OCR1 = microseconds - 1;                   // Compare value.
      GPT1_SR = 0x3F;                                 // Clear all prior status.
      GPT1_IR = GPT_IR_OF1IE;                         // Use first timer.
      GPT1_CR = GPT_CR_EN | GPT_CR_CLKSRC(1) ;        // Set to peripheral clock (24MHz).
      break;
    case 2:
      CCM_CCGR1 |= CCM_CCGR1_GPT1_BUS(CCM_CCGR_ON) ;  // Enable GPT1 module.
      GPT2_CR = 0;                                    // Disable timer.
      GPT2_PR = 23;                                   // Prescale: divide by 24 so 1 tick = 1 microsecond at 24MHz.
      GPT2_OCR1 = microseconds - 1;                   // Compare value.
      GPT2_SR = 0x3F;                                 // Clear all prior status.
      GPT2_IR = GPT_IR_OF1IE;                         // Use first timer.
      GPT2_CR = GPT_CR_EN | GPT_CR_CLKSRC(1) ;        // Set to peripheral clock (24MHz).
      break;

    // We weren't able to setup the GP1 module properly :(
    default:
      return false;
  }
  return true;
}

/*
*   @brief Pointer array to all the system threads
*/
thread_t *system_threads[MAX_WILL_THREADS];

extern "C"{

void will_os_context_switch_pit_isr();

volatile uint32_t *will_os_context_timer_flag;

/*
*   @brief subroutine called from assembly code to load next thread's context and registers
*   @notes Only to be used in assembly context switching code. Was loadNextThread in TeensyThreads 
*   @params none
*   @returns none
*/
void load_next_thread_asm_call(void){
  // Wraps around this
  will_os_load_next_thread();
}

/*
*   @brief The current thread in the program. 
*   @notes Comes preinitialized. In TeensyThreads this was currentActive
*/
int current_state = WILL_OS_UNINITIALIZED; 

/*
*   @brief The current register stack on the program
*   @notes In TeensyThreads this was *currentThread
*/
thread_t *current_thread; 

/*
* @brief Main Thread zero stack. 
*/
extern unsigned long _estack; 

/*
*   @brief interrupt service routine that deals with saving the system tick
*/
will_os_isr_func_t save_systick_isr; 

/*
*   @brief saving the supervisor call interrupt service routine
*/
will_os_isr_func_t save_svcall_isr; 


/*
*   @brief Current System Tick   
*   @notes In TeensyThreads this was currentUseSystick for the gpt timer
*/
int current_use_systick; 

/*
*   @brief Current count of system tick
*   @notes in TeensyThreads this was currentCount
*/
int current_tick_count = 10; 

/*
*   @brief Current program thread stack pointer 
*   @notes Updated everytime we enter and leave threads. 
*   @notes in TeensyThreads this was currentSP
*/
void *current_stack_pointer; 

/*
* @brief maximum value that stack pointer can be
* @notes in TeensyThreads this was currentMSP
*/
int current_max_stack_pointer; 

/*
* @notes inherited from TeensyThreads as currentSave
* @notes My current assumption is that this is the current program counter saved state of the thread
*/
void *current_save; 

}

/*
*   @brief If our current thread's stack overflows, then we kill the thread
*   @notes Please try your best to void this behavior. 
*/
extern "C" void will_os_stack_overflow_default_isr() { 
  current_thread->flags = WILL_THREAD_STATE_ENDED;
}

/*
*   @notes inherited from teensyThread, seems to link to the above function
*/
extern "C" void will_os_stack_overflow_isr(void)       __attribute__ ((weak, alias("will_os_stack_overflow_default_isr")));

/*
*   @brief Current thread that we have context of.
*   @notes This was contained in TeensyThreads' Thread class as current_thread
*/
int current_thread_id = 0; 

/*
*   @brief Keeps track of how many threads we have at the moment
*   @notes This was contained in TeensyThreads' Thread class as thread_count
*/
int thread_count; 

/*
 * Teensy 3:
 * Replace the SysTick interrupt for our context switching. Note that
 * this function is "naked" meaning it does not save it's registers
 * on the stack. This is so we can preserve the stack of the caller.
 *
 * Interrupts will save r0-r4 in the stack and since this function
 * is short and simple, it should only use those registers. In the
 * future, this should be coded in assembly to make sure.
 */
extern volatile uint32_t systick_millis_count;
extern "C" void systick_isr();

/*
* @brief Pushes our system tick isr registers onto the stack, then puts them back on
* @notes  inherited from teensythreads. 
* @params none
* @returns none
*/
extern "C" void __attribute((naked, noinline)) will_os_threads_systick_isr(void){
  if (save_systick_isr) {
    asm volatile("push {r0-r4,lr}");
    (save_systick_isr)();
    asm volatile("pop {r0-r4,lr}");
  }

  // TODO: Teensyduino 1.38 calls MillisTimer::runFromTimer() from SysTick
  if (current_use_systick) {
    // We branch in order to preserve LR and the stack.
    __asm volatile("b will_os_context_switch");
  }
  __asm volatile("bx lr");
}

/*
*   @brief: Allows us to get the correct stack, and extract the program counter
*   @notes: Inherited from TeensyThreads, deals  with context switching for thread switching 
*   @params none
*   @returns none
*/
extern "C" void __attribute((naked, noinline)) will_os_threads_svcall_isr(void){
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
  // 
  if (svc == WILL_OS_SVC_NUM) {
    __asm volatile("b will_os_context_switch_direct");
  }
  else if (svc == WILL_OS_SVC_NUM_ACTIVE) {
    current_state = WILL_OS_STARTED;
    __asm volatile("b will_os_context_switch_direct_active");
  }
  __asm volatile("bx lr");
}

/*
*   @brief Denits all thread slots to empty
*   @notes  Should only be called on Will-OS initialization
*   @params none
*   @returns none
*/
inline void init_thread_zero(void){
    // Deinit all thread slots to empty
    // Should be done otherwise 
    for(int i = 0; i < MAX_WILL_THREADS; i++)
        system_threads[i] = NULL;
    // Initializes our first thread. 
    system_threads[0] = new thread_t;
    current_thread = system_threads[0];
    current_save = &system_threads[0]->register_contexts; 
    current_max_stack_pointer = 1; 
    current_stack_pointer = 0; 
    current_tick_count = WILL_OS_DEFAULT_TICKS; 
    current_state = WILL_OS_FIRST_RUN; 

    system_threads[0]->flags = WILL_THREAD_STATE_RUNNING;
    system_threads[0]->ticks = WILL_OS_DEFAULT_TICKS; 
    system_threads[0]->stack = (uint8_t*)&_estack - WILL_OS_DEFAULT_STACK0_SIZE;
    system_threads[0]->stack_size = WILL_OS_DEFAULT_STACK0_SIZE;

    Serial.println("initialized thread zero!");
}

/*
*   @brief Used to startup the Will-OS "Kernel" of sorts
*   @notes Must be called before you do any multithreading with the willos kernel
*   @params none
*   @returns none
*/
void will_os_init(void){
    // Sets up thread zero
    init_thread_zero();

    // Tells Teensy4 to Commandeer SVCall and use GTP1 Interrupt
    save_svcall_isr = _VectorsRam[11];
    if(save_svcall_isr == unused_interrupt_vector)
      save_svcall_isr = 0; 
      
    _VectorsRam[11] = will_os_threads_svcall_isr;
    
    current_use_systick = 0; 
    
    // Initialize unused GTP timer. 
    if(t4_unused_gpt_init(1000))
      Serial.println("GPT timer setup properly!");
}

/*
*   @brief Allows us to change the Will-OS System tick. 
*   @note If you want more precision in your system ticks, take care of this here. 
*   @params int tick_microseconds
*   @returns none
*/
extern bool will_os_change_systick(int microseconds){
    return t4_unused_gpt_init(microseconds);
}

/*
*   @brief Stops the entire Will-OS Kernel
*   @notes Try to avoid stopping the kernel whenever possible. 
*   @params none
*   @returns int original state of machine
*/
extern int will_os_system_stop(){
    // Disable the gtp timer 
    __disable_irq();

    // Stops the will-os kernel, sets state. 
    int old_state = current_state;
    current_state = WILL_OS_STOPPED;

    // Renable the gtp timer 
    __enable_irq();
    return old_state;
}

/*
*   @brief Starts the entire Will-OS Kernel
*   @notes Try to avoid stopping the kernel whenever possible. 
*   @params none
*   @returns int original state of machine
*/
extern int will_os_system_start(int previous_state = -1){
    // Disable the gtp timer
    __disable_irq();
    
    // Clean up states. 
    int old_state = current_state;
    if(previous_state == -1 )
        previous_state = WILL_OS_STARTED;
    current_state = previous_state;

    // Renable the gtp timer. 
    __enable_irq();
    return old_state;
}

/*
*   @brief  deletes a thread from the system. 
*   @notes  be careful, since this also ends the system kernel and isr's
*   @params none
*   @return none
*/
extern void will_os_thread_del_process(void){
  // Stopping the Will-OS system
  int old_state = will_os_system_stop();
  
  // Ptr to the thread we are using 
  thread_t *self_thread = system_threads[current_thread_id];
  
  // We are decrementing a thread
  thread_count--; 
  
  // Setting our current thread to an "ended" state
  self_thread->flags = WILL_THREAD_STATE_ENDED;

  // Restart the will-os kernel 
  will_os_system_start(old_state);
  
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
extern void* will_os_init_threadstack(will_os_thread_func_t thread, void *arg, void *stack_addr, int stack_size){
  isr_stack_t *process_frame = (isr_stack_t*)((uint8_t*)stack_addr + stack_size - sizeof(isr_stack_t) - 8);
  // Clearing up and setting all the registers. 
  process_frame->r0 = (uint32_t)arg;
  process_frame->r1 = 0;
  process_frame->r2 = 0;
  process_frame->r3 = 0;
  process_frame->r12 = 0;
  process_frame->lr = (uint32_t)will_os_thread_del_process;
  process_frame->pc = ((uint32_t)thread);
  process_frame->xpsr = 0x1000000;
  uint8_t *ret = (uint8_t*)process_frame;
  // ret -= sizeof(software_stack_t); // uncomment this if we are saving R4-R11 to the stack
  return (void*)ret;
}

/*
* @brief Sleeps the thread through a hypervisor call. 
* @notes Checks in roughly every milliscond until thread is ready to start running again
* @params int milliseconds since last system tick
* @returns none
*/
extern void will_os_thread_delay(int millisecond){
  int start_del = millis();
  // So let the hypervisor take us away from this thread, and check 
  // Each millisecond 
  while((int)millis() - start_del < millisecond)
    will_os_yield();
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
extern will_os_thread_id_t will_os_add_thread(will_os_thread_func_t thread, void *arg, void* stack, int stack_size){
  // Shutoff ISR and kernel stuff for a moment
  int old_state = will_os_system_stop();
  if(stack_size == -1)
    stack_size = WILL_OS_DEFAULT_STACK_SIZE; 

  for(int i = 1; i < MAX_WILL_THREADS; i++){
    if(system_threads[i] == NULL)
      system_threads[i] = new thread_t;
    
    // If the thread hasn't been initialized, or if thread has been suspended
    if(system_threads[i]->flags == WILL_THREAD_STATE_UNITIALIZED || system_threads[i]->flags == WILL_THREAD_STATE_ENDED){
      thread_t *this_thread = system_threads[i]; 
      
      // If there was already a thread stack here, or if thread stack
      // Was allocated on Heap by this function call, we delete the previous stack
      // And free up the memory. 
      if(this_thread->stack && this_thread->my_stack)
        delete[] this_thread->stack;
      
      // If we didn't point 
      if(stack == 0){
        // Allocate new stack onto the heap
        stack = new uint8_t[stack_size];
        // Since this was allocated in this thread, we save that flag
        // Letting the program know it was statically allocated previously
        this_thread->my_stack = 1; 
      }
      else
        this_thread->my_stack = 0; 

      // Saving the stack pointer on the thread reference
      this_thread->stack = (uint8_t*)stack;
      // Saving the size of our stack on the thread reference
      this_thread->stack_size = stack_size; 
      void *psp = will_os_init_threadstack(thread, arg, this_thread->stack, this_thread->stack_size);
      // Setting up thread registers
      this_thread->sp = psp; 
      
      // Saving the default tick frequency
      this_thread->ticks = WILL_OS_DEFAULT_TICKS; 
      // Set thread flag state to running
      this_thread->flags = WILL_THREAD_STATE_RUNNING;
      // Setting a specific register to this 
      this_thread->register_contexts.lr = 0xFFFFFFF9;
      // Updating current state to the old state
      current_state = old_state; 
      thread_count++; 

      // If the previous state was a start up operating system or if it was the first run
      if(old_state == WILL_OS_STARTED || old_state == WILL_OS_FIRST_RUN)
        will_os_system_start();
      // Returning the thread id
      return i; 
    }
  } 

  // Restarting the OS kernel, if we couldn't find any space then we weren't able to return
  // A working thread handler since the thread wasn't initialized 
  if(old_state == WILL_OS_STARTED)
    will_os_system_start();
  return -1; 
}

/*
*   @brief Increments to next thread for context switching
*   @notes Not to be called externally!
*   @params  none
*   @returns none
*/
void will_os_load_next_thread(void){
  current_thread->sp = current_stack_pointer;
  
  // Stack overflow ISR is triggered if we overflow any thread except thread 0(main thread)
  // Allows for an extra 8 bytes for a call to ISR and one call/variables
  if(current_thread_id && (uint8_t*)current_thread->sp - current_thread->stack <= 8){
    will_os_stack_overflow_isr();
  }

  // Search for next running thread
  while(1){
    // Increment next thread.
    current_thread_id++; 
    if(current_thread_id >= MAX_WILL_THREADS){
      // Thread 0 is MSP; always active so return.
      current_thread_id = 0; 
      break;
    }
    // If we find a thread to activate, we set to that thread. 
    if((system_threads[current_thread_id] != NULL) && system_threads[current_thread_id]->flags == WILL_THREAD_STATE_RUNNING)
      break;
  }

  // Set rest of current thread information for context switching in place
  current_tick_count = system_threads[current_thread_id]->ticks; 
  // Saving the register contexts including program counter. 
  current_save = &system_threads[current_thread_id]->register_contexts;
  // Saving current maximum stack pointer
  current_max_stack_pointer = (current_thread_id == 0 ? 1: 0);
  // Current Stack Pointer
  current_stack_pointer = system_threads[current_thread_id]->sp; 
}

/*
* @brief Gets the state of a thread. 
* @brief If thread doesn't exist, then 
* @params Which thread are we trying to get our state for
* @returns will_thread_state_t
*/
will_thread_state_t will_os_get_thread_state(will_os_thread_id_t thread_id){
  if(thread_id < thread_count)
    return system_threads[thread_id]->flags;
  return WILL_THREAD_STATE_DNE; 
}

/*
* @brief Sets the state of a thread. 
* @brief If thread doesn't exist, then 
* @params Which thread are we trying to get our state for
* @returns will_thread_state_t
*/
extern will_thread_state_t will_os_set_thread_state(will_os_thread_id_t thread_id, will_thread_state_t thread_state){
  if(thread_id < thread_count){
    system_threads[thread_id]->flags = thread_state;
    return thread_state; 
  }
  return WILL_THREAD_STATE_DNE;
}

/*
* @brief Sets the state of a thread to dead 
* @brief If thread doesn't exist, then 
* @params Which thread are we trying to get our state for
* @returns will_thread_state_t
*/
extern will_os_thread_id_t will_os_kill_thread(will_os_thread_id_t thread_id){
  if(thread_id < thread_count){
    system_threads[thread_id]->flags = WILL_THREAD_STATE_ENDED;
    return thread_id; 
  }
  return -1;
}

/*
* @brief Sets the state of a thread to suspended. 
* @brief If thread doesn't exist, then 
* @params Which thread are we trying to get our state for
* @returns will_thread_state_t
*/
extern will_os_thread_id_t will_os_suspend_thread(will_os_thread_id_t thread_id){
  if(thread_id < thread_count){
    system_threads[thread_id]->flags = WILL_THREAD_STATE_SUSPENDED;
    return thread_id; 
  }
  return -1; 
}

/*
* @brief Sets the state of a thread to running 
* @brief If thread doesn't exist, then 
* @params Which thread are we trying to get our state for
* @returns will_thread_state_t
*/
extern will_os_thread_id_t will_os_restart_thread(will_os_thread_id_t thread_id){
  if(thread_id < thread_count){
    system_threads[thread_id]->flags = WILL_THREAD_STATE_RUNNING;
    return thread_id; 
  }
  return -1;
}

/*
* @brief Set's default thread creation stack size to inputed value
* @params unsigned int stack_size(that we want to change to our default)
* @returns none
*/
extern void will_os_set_default_stack_size(unsigned int stack_size){
  WILL_OS_DEFAULT_STACK_SIZE = stack_size; 
} 

/*
* @brief Sets the tick count of a thread
* @brief If thread doesn't exist, then 
* @params Which thread are we trying to change our tick count, the tickount
* @returns bool(success of function call)
*/
extern bool will_os_set_time_slice(will_os_thread_id_t thread_id, unsigned int ticks){
  if(thread_id < thread_count){
    system_threads[thread_id]->ticks = ticks-1; 
    return true; 
  }

  return false; 
}

/*
* @brief Sets the default tick count of a thread upon creation. 
* @brief If thread doesn't exist, then 
* @params unsigned int ticks(default tick count)
* @returns none
*/
extern void will_os_set_time_slice(unsigned int ticks){
  WILL_OS_DEFAULT_TICKS = ticks - 1; 
}

#endif 