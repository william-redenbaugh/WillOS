#ifndef _HM18_H
#define _HM18_h

#include <Arduino.h> 
#include "OS/OSMutexKernel.h"
#include "OS/OSSignalKernel.h"

/*!
*   @brief Different supported speeds of the of the hm18 bluetooth module
*/
typedef enum{
    HM18_1200_BAUD      = 0, 
    HM18_2400_BAUD      = 1, 
    HM18_4800_BAUD      = 2, 
    HM18_9600_BAUD      = 3, 
    HM18_19200_BAUD     = 4, 
    HM18_38400_BAUD     = 5, 
    HM18_57600_BAUD     = 6, 
    HM18_115200_BAUD    = 7, 
    HM18_230400_BAUD    = 8
}hm18_serial_speed_t; 


void start_hm18(HardwareSerial *serial_ptr); 

#endif 