#include "OSMutexKernel.h"
#ifdef MUTEX_MODULE

/*
Author: William Redenbaugh, Fernando Trias
Last Edit Date: 10/24/2020
*/

/*!
* @brief Allows us to check the current state of our mutex
* @returns MutexLockState_t state of the mutex
*/
MutexLockState_t MutexLock::getState(void){
  int os_state = os_stop(); 
  MutexLockState_t state = (MutexLockState_t)this->state; 
  os_start(os_state);
  return state; 
}

/*!
* @brief Allows us to lock our mutex
* @param timeout_ms
* @returns MutexLockReturnStatus or whether or not we were able to get the mutex
*/
MutexLockReturnStatus __attribute__ ((noinline)) MutexLock::lock(uint32_t timeout_ms){
  // Stop the kernel for mission critical stuff. 
  int os_state = os_stop(); 
  
  thread_t *this_thread = _os_current_thread(); 
  this_thread->mutex_semaphore = &this->state; 
  this_thread->flags = THREAD_BLOCKED_MUTEX_TIMEOUT; 
  this_thread->interval = timeout_ms; 
  this_thread->previous_millis = millis(); 

  // reboot the OS kernel. 
  os_start(os_state); 
  // Context switch out of the thread. 
  _os_yield(); 

  // Stop the OS again when checking mutex stuff
  os_state = os_stop();
  // If the lock in unlocked, then we acquire it.
  if(this->state == MUTEX_UNLOCKED){
    // Let's acquire it!
    this->state = MUTEX_LOCKED;
    // We are done dealing with OS specific commands
    os_start(os_state);
    // We gottem
    return MUTEX_ACQUIRE_SUCESS;
  } 
  else{
    os_start(os_state); 
    return MUTEX_ACQUIRE_FAIL; 
  }
}

/*!
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

/*!
* @brief Waits for the lock indefinitely
*/
void __attribute__ ((noinline)) MutexLock::lockWaitIndefinite(void){
  // Stop the kernel for mission critical stuff. 
  int os_state = os_stop(); 

  // If the lock in unlocked, then we acquire it before we context switch just in case, to prevent any delays
  if(this->state == MUTEX_UNLOCKED){
    // Let's acquire it!
    this->state = MUTEX_LOCKED;
    // We are done dealing with OS specific commands
    os_start(os_state);
    // We gottem
    return; 
  }

  thread_t *this_thread = _os_current_thread(); 
  this_thread->mutex_semaphore = &this->state; 
  this_thread->flags = THREAD_BLOCKED_MUTEX;  
  // reboot the OS kernel. 
  os_start(os_state); 

  // Context switch out of the thread. 
  _os_yield(); 

  // Stop the OS again when checking mutex stuff
  os_state = os_stop();
  // If the lock in unlocked, then we acquire it.
  if(this->state == MUTEX_UNLOCKED){
    // Let's acquire it!
    this->state = MUTEX_LOCKED;
    // We are done dealing with OS specific commands
    os_start(os_state);
    // We gottem
    return; 
  } 
}

/*!
* @brief Unlocks a mutex if it hasn't been otherwise locked. 
*/
void __attribute__ ((noinline)) MutexLock::unlock(void){
  int os_state = os_stop();
  // Mutex is set to unlocked flag. 
  this->state = MUTEX_UNLOCKED;
  
  __flush_cpu_pipeline();
  os_start(os_state);
} 

#endif