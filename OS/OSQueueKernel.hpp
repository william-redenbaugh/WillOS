#ifndef _OS_QUEUE_KERNEL_HPP
#define _OS_QUEUE_KERNEL_HPP

#include "OSSignalKernel.h"
#include "OSMutexKernel.h"

class VoidOSQueue{
    MutexLock queue_lock;
    uint32_t queue_len = 0; 
    uint32_t current_elements = 0;
    void **data_buffer; 
    int head = 0; 
    int tail = 0; 
    bool full = false;

    /**
     * @brief Initializes the Queue, based off the number of elements
     * @param length of elements we want to be able to have in the queue
     * @return boolean for sucess or failiure of feature generation
    */
    bool init(uint32_t queue_len);
    
    /**
     * @brief Allows us to add items into the queue, for consumption later
     * @param void* data pointer to whatever data you want to add to the queue
     * @return boolean for success or failiure of pushing elements
    */
    bool push(void *data);
    
    /**
     * @brief Removes off topmost element from queue.
     * @return void* pointer to topmost data
     * @note Returns NULL if there is no available element
    */
    void* pop(void);
};

#endif