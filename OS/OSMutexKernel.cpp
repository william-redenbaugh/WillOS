#include "OSMutexKernel.h"

/*
Author: William Redenbaugh, Fernando Trias
Last Edit Date: 8/9/2020
*/

/*
* @brief Allows us to check the current state of our mutex
* @returns MutexLockState_t state of the mutex
*/
MutexLockState_t MutexLock::getState(void){
  int os_state = os_stop(); 
  MutexLockState_t state = this->state; 
  os_start(os_state);
  return state; 
}

/*
* @brief Allows us to lock our mutex
* @params timeout_ms
* @returns MutexLockReturnStatus or whether or not we were able to get the mutex
*/
MutexLockReturnStatus __attribute__ ((noinline)) MutexLock::lock(uint32_t timeout_ms){
  if(this->tryLock())
    return MUTEX_ACQUIRE_SUCESS;
  
  uint32_t start = millis();

  while(1){
    // If we can acquire the lock!
    if(this->tryLock())
      return MUTEX_ACQUIRE_SUCESS; 

    // If we can't acquire the lock :(
    if(timeout_ms && (millis() - start > timeout_ms))
      return MUTEX_ACQUIRE_FAIL;

    // Reqlinquise this thread to someone else
    _os_yield();

  }

  // If we can't get the mutex, we clear out the cpu pipeline. 
  __flush_cpu_pipeline();
  // Let the managing thread know that we failed. 
  return MUTEX_ACQUIRE_FAIL;
}

/*
* @brief Attempt to lock the mutex without timeout. 
* @returns MutexLockReturnState state of whether or not we locked the mutex or not
*/
MutexLockReturnStatus MutexLock::tryLock(void){
  // Current state of the operating system. 
  int os_state = os_stop();
  
  // If the lock in unlocked, then we acquire it.
  if(this->state == MUTEX_UNLOCKED){
    // Let's acquire it!
    this->state = MUTEX_LOCKED;
    // We are done dealing with OS specific commands
    os_start(os_state);
    // We gottem
    return MUTEX_ACQUIRE_SUCESS;
  }

  // Otherwise we lose it, 
  os_start(os_state);
  // And we fail it. 
  return MUTEX_ACQUIRE_FAIL;
}

/*
* @brief Waits for the lock indefinitely
*/
void __attribute__ ((noinline)) MutexLock::lockWaitIndefinite(void){
  if(this->tryLock())
    return;
  
  while(1){
    // If we can acquire the lock!
    if(this->tryLock())
      return; 

    // Reqlinquise this thread to someone else
    _os_yield();
  }
  // If we can't get the mutex, we clear out the cpu pipeline. 
  __flush_cpu_pipeline();
  // Let the managing thread know that we failed. 
  return;
}

/*
* @brief Unlocks a mutex if it hasn't been otherwise locked. 
*/
void __attribute__ ((noinline)) MutexLock::unlock(void){
  int os_state = os_stop();
  
  if(this->state == MUTEX_LOCKED)
    this->state = MUTEX_UNLOCKED;

  __flush_cpu_pipeline();
  os_start(os_state);
} 
