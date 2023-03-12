#ifndef THREADS_INIT_H
#define THREADS_INIT_H
#include "sys/_stdint.h"
#include "OS/OSThreadKernel.h"

/**
 * @brief Structure that manages task initialization.
 * @note Designed to help intiailize lists of tasks all at once.
 */
typedef struct
{
    void (*task_fun)(void *task_params);
    void (*task_init_fun)(void *task_params);
    const char *task_name;
    uint16_t stack_size;
    uint16_t priority;
    void *param;
    os_thread_id_t handle;
} task_init_descriptor_t;

/**
 * @brief Initialize all the threads that we put into our list.
 *
 */
void threads_list_init(void);

extern task_init_descriptor_t THREAD_LIST[];
extern int NUM_THREADS;
#endif