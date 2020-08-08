#include <Arduino.h>
#include "TeensyThreads.h"

const int LED = 13;
volatile int blinkcode = 0;

void blinkthread(void *parameters) {
  while(1) {    
      digitalWrite(LED, HIGH);
      os_thread_delay_ms(2000);
  }
}

void setup() {
  delay(4000);
  threads_init();
  pinMode(LED, OUTPUT);
  os_add_thread((thread_func_t)blinkthread, 0, -1, 0);
  threads.delay(1000);
}

void loop() {
  os_thread_delay_ms(2000);
  digitalWrite(LED, LOW);
}
