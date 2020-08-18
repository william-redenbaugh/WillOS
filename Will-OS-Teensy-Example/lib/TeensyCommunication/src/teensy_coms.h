#ifndef _TEENSY_COMS_H
#define _TEENSY_COMS_H

// Include Arduino's primary libraries. 
#include <Arduino.h> 

// Include our RTOS stuff
#include "OS/OSThreadKernel.h"
#include "OS/OSMutexKernel.h"
#include "OS/OSSignalKernel.h"

// Include our protobuffer messages
#include "messagedata.pb.h"
#include "relay_msg.pb.h"
#include "temp_hum.pb.h"
#include "clock_program.pb.h"

/*
*   @brief
*   @notes
*/
void message_management_begin(void);

/*
*   @brief
*   @notes
*/
void message_management_run(void);

#endif 