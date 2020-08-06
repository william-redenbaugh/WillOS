#include <Arduino.h>
#include "will_os.h"

void setup(){

}

void loop(){

}

// TeensyThreads Testing
/*
#include "TeensyThreads.h"

const int LED = 13;

volatile int blinkcode = 0;

void blinkthread() {
  while(1) {    
      digitalWrite(LED, HIGH);
      threads.delay(1000);
      digitalWrite(LED, LOW);
      threads.delay(1000);
  }
}

void setup() {
  delay(1000);
  pinMode(LED, OUTPUT);
  threads.addThread(blinkthread);
}

int count = 0;

void loop() {
  //digitalWrite(LED, HIGH);
  //threads.delay(150);
  //digitalWrite(LED, LOW);
  //threads.delay(150);
  threads.delay(1000);
}

*/