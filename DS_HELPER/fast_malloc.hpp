#ifndef _FAST_MALLOC_HPP
#define _FAST_MALLOC_HPP

#include <Arduino.h> 
#include "OS/OSThreadKernel.h"

#ifdef OS_FAST_MALLOC_MODULE

/*!
*   @brief The amount of space that 
*   @note In Bytes
*/
const unsigned int FAST_MALLOC_SIZE_BYTES = 131072;

/*!
*   @brief Allocates memory on fast memory bank
*   @return Pointer to the area in memory. 
*/
void* fast_malloc(size_t size); 

/*!
*   @brief Frees up the memory in the data. 
*   @param Pointer to the memory bank data. 
*   @note it's very possible to break this function, so really make sure that the block you are checking is 
*   @note a "fast malloced block
*/
void fast_malloc_free(void *ptr); 

/*!
*   @brief 
*   @param void *ptr(memory block start location
*   @return size_t size of memory block. 
*   @note it's very possible to break this function, so really make sure that the block you are checking is 
*   @note a "fast malloced block
"
*/
size_t fast_malloc_memblock_size(void *ptr); 

#endif

#endif