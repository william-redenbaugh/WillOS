#ifndef _WILL_OS_H
#define _WILL_OS_H
#include <Arduino.h>

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

enum will_os_system_state{
    WILL_OS_STARTED =       1, 
    WILL_OS_STOPPED =       2, 
    WILL_OS_FIRST_RUN =     3,
    WILL_OS_UNINITIALIZED = 4 
};

enum will_thread_state{
    WILL_THREAD_STATE_UNITIALIZED = 1, 
    WILL_THREAD_STATE_RUNNING =     2, 
    WILL_THREAD_STATE_ENDING =      3, 
    WILL_THREAD_STATE_ENDED =       4, 
    WILL_THREAD_STATE_SUSPENDED =   5
};

#endif 