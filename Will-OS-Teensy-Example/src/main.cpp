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

void blinkthread(void *parameters) {
  
  const char helloworld[] = "hello world from thread two!\n";
  while(1) {    

    os_usb_serial_write(helloworld, sizeof(helloworld)-1);
    os_thread_delay_s(1);
  }
}

void setup() {
  Serial.read();
  // Macro that initializes the primary thread. 
  os_init();
  setup_lwip_thread();
  
  // Starts up the OS managed serial interface. 
  os_usb_serial_begin();

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  target_thread = os_add_thread((thread_func_t)blinkthread, 0, -1, 0);
}

const char helloworld_one[] = "hello world from thread one!\n"; 
void loop() {
  os_usb_serial_write(helloworld_one, sizeof(helloworld_one)-1);
  os_thread_delay_s(2);
}