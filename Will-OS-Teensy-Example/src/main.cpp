#include <Arduino.h>
#include "TeensyThreads.h"

const int LED = 13;
volatile int blinkcode = 0;

void blinkthread(void *parameters) {
  os_thread_delay_ms(1);
  while(1) {    
      digitalWrite(LED, HIGH);
      os_thread_delay_ms(100);
      digitalWrite(LED, LOW);
      os_thread_delay_ms(100);
  }
}

os_thread_id_t target_thread; 

void setup() {
  delay(4000);
  threads_init();
  pinMode(LED, OUTPUT);
  target_thread = os_add_thread((thread_func_t)blinkthread, 0, -1, 0);
}

void loop() {
  os_suspend_thread(target_thread);
  os_thread_delay_ms(2000);
  os_resume_thread(target_thread);
  os_thread_delay_ms(2000);
}
