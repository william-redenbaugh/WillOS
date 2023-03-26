#include "threads_init.h"

void threads_list_init(void)
{
    for (int n = 0; n < NUM_THREADS; n++)
    {
        if(THREAD_LIST[n].task_init_fun!= NULL)
            THREAD_LIST[n].task_init_fun(NULL);
    }

    // Launch each task!
    for (int n = 0; n < NUM_THREADS; n++)
    {
        if(THREAD_LIST[n].task_fun != NULL){
            // New block of memory from heap for stack pointer
            uint8_t *stack_ptr;
            if(THREAD_LIST[n].stack == NULL){
                stack_ptr = new uint8_t[THREAD_LIST[n].stack_size];
            }
            else{
                stack_ptr = THREAD_LIST[n].stack;
            }
            os_add_thread((thread_func_t)THREAD_LIST[n].task_fun, THREAD_LIST[n].param, THREAD_LIST[n].stack_size, stack_ptr);
        }
    }
}