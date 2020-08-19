/*
 * Threads.h - Library for threading on the Teensy.
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
 *
 * This project was heavily modified by William Redenbaugh in 2020. But props to the original creator! He did the hard work
 */

/*
Author: William Redenbaugh, Fernando Trias
Last Edit Date: 8/9/2020
*/
#ifndef _THREADS_H
#define _THREADS_H

// Currenly the only platform supported by Will-OS is the Teensy 4
#ifdef __IMXRT1062__

// Importing primary libraries. 
#include <Arduino.h>
#include <stdint.h>

/*
* @brief Enumerated State of different operating system states. 
* @notes Used for dealing with different threading purposes. 
*/
enum os_state_t{
  OS_UNINITIALIZED  = -1,
  OS_STARTED        = 1, 
  OS_STOPPED        = 2, 
  OS_FIRST_RUN      = 3
};

/*
* @brief Enumerated state of different thread states
* @notes Used for dealing with different threading purposes such as creating, enabling, and deleting a thread. 
*/
enum thread_state_t{
  THREAD_DNE        = -1,
  THREAD_EMPTY      = 0, 
  THREAD_RUNNING    = 1, 
  THREAD_ENDED      = 2, 
  THREAD_ENDING     = 3, 
  THREAD_SUSPENDED  = 4
}; 

/*
*   @brief Maximum amount of threads the Will-OS supports
*   @notes Unless we transition to a linked list(which is unlikely), this will remain the max limit
*/
#ifndef OS_EXTERN_MAX_THREADS
static const int MAX_THREADS = 128;
#else
static const int MAX_THREADS = OS_EXTERN_MAX_THREADS; 
#endif 

/*
* @brief Default Tick set to 100 microseconds per tick
* @notes As far as I know, this isn't the real tick, considering ISR happens 1,000/s not 10,000/s
*/
static const int DEFAULT_TICK_MICROSECONDS = 100;

/*
*   @brief Supervisor call number, used for yielding the program
*/
const int WILL_OS_SVC_NUM = 33; 

/*
*   @brief Supervisor call number, used for yielding the program
*/
const int WILL_OS_SVC_NUM_ACTIVE = 34; 

/*
* @brief Default stack size of thread0, or the loop thread. 
* @notes This should be changed based off how much the user needs. Can be changed using macro 
*/
#ifndef EXTERNAL_STACK0_SIZE
static const int DEFAULT_STACK0_SIZE = 10240; 
#else 
static const int DEFAULT_STACK0_SIZE = EXTERNAL_STACK0_SIZE; 
#endif 
/*
*   @brief register stack frame saved by interrupt
*   @notes Used so that when we get interrupts, we can revert back the original registers. 
*/ 
typedef struct {
  uint32_t r0;
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
  uint32_t r12;
  uint32_t lr;
  uint32_t pc;
  uint32_t xpsr;
} interrupt_stack_t;

/*
*   @brief Stack frame saved by context switch
*   @notes Used for switching between threads, we save all relevant registers between threads somewhere, and get them when needed
*/ 
typedef struct {
  uint32_t r4;
  uint32_t r5;
  uint32_t r6;
  uint32_t r7;
  uint32_t r8;
  uint32_t r9;
  uint32_t r10;
  uint32_t r11;
  uint32_t lr;
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
} software_stack_t;

/*
*   @brief Struct that contains information for each thread
*   @notes Used to deal with thread context switching
*/
typedef struct{
  // Size of stack
  int stack_size;
  
  // Stack pointer. 
  uint8_t *stack=0;
  
  // Whether or not stack was allocated by thread creation function
  int my_stack = 0;
  
  // Where we save all our registers for context switching .
  software_stack_t save;
  
  // Flags for dealing with thread 
  volatile thread_state_t flags = THREAD_EMPTY;
  
  // Where are we in the program so far
  void *sp;

  // Thread ticks  
  int ticks;

  // Flags to set or clear signals to a thread. 
  volatile uint32_t thread_set_flags = 0x0000;
}thread_t;

/*
* @brief Redeclaration of thread function
* @notes Holds pointer to begining of thread function subroutine. Holds register space for void pointer 
*/
typedef void (*thread_func_t)(void*);

/*
* @brief Redclaration of thread function with integer parameter
* @notes  Holds pointer to begining of thread function subroutine with registers set asside for integer manipulation
*/
typedef void (*int_thread_func_t)(int);

/*
* @brief Redeclaration of thread function with no parameter
* @notes  Holds pointer of begining of thread function subroutine. 
*/
typedef void (*none_thread_func_t);

