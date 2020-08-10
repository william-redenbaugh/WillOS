#include <Arduino.h>
#include "OSThreadKernel.h"
#include "OSMutexKernel.h"
#include "lp_work_thread.h"

const int LED = 13;
volatile int blinkcode = 0;
MutexLock lock_test; 
os_thread_id_t target_thread; 

void blinkthread(void *parameters) {
  while(1) {    
      os_thread_waitbits_notimeout(THREAD_SIGNAL_0);
      os_thread_clear(THREAD_SIGNAL_0);

      //lock_test.lockWaitIndefinite();
      digitalWrite(LED, HIGH);
      os_thread_delay_ms(100);

      //lock_test.unlock();
      digitalWrite(LED, LOW);
      os_thread_delay_ms(100);
  }
}

void printhello(void){
  Serial.println("Hello");
}

void setup() {
  delay(4000);
  Serial.begin(115200);
  
  // Macro that initializes the primary thread. 
  os_init();

  pinMode(LED, OUTPUT);
  target_thread = os_add_thread((thread_func_t)blinkthread, 0, -1, 0);
}

void loop() {
  os_thread_delay_ms(2000);
  //lock_test.lockWaitIndefinite();
  os_signal_thread(THREAD_SIGNAL_0, target_thread);
  os_thread_delay_ms(2000);
  //lock_test.unlock();
}