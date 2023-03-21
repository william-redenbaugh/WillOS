#ifndef _SEM_H
#define _SEM_H

#include "../OS/OSSemaphoreKernel.h"

typedef struct os_sem{
    SemaphoreLock lock;
}os_semt_t;

/**
 * @brief Initializes a semaphore module
 * @param os_sem_t 
*/
void os_sem_init(os_sem_t *sem);

/**
 * @brief Allows us to acquuire our semaphore
 * 
*/
void os_sem_entry(os_sem_t *sem, uint32_t timeout_ms);


#endif