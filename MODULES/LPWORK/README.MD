
## Setting up low priority work thread example: 
Sometimes rather than using separate threads to complete tasks, we can complete a group of functions in a single thread at scheduled intervals. This is how you can do that. 

```
// RTOS module
#include "OS/OSThreadKernel.h"

// Low priority work thread module
#include "MODULES/LPWORK/lp_work_thread.h"

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