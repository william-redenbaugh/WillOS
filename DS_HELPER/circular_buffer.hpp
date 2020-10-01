#ifndef _CIRCULAR_BUFFER_H
#define _CIRCULAR_BUFFER_H

#include "../enabled_modules.h"

#ifdef CIRCULAR_BUFFER_STRING_MODULE 

#include <Arduino.h> 
#include "DS_HELPER/fast_malloc.hpp"

class CircularBufferString{
public: 

#ifdef __IMXRT1062__
    /*!
    *   @brief Initializes the circular buffer with initalized array
    *   @param uint32_t buffer_size(size of buffer you want your str buffer to have)
    *   @param bool whether or not we allocated onto the faster RAM bank, or the slower RAM bank
    */
    void init(uint32_t buffer_size, bool fast_mem); 
#else
    /*!
    *   @brief Initializes the circular buffer with initalized array
    *   @param uint32_t buffer_size(size of buffer you want your str buffer to have)
    */
    void init(uint32_t buffer_size); 
#endif

    /*!
    *   @brief Initializes the circular buffer with initalized array
    *   @param uint32_t buffer_size(size of buffer you want your str buffer to have)
    *   @param char* pointer to ringbuffer array memory. 
    */
    void init(uint32_t buffer_size, char *arr);

    /*!
    *   @brief Insert a string of defined size into the circular buffer
    *   @param char* (character array pointer that we are inserting into the system)
    *   @param size_t len(length of string that we are inserting into the system)
    */
    bool insert(char *arr, size_t len);

    /*!
    *   @brief Deconstructs array
    */
    void deinit(void); 
  
private: 

    /*!
    *   @brief Ringbuffer array and length. 
    */
    char *buffer = NULL; 
    uint32_t buffer_length; 

    /*!
    *   @brief Current position of the ring buffer 
    */ 
    uint32_t current_position; 

#ifdef __IMXRT1062__
    /*!
    *   @brief Whether or not we are using the fast memory bank
    */
    bool fast_mem; 
#endif

};
#endif 

#ifdef CIRCULAR_BUFFER_POINTER_MODULE

class CircularBufferPointer{
public: 

#ifdef __IMXRT1062__
    /*!
    *   @brief Initializes the circular buffer with initalized array
    *   @param uint32_t buffer_size(size of buffer you want your str buffer to have)
    *   @param bool whether or not we allocated onto the faster RAM bank, or the slower RAM bank
    */
    void init(uint32_t buffer_size, bool fast_mem); 
#else
    /*!
    *   @brief Initializes the circular buffer with initalized array
    *   @param uint32_t buffer_size(size of buffer you want your str buffer to have)
    */
    void init(uint32_t buffer_size); 
#endif

    /*!
    *   @brief Initializes the circular buffer with initalized array
    *   @param uint32_t buffer_size(size of buffer you want your str buffer to have)
    *   @param char* pointer to ringbuffer array memory. 
    */
    void init(uint32_t buffer_size, void **arr);

    /*!
    *   @brief Insert a string of defined size into the circular buffer
    *   @param char* (character array pointer that we are inserting into the system)
    *   @param size_t len(length of string that we are inserting into the system)
    */
    bool insert(void *ptr);

    /*!
    *   @brief Deconstructs array
    */
    void deinit(void); 

private: 

    /*!
    *   @brief General purpose pointer array to pointers. Ringbuffer array and length
    */
    void **ptr; 
    uint32_t buffer_len; 

    /*!
    *   @brief Current position of the ring buffer 
    */ 
    uint32_t current_position; 

    #ifdef __IMXRT1062__
    /*!
    *   @brief Whether or not we are using the fast memory bank
    */
    bool fast_mem; 
#endif

};

#endif
#endif 