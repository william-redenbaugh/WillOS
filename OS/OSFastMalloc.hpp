#ifndef _OSFASTMALLOC_HPP
#define _OSFASTMALLOC_HPP

// This module is essentially just a wrapper over the fast_malloc module
#include "DS_HELPER/fast_malloc.hpp"

// Operating System Stuff
#include "OS/OSThreadKernel.h"
#include "OS/OSMutexKernel.h"

#include "enabled_modules.h"

#ifndef OS_FAST_MALLOC_MODULE

/*!
*   @brief The mutex that will allow our fast mallocs to be threadsafe!
*/
static MutexLock fast_malloc_mutex; 

/*!
*   @brief Mutex wrapper that allows our fast mallocs to be threadsafe!
*   @param size_t (size of item we want to malloc)
*/
void *os_fast_malloc(size_t size){
    fast_malloc_mutex.lockWaitIndefinite(); 
    void *ptr = fast_malloc(size); 
    fast_malloc_mutex.unlock(); 
}

/*!
*   @brief Mutex wrapper that allows our fast free-heaps to be threadsafe!
*   @param void *ptr
*/
void *os_fast_malloc_free(void *ptr){
    fast_malloc_mutex.lockWaitIndefinite(); 
    fast_malloc_free(ptr);  
    fast_malloc_mutex.unlock(); 
}

/*!
*   @return size of the referenced block
*/
uint16_t os_fast_malloc_block_size(void *ptr){
    fast_malloc_mutex.lockWaitIndefinite(); 
    register uint16_t ret = fast_malloc_memblock_size(ptr); 
    fast_malloc_mutex.unlock(); 

    return ret; 
}
#endif
#endif 