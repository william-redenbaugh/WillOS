#ifndef _OSMUTEXKERNEL_H
#define _OSMUTEXKERNEL_H

// So we can configure modules
#include "../enabled_modules.h"

#ifdef MUTEX_MODULE

#include "OSThreadKernel.h"
#include "DS_HELPER/priority_queue.hpp"

/*!
* @brief The different states that one can get from the mutex
*/
enum MutexLockState_t{
  MUTEX_UNLOCKED = 0, 
  MUTEX_LOCKED = 1
};

/*!
* @brief Enumerated success or failiure of acquiring the mutex
*/
enum MutexLockReturnStatus{
  MUTEX_ACQUIRE_SUCESS  = 1, 
  MUTEX_ACQUIRE_FAIL    = 0
};

/*!
* @brief Object descriptor to control a semaphore 
*/
class MutexLock{
  public:
    
    /*!
    * @brief Allows us to check the current state of our mutex
    * @returns MutexLockState_t state of the mutex
    */
    MutexLockState_t getState(void);

    /*!
    * @brief Allows us to lock our mutex
    * @param timeout_ms
    * @returns MutexLockReturnStatus or whether or not we were able to get the mutex
    */
    MutexLockReturnStatus lock(uint32_t timeout_ms);

    /*!
    * @brief Attempt to lock the mutex without timeout. 
    * @returns MutexLockReturnState state of whether or not we locked the mutex or not
    */
    MutexLockReturnStatus tryLock(void);
    
    /*!
    * @brief Waits for the lock indefinitely
    */
    void lockWaitIndefinite(void);

    /*!
    * @brief Unlocks a mutex if it hasn't been otherwise locked. 
    */
    void unlock(void);

  private: 
    volatile uint32_t state = MUTEX_UNLOCKED;

};

#endif
#endif 