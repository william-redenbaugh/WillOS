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

    uint32_t start = millis(); 

    while(1){
        if(this->tryEntry())
            return SEMAPHORE_ACQUIRE_SUCCESS;

        // If we can't acquire the lock :(
        if(timeout_ms && (millis() - start > timeout_ms))
            break; 
        
        _os_yield(); 
    }

    __flush_cpu_pipeline(); 

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
    
    while(1){
        if(this->tryEntry())
            return;
        _os_yield(); 
    }
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