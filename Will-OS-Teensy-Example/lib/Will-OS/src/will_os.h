#ifndef _WILL_OS_H
#define _WILL_OS_H
#include <Arduino.h>
#include <stdint.h> 

extern "C" {
  void will_os_context_switch(void);
  void will_os_context_switch_direct(void);
  void will_os_context_switch_pit_isr(void);
  void load_next_thread_asm_call(void);
  void stack_overflow_isr(void);
  void will_os_threads_svcall_isr(void);
  void will_os_threads_systick_isr(void);
}

/*
*   @brief Maximum amount of threads the Will-OS supports
*   @notes Unless we transition to a linked list(which is unlikely), this will remain the max limit
*/
#define MAX_WILL_THREADS 128    

/*
*   @brief Default size of stack 0 for thread 0   
*   @notes Should changed based
*/

/*
*   @brief register stack frame saved by interrupt
*   @notes Used so that when we get interrupts, we can revert back the original registers. 
*/ 
typedef struct{
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t xpsr;
}isr_stack_t;

/*
*   @brief Stack frame saved by context switch
*   @notes Used for switching between threads, we save all relevant registers between threads somewhere, and get them when needed
*/ 
typedef struct{
    // Normal Cortex Registers
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t lr; 
    // FPU registers
    uint32_t s0;
    uint32_t s1;
    uint32_t s2;
    uint32_t s3;
    uint32_t s4;
    uint32_t s5;
    uint32_t s6;
    uint32_t s7;
    uint32_t s8;
    uint32_t s9;
    uint32_t s10;
    uint32_t s11;
    uint32_t s12;
    uint32_t s13;
    uint32_t s14;
    uint32_t s15;
    uint32_t s16;
    uint32_t s17;
    uint32_t s18;
    uint32_t s19;
    uint32_t s20;
    uint32_t s21;
    uint32_t s22;
    uint32_t s23;
    uint32_t s24;
    uint32_t s25;
    uint32_t s26;
    uint32_t s27;
    uint32_t s28;
    uint32_t s29;
    uint32_t s30;
    uint32_t s31;
    uint32_t fpscr;
}context_switch_stack_t;

/*
*   @brief Enumerated state values of operating system. 
*   @notes Even though it's in the header file, should otherwise only be used by operating system 
*/
enum will_os_system_state_t{
    WILL_OS_STARTED =       1, 
    WILL_OS_STOPPED =       2, 
    WILL_OS_FIRST_RUN =     3,
    WILL_OS_UNINITIALIZED = 0 
};

/*
*   @brief Enumerated state values of threads within the Will-OS operating system 
*   @notes Even though it's saved in the header file, should otherwise be used by operating sytem 
*/
enum will_thread_state_t{
    WILL_THREAD_STATE_UNITIALIZED = 1, 
    WILL_THREAD_STATE_RUNNING =     2, 
    WILL_THREAD_STATE_ENDING =      3, 
    WILL_THREAD_STATE_ENDED =       4, 
    WILL_THREAD_STATE_SUSPENDED =   5,
    WILL_THREAD_STATE_DNE =         -1
};

/*
*   @brief thread id that we can use to control thread
*   @notes Should you need to stop, start, end, begin new threads, make sure you keep this saved somewhere
*/
typedef int will_os_thread_id_t;

/*
*   @brief Struct that contains information for each thread
*   @notes Used to deal with thread context switching
*/
typedef struct{
    // Size of stack
    uint32_t stack_size; 
    // Stack pointer. 
    uint8_t *stack = 0;
    // Whether or not stack was allocated by thread creation function 
    int my_stack = 0; 
    // Where we save all our registers for context switching .
    context_switch_stack_t register_contexts; 
    // Flags for dealing with thread 
    volatile int flags = WILL_THREAD_STATE_UNITIALIZED; 
    // Where are we in the program so far
    void *sp; 
    // Thread ticks 
    int ticks; 
}thread_t; 

/*
*   @brief Stack space used by will-os. 
*   @notes In order to generate a thread, you need to have allocated stack space first!
*/
typedef uint8_t will_os_stack_t;

/*
*   @brief Thread function program counter pointer
*   @notes This is the type of functio you pass into will-os for thread creation
*/
typedef void (*will_os_thread_func_t)(void*);

/*
*   @brief Will OS ISR function stuff, used for interrupt service routines 
*   @notes currrently used for gpt timer configuration
*/
typedef void (*will_os_isr_func_t)();

/*
*   @brief Allows us to change the Will-OS System tick. 
*   @note If you want more precision in your system ticks, take care of this here. 
*   @params int tick_microseconds
*   @returns none
*/

