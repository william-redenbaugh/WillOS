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
  MutexLockState_t state = this->state; 
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

  // If the lock in unlocked, then we acquire it before we context switch just in case, to prevent any delays
  if(this->state == MUTEX_UNLOCKED){
    // Let's acquire it!
    this->state = MUTEX_LOCKED;
    // We are done dealing with OS specific commands
    os_start(os_state);
    // We gottem
    return MUTEX_ACQUIRE_SUCESS;
  }
  
  // Otherwise we sleep the thread: 
  thread_t *current_thread = _os_current_thread(); 
  
  // When will the thread run next if it isn't premepted yet
  current_thread->previous_millis = millis(); 
  current_thread->interval = timeout_ms; 

  // Set thread flags to sleeping. 
  current_thread->flags = THREAD_SLEEPING; 

  // Whether or not we want to insert in a new place in the thread list or not. 
  bool insert = true; 
  PriorityQueueHeapNode* all_elements = thread_list.all_elements(); 
  for(int n = 0; n < thread_list.num_elemnts(); n++)
    // 
    if(all_elements[n].ptr == (void*) current_thread)
      insert = false; 
  

  // If this thread wasn't already saved in the heap, we place it there. 
  if(insert)
    // Taking care of thread stuff
    thread_list.insert((void*)current_thread, current_thread->thread_priority);

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
  
  // Otherwise we sleep the thread: 
  thread_t *current_thread = _os_current_thread(); 

  // Set thread flags to sleeping. 
  current_thread->flags = THREAD_SUSPENDED; 

  // Taking care of thread stuff
  thread_list.insert((void*)current_thread, current_thread->thread_priority);

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

  // If we have any threads waiting on the mutex. 
  if(thread_list.num_elemnts() != 0){

    // We get the highest priority thread first. 
    thread_t *next_thread = (thread_t*)thread_list.pop(); 

    // Since our code is lazy and only runs at the last moment
    // When we are looking for the next thread to activate, 
    // If that thread has already been unlocked via timeout, we don't remove it from the heap
    // This gets really messy and I need to find a better way to deal with this. 
    while(next_thread->flags != THREAD_RUNNING &&  thread_list.num_elemnts() != 0)
      next_thread = (thread_t*) thread_list.pop();

    // And we wake it up. 
    next_thread->flags = THREAD_RUNNING; 
  }
  
  __flush_cpu_pipeline();
  os_start(os_state);
} 

#endif