#ifndef _LP_WORK_THREAD_H
#define _LP_WORK_THREAD_H

// So we can configure modules
#include "../../enabled_modules.h"

#ifdef LPWORK_MODULE

#include <Arduino.h> 
#include "OS/OSThreadKernel.h"
#include "OS/OSMutexKernel.h"

// Maximum number of low priority work threads available
#define NUM_MAX_LPWTHREADS 256

/**************************************************************************/
/*!
    @brief Enumerated Status for thread initialization
*/
/**************************************************************************/
enum LPThreadInitStatus_t{
    LP_THREAD_INIT_SUCCESS, 
    LP_THREAD_INIT_FAILIURE_UNKOWN,
    LP_THREAD_REPLACE_UNDEFINED, 
    LP_THREAD_DELETE_UNDEFINED, 
    LP_THREAD_DELETE_SUCCESS,
    LP_THREAD_EN_UNDEFINED,
    LP_THREAD_EN_SUCCESS, 
    LP_THREAD_INIT_FAILIURE_MAX_THREAD
};

/**************************************************************************/
/*!
    @brief Returned status for Low priority thread creation
*/
/**************************************************************************/
struct LPThreadInitReturn{
    uint32_t thread_handle = 0; 
    LPThreadInitStatus_t init_status; 
};

// Starting up our low priority work thread. 
extern void setup_lwip_thread(void);
extern LPThreadInitReturn add_lwip_task(void (*func)(void *ptr),  void *args, uint32_t interval_ms);
extern LPThreadInitStatus_t disable_lwip_task(uint32_t thread_handle);
extern LPThreadInitStatus_t enable_lwip_task(uint32_t thread_handle);
extern LPThreadInitStatus_t del_lwip_task(uint32_t thread_handle);

#endif 
#endif