#ifndef _ENABLED_MODULES_H
#define _ENABLED_MODULES_H

/*!
*   @brief Fast Malloc module for allocated HEAP data onto RAMBANK1 on Teensy4
*   @note This is threadsafe module, non-thread-safe module is always availabe since it's used in scheduler
*/
// #define OS_FAST_MALLOC_MODULE

/*!
*   @brief Low priority work thread module
*/
#define LPWORK_MODULE

/*!
*   @brief Protobuffer Callbacks Handler module
*/
#define PROTOCALLBACKS_MODULE

/*!
*   @brief Operating System Semaphore Module
*/
#define SEMAPHORE_MODULE

/*!
*   @brief Operating System Mutex Module
*/
#define MUTEX_MODULE

/*!
*   @brief Operating System Signaling Module
*/
#define SIGNALING_MODULE

/*!
*   @brief Included SSD1351 OLED Module for easy DMA manipulation
*   @note Depends on OSThreadKernel Module
*/
// #define SSD1351_MODULE

/*!
*   @brief Included APA102 SPI Based LED strip module for easy DMA manipulation
*   @note Depends on OSThreadKernel Module
*/
// #define APA102_MODULE

/*!
*   @brief Easy module that allows us to read voltages 
*/
#define VOLTAGE_READ_MODULE

#endif 