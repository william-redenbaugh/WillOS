#ifndef _OSMESSAGEKERNEL_H
#define _OSMESSAGEKERNEL_H

#include "OSThreadKernel.h"
#include "OSMutexKernel.h"

class OSSignal{
    public: 
    
    /*
    *   @brief Signals a bit
    *   @params thread_signal_t which signal we are setting
    */
    void signal(thread_signal_t thread_signal);

    /*
    *   @brief clears a bit
    *   @params thread_signal_t which signal we are clearing
    */
    void clear(thread_signal_t thread_signal);
    
    /*
    *   @brief Checks to see if a bit is cleared or set
    *   @params thread_signal_t which bit we want to check
    *   @returns The status of the signal we are checking 
    */
    thread_signal_status_t check(thread_signal_t thread_signal);
    
    /*
    *   @brief Checks to see if a bit is cleared or set
    *   @params thread_signal_t which bit we want to check
    *   @params uint32_t timeout_ms timeout or max time we are willing to wait for bits to clear
    *   @returns whether or or not we we able to get set bits or not
    */
    thread_signal_status_t wait(thread_signal_t thread_signal, uint32_t timeout_ms);
    
    /*
    *   @brief Waits for bits to be set indefinitly
    *   @params thread_signal_t thread_signal to be sete
    */    
    void wait_notimeout(thread_signal_t thread_signal);
    
    /*
    *   @returns The raw bits that hold our signals.
    */
    uint32_t bits_return(void);

    private: 
    // Bits data that we are using to wait with 
    volatile uint32_t bits; 
};

#endif 