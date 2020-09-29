#ifndef _TEENSY_COMS_H
#define _TEENSY_COMS_H

// So we can configure modules
#include "../../enabled_modules.h"

#ifdef PROTOCALLBACKS_MODULE

// Include Arduino's primary libraries. 
#include <Arduino.h> 

// Include our RTOS stuff
#include "OS/OSThreadKernel.h"
#include "OS/OSMutexKernel.h"
#include "OS/OSSignalKernel.h"

// Include our protobuffer messages
#include "messagedata.pb.h"
#include "nanopb/pb_decode.h"

// Used so we know whether or not our callback initialization was accepted. 
enum MessageCallbackSetupStatus{
    CALLBACK_ADD_SUCCESS, 
    CALLBACK_ADD_FAIL_MAX_NUM_REACHED, 
    CALLBACK_ADD_FAIL_UNKNOWN, 
    CALLBACK_REMOVE_SUCCESS, 
    CALLBACK_REMOVE_OUT_OF_BOUMDS,
    CALLBACK_REMOVE_FAIL_UNKNOWN
};

// Struct that contains information helping us 
// Deal with message subroutine stuff. 
struct MessageCallbackSetupReturn{
    MessageCallbackSetupStatus setup_status; 
    uint32_t callback_handler_id; 
};

// Whenever we get a message. we process this. 
// Note only use the data found in here(including the data pointers)
// When the callback is in scope. Otherwise you might be reading the wrong data. 
struct MessageReq{
    // Pointer to our message information
    uint8_t *data_ptr;
    // Pointer to our message length
    int data_len; 
    // If the caller requested that we send something back
    bool return_message; 
};

/* 
* @brief Easy method for dealing with new messages coming into the system 
* @notes Just makes callbacks easy to deal with, you still need to deal with the deserialization
* And unpacking yourself. 
* params MessageData_MessageType which type of message data are we sending?
* params void(*func)(MessageReq *ptr) callback for dealing with subroutines. 
* returns MessageSubroutineSetupStatus whether or not we actually go the information we needed and the handler ID
*/
extern MessageCallbackSetupReturn add_message_callback(MessageData_MessageType msg_type, void(*func)(MessageReq *ptr));

/*
*   @brief Removes a message callback, so we aren't getting callbacks from that anymore 
*   @params uint32_t callback_handler_id(which thread are we trying to remove)
*   @returns Whether or not we were able to remove the callback and why. 
*/
extern MessageCallbackSetupStatus remove_message_callback(uint32_t callback_handler_id); 

/*!
*   @brief  Starts up all of the message management stuff so we can get messages!
*   @param HardwareSerial *serial_ptr (which serial device we are connecting to)
*   @param uint32_t baud(speed of serial device)
*   @note  Just call this, and then attach whatever event driven messaging stuff you feel you need to do 
*/
void message_callbacks_begin(HardwareSerial *serial_ptr, uint32_t baud);

/*
*   @brief Kills the message management thread. 
*/
void message_callbacks_end(void);

#endif 
#endif