/*
*   @brief Intializes the unused General Purpose Timers in the Teensy 4. 
*   @notes if we can't set this up, then currently Teensy 4 will not initialize Will-OS
*   @params microseconds between each context switch
*/
bool t4_unused_gpt_init(unsigned int microseconds);

/*
*   @brief Used to startup the Will-OS "Kernel" of sorts
*   @notes Must be called before you do any multithreading with the willos kernel
*   @params none
*   @returns none
*/
void will_os_init(void);


/*
*   @brief Allows us to change the Will-OS System tick. 
*   @note If you want more precision in your system ticks, take care of this here. 
*   @params int tick_microseconds
*   @returns none
*/
extern bool will_os_change_systick(int microseconds);

/*
*   @brief Stops the entire Will-OS Kernel
*   @notes Try to avoid stopping the kernel whenever possible. 
*   @params none
*   @returns int original state of machine
*/
extern int will_os_system_stop();

/*
*   @brief Starts the entire Will-OS Kernel
*   @notes Try to avoid stopping the kernel whenever possible. 
*   @params none
*   @returns int original state of machine
*/
extern int will_os_system_start(int previous_state = -1);

/*
*   @brief  deletes a thread from the system. 
*   @notes  be careful, since this also ends the system kernel and isr's
*   @params none
*   @return none
*/
extern void will_os_thread_del_process(void);

/*
*   @brief Given a thread pointer, arguements towards thread pointer, stack address to thread pointer, and stack size, we can setup the isr interrupts for thread. 
*   @notes Called whenever we add another thread. 
*   @params will_os_thread_func_t thread pointer to program counter start of thread
*   @params void *arg address pointer of arguements to pass into will-os
*   @params void *stack_addr pointer to the begining of the stack address. 
*   @params int stack_size size of the stack for this thread
*   @returns pointer to the new threadstack. 
*/
extern void* will_os_init_threadstack(will_os_thread_func_t thread, void *arg, void *stack_addr, int stack_size);

/*
* @brief Sleeps the thread through a hypervisor call. 
* @notes Checks in roughly every milliscond until thread is ready to start running again
* @params int milliseconds since last system tick
* @returns none
*/
extern void will_os_thread_delay(int millisecond);

/*
* @brief Adds a thread to Will-OS Kernel
* @notes Paralelism at it's finest!
* @params will_os_thread_func_t thread(pointer to thread function call begining of program counter)
* @params void *arg(pointer arguement to parameters for thread)
* @param void *stack(pointer to begining of thread stack)
* @param int stack_size(size of the allocated threadstack)
* @returns none
*/
extern will_os_thread_id_t will_os_add_thread(will_os_thread_func_t thread, void *arg, void* stack, int stack_size);

/*
*   @brief Increments to next thread for context switching
*   @notes Not to be called externally!
*   @params  none
*   @returns none
*/
extern void will_os_load_next_thread(void);

/*
* @brief Gets the state of a thread. 
* @brief If thread doesn't exist, then 
* @params Which thread are we trying to get our state for
* @returns will_thread_state_to
*/
will_thread_state_t will_os_get_thread_state(will_os_thread_id_t thread_id);

/*
* @brief Sets the state of a thread to dead 
* @brief If thread doesn't exist, then 
* @params Which thread are we trying to get our state for
* @returns will_thread_state_t
*/
extern will_os_thread_id_t will_os_kill_thread(will_os_thread_id_t thread_id);

/*
* @brief Sets the state of a thread to suspended. 
* @brief If thread doesn't exist, then 
* @params Which thread are we trying to get our state for
* @returns will_thread_state_t
*/
extern will_os_thread_id_t will_os_suspend_thread(will_os_thread_id_t thread_id);

/*
* @brief Sets the state of a thread to running 
* @brief If thread doesn't exist, then 
* @params Which thread are we trying to get our state for
* @returns will_thread_state_t
*/
extern will_os_thread_id_t will_os_restart_thread(will_os_thread_id_t thread_id);

/*
* @brief Set's default thread creation stack size to inputed value
* @params unsigned int stack_size(that we want to change to our default)
* @returns none
*/
extern void will_os_set_default_stack_size(unsigned int stack_size);

/*
* @brief Sets the tick count of a thread
* @brief If thread doesn't exist, then 
* @params Which thread are we trying to change our tick count, the tickount
* @returns bool(success of function call)
*/
extern bool will_os_set_time_slice(will_os_thread_id_t thread_id, unsigned int ticks);

/*
* @brief Sets the default tick count of a thread upon creation. 
* @brief If thread doesn't exist, then 
* @params unsigned int ticks(default tick count)
* @returns none
*/
extern void will_os_set_time_slice(unsigned int ticks);
#endif 