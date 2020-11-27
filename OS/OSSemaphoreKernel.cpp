#include "OSSemaphoreKernel.h"

#ifdef SEMAPHORE_MODULE

/*!
*   Author: William Redenbaugh
*   Last Edite Date: 9/23/2020
*/

/*!
*   @brief Get's the current entrants / states of the semaphore 
*   @return SemaphoreLockState_t state of the semaphore
*/
uint32_t SemaphoreLock::getState(void){
    int os_state = os_stop(); 
    uint32_t state = this->state; 
    os_start(os_state);
    return state; 
}

/*!
* @brief Allows us to acquire our semaphore
* @param timeout_ms
* @returns SemaphoreLockReturnStatus or whether or not we were able to get the mutex
*/
SemaphoreLockReturnStatus __attribute__ ((noinline))SemaphoreLock::entry(uint32_t timeout_ms){
    if(this->tryEntry())
        return SEMAPHORE_ACQUIRE_SUCCESS; 

    int state = os_stop(); 

    thread_t *this_thread = _os_current_thread(); 
    this_thread->flags = THREAD_BLOCKED_SEMAPHORE_TIMEOUT; 
    this_thread->interval = timeout_ms; 
    this_thread->previous_millis = millis(); 

    // Setting up the semaphore current and future counts here
    this_thread->mutex_semaphore = &this->state; 
    this_thread->semaphore_max_count = this->max_entry; 

    os_start(state); 
    _os_yield(); 

    __flush_cpu_pipeline(); 
    
    if(this->tryEntry())
        return SEMAPHORE_ACQUIRE_SUCCESS; 

    // So the compiler stops giving me errors. 
    return SEMAPHORE_ACQUIRE_FAIL;    
}

/*!
*   @brief Trying to enter our semaphore
*   @returns SemaphoreLockReturnStatus or whether or not we were able to get the mutex
*/
SemaphoreLockReturnStatus SemaphoreLock::tryEntry(void){
    int os_state = os_stop(); 

    if(this->state < this->max_entry){
        this->state++; 
        os_start(os_state); 
        return SEMAPHORE_ACQUIRE_SUCCESS; 
    }

    os_start(os_state); 

    return SEMAPHORE_ACQUIRE_FAIL; 
}

/*!
* @brief Waits for the semaphore indefinitely
*/
void __attribute__ ((noinline)) SemaphoreLock::entryWaitIndefinite(void){
    if(this->tryEntry())
        return; 

    int state = os_stop(); 

    // Pointer to the current thread. 
    thread_t *this_thread = _os_current_thread(); 

    // Setting up the semaphore current and future counts here
    this_thread->flags = THREAD_BLOCKED_SEMAPHORE; 
    this_thread->mutex_semaphore = &this->state; 
    this_thread->semaphore_max_count = this->max_entry; 
    
    os_start(state);
    _os_yield();
    __flush_cpu_pipeline(); 
    
    // Increment the semaphore so it's valid. 
    this->tryEntry(); 

    // So the compiler stops giving me errors. 
    return;     
}

/*!
*   @brief Decrements the semaphore counter. 
*/
void __attribute__ ((noinline)) SemaphoreLock::exit(void){
    int os_state = os_stop();;
    this->state--; 
    __flush_cpu_pipeline(); 
    os_start(os_state); 
}

#endif