/*
* @brief Interrupt service function call
* @notes  Used to deal with ISR functions
*/
typedef void (*os_isr_function_t)();

/*
*   @brief Ensures that all memory access appearing before this program point are taken care of.   
*   @notes To understand more, visit: https://www.keil.com/support/man/docs/armasm/armasm_dom1361289870356.htm
*/
#define __flush_cpu_pipeline() __asm__ volatile("DMB");

/*
* @brief  Thread id value
* @notes
*/
typedef int os_thread_id_t;

extern "C" {

/*
* @brief  Assembly call to the context switch subroutine 
* @notes  Uses for switching "contexts" between threads. 
*/
void context_switch(void);

/*
* @brief Assembly call to the context switch direct subroutine"
* @notes  Used for switching "contexts" between threads, bypasses some of the typical context switch subrountes
*/
void context_switch_direct(void);

/*
* @brief Assembly call helping deal with context switching
* @notes n/a
*/
void context_switch_pit_isr(void);

/*
* @brief function that is to be called in assembly to access contexts for thread switching
* @notes  Should only be called for assembly use, not user use
*/
void load_next_thread_asm();

/*
* @brief If our stack overflows, we call this subroutine
* @notes  n/a
*/
void stack_overflow_isr(void);

/*
* @brief isr helping deal with thread stuff
* @notes  n/a
*/
void threads_svcall_isr(void);

/*
* @brief isr helping deal with system tick stuff
* @notes  n/a
*/
void threads_systick_isr(void);
}

/*
*   @brief allows our program to "yield" out of current subroutine
*   @notes Call's hypervisor command to look into something else. 
*/
extern "C" void _os_yield(void);

/*
* @brief Sleeps the thread through a hypervisor call. 
* @notes Checks in roughly every milliscond until thread is ready to start running again
* @params int milliseconds since last system tick
* @returns none
*/
extern void os_thread_delay_ms(int millisecond);

/*
* @brief Sleeps the thread through a hypervisor call. 
* @notes Checks in roughly every milliscond until thread is ready to start running again
* @params seconds
* @returns none
*/
#define os_thread_delay_s(seconds)  os_thread_delay_ms(1000 * seconds)

/*
*   @brief Used to startup the Will-OS "Kernel" of sorts
*   @notes Must be called before you do any multithreading with the willos kernel
*   @params none
*   @returns none
*/
void threads_init(void);
#define os_init() threads_init()

/*
*   @brief Increments to next thread for context switching
*   @notes Not to be called externally!
*   @params  none
*   @returns none
*/
void os_get_next_thread();

/*
* @brief Adds a thread to Will-OS Kernel
* @notes Paralelism at it's finest!
* @params will_os_thread_func_t thread(pointer to thread function call begining of program counter)
* @params void *arg(pointer arguement to parameters for thread)
* @param void *stack(pointer to begining of thread stack)
* @param int stack_size(size of the allocated threadstack)
* @returns none
*/
os_thread_id_t os_add_thread(thread_func_t p, void * arg, int stack_size, void *stack);

/*
*   @brief Allows us to change the Will-OS System tick. 
*   @note If you want more precision in your system ticks, take care of this here. 
*   @params int tick_microseconds
*   @returns none
*/
bool os_set_microsecond_timer(int tick_microseconds);

/*
* @brief Sets the state of a thread to suspended. 
* @brief If thread doesn't exist, then 
* @params Which thread are we trying to get our state for
* @returns will_thread_state_t
*/
os_thread_id_t os_suspend_thread(os_thread_id_t target_thread_id);

/*
* @brief Sets the state of a thread to resumed. 
* @brief If thread doesn't exist or hasn't been run before, then 
* @params Which thread are we trying to get our state for
* @returns will_thread_state_t
*/
os_thread_id_t os_resume_thread(os_thread_id_t target_thread_id);

/*
* @brief Sets the state of a thread to dead 
* @brief If thread doesn't exist, then 
* @params Which thread are we trying to get our state for
* @returns will_thread_state_t
*/
os_thread_id_t os_kill_thread(os_thread_id_t target_thread_id);

/*
*   @brief Stops the entire Will-OS Kernel
*   @notes Try to avoid stopping the kernel whenever possible. 
*   @params none
*   @returns int original state of machine
*/
int os_stop(void);

/*
*   @brief Starts the entire Will-OS Kernel
*   @notes Try to avoid stopping the kernel whenever possible. 
*   @params none
*   @returns int original state of machine
*/
int os_start(int prev_state = -1);

/*
* @returns The current thread's ID. 
*/
os_thread_id_t os_current_id(void);

