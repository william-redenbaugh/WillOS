#ifndef _OSMUTEXKERNEL_H
#define _OSMUTEXKERNEL_H

#include "OSThreadKernel.h"

/*
* @brief The different states that one can get from the mutex
*/
enum MutexLockState_t{
  MUTEX_UNLOCKED, 
  MUTEX_LOCKED
};

enum MutexLockReturnStatus{
  MUTEX_ACQUIRE_SUCESS  = 1, 
  MUTEX_ACQUIRE_FAIL    = 0
};

/*
* @brief Object Reference to control our mutexes 
* @notes So we can take care of our mutex stuff
*/
class MutexLock{
  public: 
    MutexLockState_t getState(void);
    MutexLockReturnStatus lock(uint32_t timeout_ms);
    MutexLockReturnStatus tryLock(void);
    
    void lockWaitIndefinite(void);
    void unlock(void);

  private: 
    volatile MutexLockState_t state = MUTEX_UNLOCKED;
};

#endif 