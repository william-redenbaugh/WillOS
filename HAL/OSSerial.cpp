#include "OSSerial.h"

static MutexLock usbSerialMutex; 

/*
*   @brief USB controls through the TeensyOS, initialization
*/
void os_usb_serial_begin(void){
    usbSerialMutex.lockWaitIndefinite(); 
    usb_serial_configure();
    usbSerialMutex.unlock();
}

/*
*   @brief Allows us to send over packets over the usb serial interface. 
*   @params const void* buffer that will hold all of our information
*   @params size of buffer information
*/
void os_usb_serial_write(const void *buffer, uint32_t size){
    usbSerialMutex.lockWaitIndefinite();
    usb_serial_write(buffer, size); 
    usbSerialMutex.unlock();
}

/*
*   @brief Reads a few bytes of information from the teensy into
*   into the desirerd buffer 
*   @params pointer to the void *buffer
*   @params uint32_t size of the buffer that we are passing in
*/
void os_usb_serial_read(void *buffer, uint32_t size){
    usbSerialMutex.lockWaitIndefinite();
    usb_serial_read(buffer, size); 
    usbSerialMutex.unlock();
}

/*
*   @brief How many bytes are available from the usb serial interface 
*   @returns uin32_t number of bytes available
*/
uint32_t os_usb_serial_bytes_available(void){
    return (uint32_t)usb_serial_available();
}