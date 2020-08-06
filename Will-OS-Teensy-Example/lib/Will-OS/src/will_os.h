#ifndef _WILL_OS_H
#define _WILL_OS_H
#include <Arduino.h>

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
    WILL_THREAD_STATE_SUSPENDED =   5
};

/*
*   @brief Struct that contains information for each thread
*   @notes Used to deal with thread context switching
*/
typedef struct{
    // Size of stack
    uint32_t stack_size; 
    // Stack pointer. 
    uint8_t *stack = 0; 
    int my_stack; 
    // Where we save all our registers for context switching 
    context_switch_stack_t register_contexts; 
    // Flags for dealing with thread 
    volatile will_thread_state_t flags = WILL_THREAD_STATE_UNITIALIZED; 
    // Where are we in the program so far
    void *sp; 
    // Thread ticks 
    int ticks; 
}thread_t; 

/*
*   @brief Used to startup the Will-OS "Kernel" of sorts
*   @notes Must be called before you do any multithreading with the willos kernel
*   @params none
*   @returns none
*/
void will_os_init(void);

/*
*   @brief Intializes the unused General Purpose Timers in the Teensy 4. 
*   @notes if we can't set this up, then currently Teensy 4 will not initialize Will-OS
*   @params microseconds between each context switch
*/
bool t4_unused_gpt_init(unsigned int microseconds);

/*
*   @brief Stack space used by will-os. 
*   @notes In order to generate a thread, you need to have allocated stack space first!
*/
typedef uint8_t will_os_stack_t;

#endif 