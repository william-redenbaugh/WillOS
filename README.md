# TeensyOS
My Own Operating System. Context switching heavily derived from TeensyThreads ftrias, just gotta give credit where credit is due. 

# Supported Boards: 
#### Teensy 4(Pre-emptive and cooperative switching)
#### STM32F767ZI(Currently only supports cooperative switching)
#### STM32F407VE(Currently only supports cooperative switching)

# TeensyThreads Context Switching
Even though this is a fork from the TeensyThreads library, in reality it's a complete re-write and refractor. Most of the command calls have been moved around to c-style cpp, in reality this is my codebase now execpt for the context switching. 

##Documentation: 

### Creating a new thread example: 
Here is an example where we create a thread, and sleep it every 1/10th of a second. 

```
#include "OS/OSThreadKernel.h"

// Thread handler ID that we can use to manipulate a thread. 
os_thread_id_t target_thread; 

// Function that we are spawning with our thread. 
void example_thread(void *parameters){
  while(1){
    // Delays a thread for 100 milliseconds    
    os_thread_delay_ms(100);
  }
}

void setup(){
  os_init();
  // Instanciate the thread with the given thread ID if we want to instruct it later. 
  target_thread = os_add_thread((thread_func_t)example_thread, 0, -1, 0);
}

void loop(){
  os_thread_delay_ms(100);
}

```

### Mutex example:
Here is an example where we create a thread, and put a mutex over a resource. For sake of simplicity we will put a resource over the serial terminal. 

```
#include "OS/OSThreadKernel.h"
#include "OS/OSMutexKernel.h"

// Thread handler ID that we can use to manipulate a thread. 
os_thread_id_t target_thread; 

// Mutex Lock that we will wrap around the serial interface. 
MutexLock serial_mutex; 

// Function that we are spawning with our thread. 
void example_thread(void *parameters){
  while(1){
    // Delays a thread for 100 milliseconds    
    os_thread_delay_s(1);
    
    // Places a lock around the serial interface, and then releases that lock
    serial_mutex.lockWaitIndefinite();
    Serial.println("Hello from thread 1!");
    serial_mutex.unlock();
  }
}

void setup(){
  // We setup the serial object before we startup the OS
  // For that reason we aren't wrapping it around with the mutex. 
  Serial.begin(115200);
  
  // We instanciate the resource. 
  os_init();
  // Instanciate the thread with the given thread ID if we want to instruct it later. 
  target_thread = os_add_thread((thread_func_t)example_thread, 0, -1, 0);
}

void loop(){
  // Just so we aren't spamming the system, we delay by 2 seconds
  os_thread_delay_s(2);
  
  // Places a lock around the serial interface, and then releases that lock
  serial_mutex.lockWaitIndefinite(); 
  Serial.println("Hello from thread 0!");
  serial_mutex.unlock();
}

```

## Set/Clear bits using thread_control. 
There are control bits that we can use to signal a thread. 32 per thread in fact. Here is an example where we use those bits: 
```

#include "OS/OSThreadKernel.h"
#include "OS/OSMutexKernel.h"

// Thread handler ID that we can use to manipulate a thread. 
os_thread_id_t target_thread; 

// Mutex Lock that we will wrap around the serial interface. 
MutexLock serial_mutex; 

// Function that we are spawning with our thread. 
void example_thread(void *parameters){
  while(1){
    // Wait for bits from the internal thread signals. 
    os_thread_waitbits_notimeout(THREAD_SIGNAL_0);
    os_thread_clear(THREAD_SIGNAL_0);
    
    // Places a lock around the serial interface, and then releases that lock
    serial_mutex.lockWaitIndefinite();
    Serial.println("Hello from thread 1!");
    serial_mutex.unlock();
  }
}

void setup(){
  // We setup the serial object before we startup the OS
  // For that reason we aren't wrapping it around with the mutex. 
  Serial.begin(115200);
  
  // We instanciate the resource. 
  os_init();
  // Instanciate the thread with the given thread ID if we want to instruct it later. 
  target_thread = os_add_thread((thread_func_t)example_thread, 0, -1, 0);
}

void loop(){
  // Just so we aren't spamming the system, we delay by 2 seconds
  os_thread_delay_s(2);
  
  // Set the thread bits of our target thread ID.
  os_signal_thread(THREAD_SIGNAL_0, target_thread); 
}

```


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


## Writing the the USB Serial in a threadsafe way
Prefered whenever you are using serial device across a few threads. 
```

#include "OS/OSThreadKernel.h"
#include "HAL/OSSerial.h"

void setup(){
  os_init(); 
   
  // Starts up the serial peripheral. 
  os_usb_serial_begin(); 
  
  uint8_t buffer[16]; 
  // Read in data from serial bus into buffer
  os_usb_serial_read(buffer, sizeof(buffer)); 
  
  // Write data that we just took in back into the buffer
  os_usb_write(buffer, sizeof(buffer); 
    
  // Checking how many bytes we have available. 
  uint32_t current_bytes_in_array = os_usb_serial_bytes_available(); 
}

void loop(){
  os_thread_delay_ms(100); 
}

```
