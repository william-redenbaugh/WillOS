#ifndef _TEENSY_COMS_H
#define _TEENSY_COMS_H

// Include Arduino's primary libraries. 
#include <Arduino.h> 

// Include our RTOS stuff
#include "OS/OSThreadKernel.h"
#include "OS/OSMutexKernel.h"
#include "OS/OSSignalKernel.h"

// Include our RTOS device drivers. 
#include "HAL/OSSerial.h"

// Include our protobuffer messages
#include "messagedata.pb.h"
#include "relay_msg.pb.h"
#include "temp_hum.pb.h"
#include "clock_program.pb.h"

// Used so we know whether or not our callback initialization was accepted. 
enum MessageSubroutineSetupStatus{
    SUBROUTINE_ADD_SUCCESS, 
    SUBROUTINE_ADD_FAIL_MAX_NUM_REACHED, 
    SUBROUTINE_ADD_FAIL_UNKNOWN, 
    SUBROUTINE_REMOVE_SUCCESS, 
    SUBROUTINE_REMOVE_OUT_OF_BOUMDS,
    SUBROUTINE_REMOVE_FAIL_UNKNOWN
};

// Struct that contains information helping us 
// Deal with message subroutine stuff. 
struct MessageSubroutineSetupReturn{
    MessageSubroutineSetupStatus setup_status; 
    uint32_t callback_handler_id; 
};

// Whenever we get a message, 
struct MessageReq{
    // Pointer to our message information
    uint8_t *data_ptr;
    // Pointer to our message length
    int data_len; 
    // If the caller requested that we send something back
    bool return_message; 
};

void message_management_begin(void);

#endif 