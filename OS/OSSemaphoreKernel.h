#ifndef _OSSEMAPHOREKERNEL_H
#define _OSSEMAPHOREKERNEL_H

#include <Arduino.h> 
#include "OSThreadKernel.h"

/*!
* @brief Enumerated success or failiure of acquiring the semaphore
*/
enum SemaphoreLockReturnStatus{
    SEMAPHORE_ACQUIRE_SUCCESS = 1, 
    SEMAPHORE_ACQUIRE_FAIL = 0, 
}; 



/*!
*   @brief Object descriptor to control a semaphore
*   @brief By default a binary semaphore()
*/
class SemaphoreLock{
    public: 
        uint32_t getState(void); 
        SemaphoreLockReturnStatus entry(uint32_t timeout_ms); 
        SemaphoreLockReturnStatus tryEntry(void); 

        /*!
        *   @brief Waits
        */
        void entryWaitIndefinite(void); 

        /*!
        *   @brief Decrements the current semaphore counter
        */
        void exit(void); 

    private: 
        volatile uint32_t state = 0; 
        uint32_t max_entry = 1; 
}; 

#endif 