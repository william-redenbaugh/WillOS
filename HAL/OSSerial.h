#ifndef _OSSERIAL_H
#define _OSSERIAL_H

#include <Arduino.h> 
#include "../OS/OSMutexKernel.h"
#include "../OS/OSSignalKernel.h" 
#include "../OS/OSThreadKernel.h" 

/*
*   @brief USB controls through the TeensyOS, initialization
*/
void os_usb_serial_begin(void);

/*
*   @brief Allows us to send over packets over the usb serial interface. 
*   @params const void* buffer that will hold all of our information
*   @params size of buffer information
*/
void os_usb_serial_write(const void *buffer, uint32_t size);

/*
*   @brief Reads a few bytes of information from the teensy into
*   into the desirerd buffer 
*   @params pointer to the void *buffer
*   @params uint32_t size of the buffer that we are passing in
*/
void os_usb_serial_read(void *buffer, uint32_t size);  

/*
*   @brief How many bytes are available from the usb serial interface 
*   @returns uin32_t number of bytes available
*/
uint32_t os_usb_serial_bytes_available(void); 

#endif 