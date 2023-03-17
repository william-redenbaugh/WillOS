# Thread Initialization Module

Just a simple module that lets you have a framework for which to initialize and run threads. 

### Instructions
In your threads list file file, import the THREAD_INIT/threads_init.h file and them build a list to hold all your threads

```

#include "THREAD_INIT/threads_init.h"

#include "thread_one_file_location.h"
#include "thread_two_file_location.h"

task_init_descriptor_t THREAD_LIST=                                                     \
    {                                                                                   \
        {thread_one, thread_one_init, "Thread one name", 2048, 1, NULL, NULL},          \
        {thread_two, thread_two_init, "Thread two name", 2048, 1, NULL, NULL},          \
    };

int NUM_THREADS  = sizeof(THREAD_LIST)/sizeof(task_init_descriptor_t);

```