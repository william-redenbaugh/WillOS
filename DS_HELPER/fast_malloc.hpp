#ifndef _FAST_MALLOC_HPP
#define _FAST_MALLOC_HPP

#include <Arduino.h> 
#include "OS/OSThreadKernel.h"

/*!
*   @brief The amount of space that 
*   @note In Bytes
*/
const unsigned int FAST_MALLOC_SIZE_BYTES = 131072;

/*!
*   @brief Set's up the malloc
*/
void fast_malloc_init(void); 

/*!
*   @brief Allocates memory on fast memory bank
*   @return Pointer to the area in memory. 
*/
void* fast_malloc(size_t size); 

/*!
*   @brief Frees up the memory in the data. 
*   @param Pointer to the memory bank data. 
*/
void fast_malloc_free(void *ptr); 

#endif