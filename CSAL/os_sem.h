#ifndef _SEM_H
#define _SEM_H

#include "OS/OSSemaphoreKernel.h"

typedef struct os_sem_t{
    SemaphoreLock lock;
}os_sem_t;

/**
 * @brief Initializes a semaphore module
 * @param os_sem_t 
*/
int os_sem_init(os_sem_t *sem, int count);

/**
 * @brief Allows us to acquuire our semaphore
 * 
*/
int os_sem_entry(os_sem_t *sem, uint32_t timeout_ms);

/**
 * @brief How many re-entrants into the lock
*/
int os_sem_count(os_sem_t *sem);

/**
 * @brief Waits indefinitely until the semaphore has a rentrant lock available
*/
int os_sem_entry_wait_indefinite(os_sem_t *sem);

#endif