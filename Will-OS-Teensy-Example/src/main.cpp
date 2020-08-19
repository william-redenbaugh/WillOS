#include <Arduino.h>

// RTOS stuff
#include "OS/OSThreadKernel.h"
#include "OS/OSMutexKernel.h"

// RTOS based Device managers 
#include "HAL/OSSerial.h"

// Low priority work thread stuff
#include "lp_work_thread.h"

// Message management stuff
#include "teensy_coms.h"

const int LED = 13;
bool en = true; 

void msg_callback(MessageReq *msg){
  if(en){
    en = false; 
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else{
    en = true; 
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void setup() {
  // Macro that initializes the primary thread. 
  os_init();
  // Setup low priority thread. 
  setup_lwip_thread();
  // Starts up the OS managed serial interface. 
  os_usb_serial_begin();
  
  pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN, HIGH);

  message_management_begin();  
  add_message_callback(MessageData_MessageType_STATUS_DATA, msg_callback);
}

void loop() {
  os_thread_delay_s(1);   
}