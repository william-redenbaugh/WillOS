#include <Arduino.h>
#include "OSThreadKernel.h"
#include "OSMutexKernel.h"

const int LED = 13;
volatile int blinkcode = 0;
MutexLock lock_test; 
os_thread_id_t target_thread; 

void blinkthread(void *parameters) {
  while(1) {    
      lock_test.lockWaitIndefinite();
      digitalWrite(LED, HIGH);
      os_thread_delay_ms(100);

      lock_test.unlock();
      digitalWrite(LED, LOW);
      os_thread_delay_ms(100);
  }
}

void setup() {
  delay(4000);

  // Macro that initializes the primary thread. 
  os_init();
  pinMode(LED, OUTPUT);
  target_thread = os_add_thread((thread_func_t)blinkthread, 0, -1, 0);
}

void loop() {
  os_thread_delay_ms(2000);
  lock_test.lockWaitIndefinite();
  os_thread_delay_ms(2000);
  lock_test.unlock();
}
