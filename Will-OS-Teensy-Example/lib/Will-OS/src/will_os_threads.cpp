#include "will_os.h"

// Currenly the only platform supported by Will-OS is the Teensy 4
#ifdef __IMXRT1062__

/*
*   @brief Ensures that all memory access appearing before this program point are taken care of.   
*   @notes To understand more, visit: https://www.keil.com/support/man/docs/armasm/armasm_dom1361289870356.htm
*/
#define flush_cpu_pipeline() __asm__ volatile("DMB");

/*
*   @brief Supervisor call number, used for yielding the program
*/
const int WILL_OS_SVC_NUM = 33; 

/*
*   @brief allows our program to "yield" out of current subroutine
*   @notes 
*/
void will_os_yield(void){
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
void will_os_yield_start(void){
    __asm volatile("svc %0" : : "i"(WILL_OS_SVC_NUM_ACTIVE));
}

/*
*   @brief Default amount of ticks
*/
const int WILL_OS_DEFAULT_TICKS = 10; 

/*
*   @brief Default stack size of thread zero, set to 2^14 bytes
*/
const int WILL_OS_DEFAULT_STACK0_SIZE = 16384;

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
  __asm volatile("b context_switch");   // Branch to context switch assembly code. 
}

/*
*   @brief GPT1 ISR function, which calls on timer call
*   @notes Used exclusively for ISR context switching purposes.
*/
static void __attribute((naked, noinline)) gpt2_isr() {
  GPT2_SR |= GPT_SR_OF1;                // Clear set bit.
  __asm volatile ("dsb");               // see github bug #20 by manitou48
  __asm volatile("b context_switch");   // Branch to context switch assembly code.
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
* @brief Main Thread zero stack. 
*/
extern unsigned long _estack; 

/*
*   @brief 
*/
will_os_isr_func_t save_systick_isr; 

/*
*   @brief
*/
will_os_isr_func_t save_svcall_isr; 

/*
*   @brief Pointer array to all the system threads
*/
thread_t *system_threads[MAX_WILL_THREADS];

extern "C"{

/*
*   @brief subroutine called from assembly code to load next thread's context and registers
*   @notes Only to be used in assembly context switching code 
*   @params none
*   @returns none
*/
void load_next_thread(void){

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
*   @brief Current System Tick   
*   @notes In TeensyThreads this was currentUseSystick for the gpt timer
*/
int current_use_systick; 

/*
*   @brief Current count of system tick
*   @notes in TeensyThreads this was currentCount
*/
int current_tick_count; 

/*
*   @brief Current program thread stack pointer 
*   @notes Updated everytime we enter and leave threads. 
*   @notes in TeensyThreads this was currentSP
*/
void *current_stack_pointer; 

}

/*
*   @brief If our current thread's stack overflows, then we kill the thread
*   @notes Please try your best to void this behavior. 
*/
extern "C" void stack_overflow_default_isr() { 
  current_thread->flags = WILL_THREAD_STATE_ENDED;
}

/*
*   @notes inherited from teensyThread, seems to link to the above function
*/
extern "C" void stack_overflow_isr(void)       __attribute__ ((weak, alias("stack_overflow_default_isr")));

/*
*   @brief Current thread that we have context of.
*/
int current_thread_id; 

/*
*   @brief Keeps track of how many threads we have at the moment
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
void __attribute((naked, noinline)) threads_systick_isr(void)
{
  if (save_systick_isr) {
    asm volatile("push {r0-r4,lr}");
    (save_systick_isr)();
    asm volatile("pop {r0-r4,lr}");
  }

  // TODO: Teensyduino 1.38 calls MillisTimer::runFromTimer() from SysTick
  if (current_use_systick) {
    // we branch in order to preserve LR and the stack
    __asm volatile("b context_switch");
  }
  __asm volatile("bx lr");
}

/*
*   @brief: Allows us to get the correct stack, and extract the program counter
*   @notes: Inherited from TeensyThreads, deals  with context switching for thread switching 
*   @params none
*   @returns none
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
  if (svc == WILL_OS_SVC_NUM) {
    __asm volatile("b context_switch_direct");
  }
  else if (svc == WILL_OS_SVC_NUM_ACTIVE) {
    current_state = WILL_OS_STARTED;
    __asm volatile("b context_switch_direct_active");
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
    system_threads[0]->flags = WILL_THREAD_STATE_RUNNING;
    system_threads[0]->ticks = WILL_OS_DEFAULT_TICKS; 
    system_threads[0]->stack_size = WILL_OS_DEFAULT_STACK0_SIZE;
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
        _VectorsRam[11] = threads_svcall_isr;
    
    current_use_systick = 0; 
    
    // Initialize unused GTP timer. 
    t4_unused_gpt_init(1000);
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
    __disable_irq();
    int old_state = current_state;
    current_state = WILL_OS_STOPPED;
    __enable_irq();
    return old_state;
}

/*
*   @brief Starts the entire Will-OS Kernel
*   @notes Try to avoid stopping the kernel whenever possible. 
*   @params none
*   @returns int original state of machine
*/
extern int will_os_system_start(int previous_state){
    __disable_irq();
    int old_state = current_state;
    if(previous_state == -1 )
        previous_state = WILL_OS_STARTED;

    current_state = previous_state;
    __enable_irq();
    return old_state;
}

#endif 