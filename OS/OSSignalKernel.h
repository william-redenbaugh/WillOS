#ifndef _OSMESSAGEKERNEL_H
#define _OSMESSAGEKERNEL_H

// So we can configure modules
#include "../enabled_modules.h"
#ifdef SIGNALING_MODULE

#include "OSThreadKernel.h"
#include "OSMutexKernel.h"

class OSSignal{
    public: 

    /*!
    *   @brief Signals a bit
    *   @param thread_signal_t which signal we are setting
    */
    void signal(thread_signal_t thread_signal);

    /*!
    *   @brief clears a bit
    *   @param thread_signal_t which signal we are clearing
    */
    void clear(thread_signal_t thread_signal);
    
    /*!
    *   @brief Checks to see if a bit is cleared or set
    *   @param thread_signal_t which bit we want to check
    *   @returns The status of the signal we are checking 
    */
    bool check(thread_signal_t thread_signal);
    
    /*!
    *   @brief Checks to see if a bit is cleared or set
    *   @param thread_signal_t which bit we want to check
    *   @param uint32_t timeout_ms timeout or max time we are willing to wait for bits to clear
    *   @returns whether or or not we we able to get set bits or not
    */
    bool wait(thread_signal_t thread_signal, uint32_t timeout_ms);

    /*!
    *   @brief Checks to see if a bit is cleared or set. If bit was set, then we cleared it. 
    *   @param thread_signal_t which bit we want to check
    *   @param uint32_t timeout_ms timeout or max time we are willing to wait for bits to clear
    *   @returns whether or or not we we able to get set bits or not
    */
    bool wait_n_clear(thread_signal_t thread_signal, uint32_t timeout_ms); 
    
    /*!
    *   @brief Waits for bits to be set indefinitly
    *   @param thread_signal_t thread_signal to be sete
    */    
    void wait_notimeout(thread_signal_t thread_signal);
    
    /*!
    *   @returns The raw bits that hold our signals.
    */
    uint32_t bits_return(void);

    private: 
    // Bits data that we are using to wait with 
    volatile uint32_t bits = 0; 
};

#endif 
#endif