/*
* @brief unused ISR routine that we can use for whatever
* @notes I guess we have this here if we wanna use it 
*/
extern "C" void unused_isr(void);

/*
* @brief allows us to sleep the thread for a period of time. 
*/ 
extern "C" int enter_sleep(int ms);

/*
* @brief Which threads we want to set signals for
*/
typedef enum{
  THREAD_SIGNAL_0   = 0,
  THREAD_SIGNAL_1   = 1,
  THREAD_SIGNAL_2   = 2,
  THREAD_SIGNAL_3   = 3,
  THREAD_SIGNAL_4   = 4,
  THREAD_SIGNAL_5   = 5,
  THREAD_SIGNAL_6   = 6,
  THREAD_SIGNAL_7   = 7,
  THREAD_SIGNAL_8   = 8,
  THREAD_SIGNAL_9   = 9,
  THREAD_SIGNAL_10  = 10,
  THREAD_SIGNAL_11  = 11,
  THREAD_SIGNAL_12  = 12,
  THREAD_SIGNAL_13  = 13,
  THREAD_SIGNAL_14  = 14,
  THREAD_SIGNAL_15  = 15,
  THREAD_SIGNAL_16  = 16,
  THREAD_SIGNAL_17  = 17,
  THREAD_SIGNAL_18  = 18,
  THREAD_SIGNAL_19  = 19,
  THREAD_SIGNAL_20  = 20,
  THREAD_SIGNAL_21  = 21,
  THREAD_SIGNAL_22  = 22,
  THREAD_SIGNAL_23  = 23,
  THREAD_SIGNAL_24  = 24,
  THREAD_SIGNAL_25  = 25,
  THREAD_SIGNAL_26  = 26,
  THREAD_SIGNAL_27  = 27, 
  THREAD_SIGNAL_28  = 28,
  THREAD_SIGNAL_29  = 29,
  THREAD_SIGNAL_30  = 30,
  THREAD_SIGNAL_31  = 31
}thread_signal_t;

/*
* @brief The staus of a thread whose bits we wanna check
*/
typedef enum{
  THREAD_SIGNAL_SET, 
  THREAD_SIGNAL_CLEAR, 
  THREAD_SIGNAL_DNE, 
  THREAD_SIGNAL_TIMEOUT
}thread_signal_status_t;

/*
* @brief Macro that checks if a certain bit in a variable is set. 
* @params(variable, position in variable(0 lsb))
* @returns 1 if true, 0 otherwise
*/
#define OS_CHECK_BIT(var,pos) ((var) & (1<<(pos)))

/*
* @brief Allows us to send signals to each thread by setting a bitmask
* @notes This uses preset flags to allow us to set and clear clags in a thread
* @params thread_signal_t thread_signal(there are 32 thread signals per thread)
*/
void os_thread_signal(thread_signal_t thread_signal);

/*
* @brief Allows us to send signals to each thread by clearing a bitmask
* @notes This uses preset flags to allow us to set and clear clags in a thread
* @params thread_signal_t thread_signal(there are 32 thread signals per thread)
*/
void os_thread_clear(thread_signal_t thread_signal);

/*
* @brief Allows us to send signals to each thread by setting a bitmask
* @notes This uses preset flags to allow us to set and clear clags in a thread
* @params thread_signal_t thread_signal(there are 32 thread signals per thread)
* @params os_thread_id_t target_thread_id which thread we want to signal
*/
bool os_signal_thread(thread_signal_t thread_signal, os_thread_id_t target_thread_id);

/*
* @brief Allows us to send signals to each thread by clearing a bitmask
* @notes This uses preset flags to allow us to set and clear clags in a thread
* @params thread_signal_t thread_signal(there are 32 thread signals per thread)
* @params os_thread_id_t target_thread_id in which we want to clear flags
*/
bool os_signal_thread_clear(thread_signal_t thread_signal, os_thread_id_t target_thread_id);

/*
* @brief We can check if there are bits that are signaled
* @params which bits we want to check,
* @return if those bits are set or not
*/  
thread_signal_status_t os_thread_checkbits(thread_signal_t thread_signal);

/*
* @brief We can check if there are bits that are signaled
* @params thread_signal which bits we want to check,
* @params target_thread_id which thread we 
* @return if those bits are set or not
*/  
thread_signal_status_t os_checkbits_thread(thread_signal_t thread_signal, os_thread_id_t target_thread_id);

/*
* @brief Hangs thread until signal has been cleared
* @params thread_signal_t thread_signal
*/
void os_thread_waitbits_notimeout(thread_signal_t thread_signal);

// This endif is for checking if we are using the Teensy4 IMXRT board
#endif
#endif