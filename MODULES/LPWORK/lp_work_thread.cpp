#include "lp_work_thread.h"
#include <vector>

#ifdef LPWORK_MODULE

/*
Author: William Redenbaugh
Last Edit Date: 8/24/2020
*/

/**************************************************************************/
/*!
    @brief Thread pointer struct that holds everything we need to run a mini-thread
*/
/**************************************************************************/
struct LwipThread{
    // Pass in function as arguement
    void (*func)(void *ptr);
    // Pointer to function arguements
    void *ptr; 
    uint32_t next_exec_time; 
    uint32_t interval_time;
    bool thread_en = true; 
    uint32_t handle = 0; 
};

/*!
*   @brief Number of currently active low priority work threads. 
*/
volatile uint16_t num_lwip_threads = 0;

volatile uint32_t handle_increment = 0;  
volatile uint32_t lowest_interval = 0; 

// List of low priority functions and parameters, with exectution time.
std::vector<LwipThread> thread_list;  
MutexLock lpwork_mutex; 

//LwipThread thread_list[NUM_MAX_LPWTHREADS];
extern LPThreadInitReturn add_lwip_task(void (*func)(void *ptr),  void *args, uint32_t interval_ms);

extern void setup_lwip_thread(void);

/*!
*   @brief checks to see if a task is ready and should be triggered
*   @param Which task we are specifying
*/
static inline bool check_task_trigger(uint16_t num){
    return (thread_list[num].next_exec_time <= millis()) && (thread_list[num].thread_en); 
}

/*!
*   @brief Periodic task that runs through, executes the tasks we want it to, then sleeps until next required task
*   @notes Should be run in another loop.
*/
static inline void run_tasks(void){
    // Wait until we have the lpwork mutex
    lpwork_mutex.lockWaitIndefinite();
    
    // Minimum ticks until we need to circle back around and get to all the lwip threads
    uint32_t min_tick = thread_list[0].next_exec_time; 
    // run through entire list of functions and execute
    for(uint16_t num = 0; num < num_lwip_threads; num++){
        // If it's about time to run the function. 
        if(check_task_trigger(num)){
            // We run the function
            thread_list[num].func(thread_list[num].ptr);

            // Set the next time we want the function to run
            thread_list[num].next_exec_time = thread_list[num].interval_time + millis();
            // If there is a thread that must run in less time than currently specified for low
        }
        // priority thread to work in, decreate time for next thread sleep. 
        if(min_tick > thread_list[num].next_exec_time)
            min_tick = thread_list[num].next_exec_time; 
        
    }
    lpwork_mutex.unlock();
    
    // Just in case timer restarted or other issue that hangs a lwip thread, then we don't wait for a sleep command just in case things take longer than we want them to. 
    // We opt out of sleeping. 
    if(min_tick > millis())
        // Sleep for shortest remaining tick cycle until the next command 
        os_thread_delay_ms(min_tick - millis());
}

/*!
*   @brief Actual thread function that holds everything
*/
void lp_thread_func(void){
    for(;;){
        if(num_lwip_threads >= 1)
            run_tasks();

        // If there are no threads, then we just chill, sleep the thread for a short while
        // Also serves as a "time remainder thread" for now
        else
            os_thread_sleep_ms(10);
    }
}

/**************************************************************************/
/*!
    @brief Starts up our low priority work thread. 
*/
/**************************************************************************/
extern void setup_lwip_thread(void){
    os_add_thread((thread_func_t)lp_thread_func, NULL, 4092, NULL);
}

/**************************************************************************/
/*!
    @brief Allows us to add "sub-threads" to our low priority work thread
    @param void (*func)(void *ptr) pointer to the void ptr function
    @param void *args ptr arguements to pass into function
    @param system_t (time expected between each task interval)
    @returns LPThreadInitReturn(return struct with a couple of things thread management inside. )
*/
/**************************************************************************/
extern LPThreadInitReturn add_lwip_task(thread_func_t func,  void *args, uint32_t interval_ms){
    LPThreadInitReturn lp_return; 
    if(num_lwip_threads >= NUM_MAX_LPWTHREADS){
        lp_return.init_status = LP_THREAD_INIT_FAILIURE_MAX_THREAD;
        return lp_return; 
    }
    else{        
        lpwork_mutex.lockWaitIndefinite();
        thread_list.push_back(LwipThread());

        // Pass in function arguments and interval speed
        thread_list[num_lwip_threads].func = func; 
        thread_list[num_lwip_threads].ptr = args; 
        thread_list[num_lwip_threads].interval_time = interval_ms; 
        thread_list[num_lwip_threads].thread_en = true; 
        
        thread_list[num_lwip_threads].handle = handle_increment; 
        handle_increment++; 

        // Next time we want to run this function
        thread_list[num_lwip_threads].next_exec_time = interval_ms + millis();
        
        // Save LP return data
        lp_return.init_status = LP_THREAD_INIT_SUCCESS; 
        lp_return.thread_handle = thread_list[num_lwip_threads].handle; 

        // Increment number of threads currently
        num_lwip_threads++;
        lpwork_mutex.unlock();
        return lp_return; 
    }
}

/**************************************************************************/
/*!
    @brief disables a specifc low priority task
    @param uint32_t (handle of task) 
*/
/**************************************************************************/
extern LPThreadInitStatus_t disable_lwip_task(uint32_t thread_handle){
    if(thread_handle < num_lwip_threads){
        lpwork_mutex.lockWaitIndefinite();
        for(uint16_t n = 0; n < num_lwip_threads; n++){
            // find and search for the specific thread handle
            if(thread_list[thread_handle].handle == thread_handle){
                thread_list[thread_handle].thread_en = false; 
                return LP_THREAD_EN_SUCCESS; 
            }
        }
        lpwork_mutex.unlock();
    }
    return LP_THREAD_EN_UNDEFINED;
}

/**************************************************************************/
/*!
    @brief enables a specifc low priority task
    @param uint32_t (handle of task) 
*/
/**************************************************************************/
extern LPThreadInitStatus_t enable_lwip_task(uint32_t thread_handle){
    if(thread_handle < num_lwip_threads){
        lpwork_mutex.lockWaitIndefinite();
        for(uint16_t n = 0; n < num_lwip_threads; n++){
            if(thread_list[n].handle == thread_handle){
                thread_list[n].thread_en = true; 
                return LP_THREAD_EN_SUCCESS; 
            }
        }
        lpwork_mutex.unlock();
    }
    return LP_THREAD_EN_UNDEFINED;
}

/**************************************************************************/
/*!
    @brief deletes a specifc low priority task
    @param uint32+t (handle of task) 
*/
/**************************************************************************/
extern LPThreadInitStatus_t del_lwip_task(uint32_t thread_handle){
    if(thread_handle < num_lwip_threads){
        lpwork_mutex.lockWaitIndefinite();
        for(uint16_t n = 0; n < num_lwip_threads; n++){
            if(thread_list[n].handle == thread_handle){
                thread_list.erase(thread_list.begin() + n);
                return LP_THREAD_DELETE_SUCCESS; 
            }
        }
        lpwork_mutex.unlock();
    }
    return LP_THREAD_DELETE_UNDEFINED; 
}
#endif