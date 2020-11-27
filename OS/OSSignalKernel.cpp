#include "OSSignalKernel.h"

#ifdef SIGNALING_MODULE

/*!
*   @brief Signals a bit
*   @param thread_signal_t which signal we are setting
*/
void OSSignal::signal(thread_signal_t thread_signal){
    int state = os_stop(); 
    this->bits |= (1 << (uint32_t)thread_signal);
    os_start(state); 
}

/*!
*   @brief clears a bit
*   @param thread_signal_t which signal we are clearing
*/
void OSSignal::clear(thread_signal_t thread_signal){
    int state = os_stop(); 
    this->bits &= ~(1 << (uint32_t)thread_signal);
    os_start(state); 
}

/*!
*   @brief Checks to see if a bit is cleared or set
*   @param thread_signal_t which bit we want to check
*   @returns The status of the signal we are checking 
*/
bool OSSignal::check(thread_signal_t thread_signal){
    if(OS_CHECK_BIT(this->bits, (uint32_t)thread_signal))
        return true; 
    return false;    
}


/*!
*   @brief Checks to see if a bit is cleared or set
*   @param thread_signal_t which bit we want to check
*   @param uint32_t timeout_ms timeout or max time we are willing to wait for bits to clear
*   @returns whether or or not we we able to get set bits or not
*/
bool OSSignal::wait(thread_signal_t thread_signal, uint32_t timeout_ms){
    // Checking case immediatly. 
    if(OS_CHECK_BIT(this->bits, (uint32_t)thread_signal))
        return true; 

    // Stop the operating system so we can make changes to the thread. 
    int os_state = os_stop(); 

    thread_t *this_thread = _os_current_thread(); 
    
    // Set the bit that the thread is waiting on
    this_thread->signal_bit = &this->bits; 
    // Which bits are we comparing to
    this_thread->signal_bits_compare = (1 << (uint32_t)thread_signal);
    this_thread->previous_millis = millis(); 
    this_thread->interval = timeout_ms; 
    this_thread->flags = THREAD_BLOCKED_SIGNAL_TIMEOUT; 
    
    // Restart the OS after we have completed touching the thread. 
    os_start(os_state); 

    // Context switch out of the thread. 
    _os_yield(); 

    // If were able to get access to the signal
    if(OS_CHECK_BIT(this->bits, (uint32_t)thread_signal))
        return true; 

    // Otherwise we weren't able to
    return false; 
}

/*!
*   @brief Checks to see if a bit is cleared or set. If bit was set, then we cleared it. 
*   @param thread_signal_t which bit we want to check
*   @param uint32_t timeout_ms timeout or max time we are willing to wait for bits to clear
*   @returns whether or or not we we able to get set bits or not
*/
bool OSSignal::wait_n_clear(thread_signal_t thread_signal, uint32_t timeout_ms){
    // We waited, and eventually the signal worked out for us 
    if(this->wait(thread_signal, timeout_ms)){
        this->clear(thread_signal);
        return true;
    }

    // Weren't able to what we wanted. 
    return false; 
}

/*!
*   @brief Waits for bits to be set indefinitly
*   @param thread_signal_t thread_signal to be sete
*/
void OSSignal::wait_notimeout(thread_signal_t thread_signal){
    // Checking case immediatly. 
    if(OS_CHECK_BIT(this->bits, (uint32_t)thread_signal))
        return;

    // Stop the operating system so we can make changes to the thread. 
    int os_state = os_stop();

    thread_t *this_thread = _os_current_thread(); 
    
    // Set the bit that the thread is waiting on
    this_thread->signal_bit = &this->bits; 
    // Which bits are we comparing to
    this_thread->signal_bits_compare = (1 << (uint32_t)thread_signal);
    this_thread->flags = THREAD_BLOCKED_SIGNAL; 

    // Restart the OS after we have completed touching the thread. 
    os_start(os_state); 

    // Context switch out of the thread. Whatever happens happens. 
    _os_yield(); 
}

/*!
*   @returns The raw bits that we are using
*/
uint32_t OSSignal::bits_return(void){
    return this->bits; 
}
#endif