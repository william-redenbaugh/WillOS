# TeensyOS
My Own Operating System. Context switching heavily derived from TeensyThreads ftrias

Currently most of the code sits in the subrepo TeensyThreads contained in the lib folder, go there for most of the core threading functionality. However there are a few modules sitting here as well. 

## Setting up low priority work thread example: 

```
// RTOS module
#include "OSThreadKernel.h"

// Low priority work thread module
#include "lp_work_thread.h"

// Periodic function that we want to run periodically. 
void printhello(void){
  Serial.println("Hello");
}


void setup() {
  Serial.begin(115200);
  
  // Macro that initializes the primary thread and the operating system 
  os_init();
  
  // Setting up the low priority thread module. 
  setup_lwip_thread();
  
  // Adding a low priorty thread module.
  add_lwip_task(&printhello, NULL, 1000);
}

void loop() {
  os_thread_delay_ms(100);
}

```
