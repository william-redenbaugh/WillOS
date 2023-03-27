#ifndef _OSSEMAPHOREKERNEL_H
#define _OSSEMAPHOREKERNEL_H

// So we can configure modules
#include "../enabled_modules.h"

#ifdef SEMAPHORE_MODULE

#include <Arduino.h>
#include "OSThreadKernel.h"

/*!
* @brief Enumerated success or failiure of acquiring the semaphore
*/
enum SemaphoreLockReturnStatus{
    SEMAPHORE_ACQUIRE_SUCCESS = 1,
    SEMAPHORE_ACQUIRE_FAIL = 0,
};

/**
 * @brief Enumerated success or failure of exiting the semaphore
*/
enum SemaphoreExitReturnStatus{
    SEMAPHORE_EXIT_SUCCCESS = 1, 
    SEMAPHORE_EXIT_FAIL = 0
};

/**
 * @brief Little data structure that gives us the count and return status of our semaphore
*/
typedef struct SemaphoreRet{
    SemaphoreLockReturnStatus ret_status;
    int count;
}SemaphoreRet;

/*!
*   @brief Object descriptor to control a semaphore
*   @brief By default a binary semaphore()
*/
class SemaphoreLock{
    public:
        SemaphoreLock(int count){
            max_entry = count;
        }

        /*!
        *   @brief Get's the current entrants / states of the semaphore
        *   @return SemaphoreLockState_t state of the semaphore
        */
        uint32_t getState(void);

        /*!
        * @brief Allows us to acquire our semaphore
        * @param timeout_ms
        * @returns SemaphoreLockReturnStatus or whether or not we were able to get the mutex
        */
        SemaphoreRet entry(uint32_t timeout_ms);

        /*!
        * @brief Allows us to acquire our semaphore
        * @param timeout_ms
        * @returns SemaphoreLockReturnStatus or whether or not we were able to get the mutex
        */
        SemaphoreRet tryEntry(void);

        /*!
        *   @brief Waits
        */
        int entryWaitIndefinite(void);

        /*!
        *   @brief Decrements the current semaphore counter
        */
        SemaphoreExitReturnStatus exit(void);

    private:
        /*!
        *   @brief
        */
        volatile uint32_t state = 0;
        uint32_t max_entry = 1;
};

#endif
#endif