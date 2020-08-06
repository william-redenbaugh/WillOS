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
*   @brief GPT1 ISR function, which calls on timer call
*   @notes Used exclusively for ISR context switching purposes.
*/
static void __attribute((naked, noinline)) gpt1_isr() {
  GPT1_SR |= GPT_SR_OF1;                // clear set bit
  __asm volatile ("dsb");               // see github bug #20 by manitou48
  __asm volatile("b context_switch");
}

/*
*   @brief GPT1 ISR function, which calls on timer call
*   @notes Used exclusively for ISR context switching purposes.
*/
static void __attribute((naked, noinline)) gpt2_isr() {
  GPT2_SR |= GPT_SR_OF1;                // clear set bit
  __asm volatile ("dsb");               // see github bug #20 by manitou48
  __asm volatile("b context_switch");
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
      GPT1_PR = 23;                                   // Prescale: divide by 24 so 1 tick = 1 microsecond at 24MHz
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
*   @brief Pointer array to all the system threads
*/
thread_t *system_threads[MAX_WILL_THREADS];

extern "C"{
/*
*   @brief The current thread in the program. 
*   @notes Comes preinitialized
*/
int current_state = WILL_OS_UNINITIALIZED; 

/*
*   @brief The current register stack on the program
*/
context_switch_stack_t current_context; 

/*
*   @brief Current System Tick   
*/
int current_use_systick; 

/*
*
*/
int current_tick_count; 

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
}


#endif 