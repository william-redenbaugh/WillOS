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
volatile int blinkcode = 0;
MutexLock lock_test; 
os_thread_id_t target_thread; 

void setup() {
  Serial.read();
  // Macro that initializes the primary thread. 
  os_init();
  // Setup low priority thread. 
  setup_lwip_thread();
  // Starts up the OS managed serial interface. 
  os_usb_serial_begin();

  message_management_begin();  
}


void loop() {
    
}