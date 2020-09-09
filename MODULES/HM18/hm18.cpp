#include "hm18.h"

/*
*   To send UART data over bluetooth, you need to connect to the module as a ble module, and write text data in the specified Service and Characteristic
*   BLE Service: 0xFFE0
*   BLE Characteristic: 0xFFE1
*/

/*!
*   @brief The default baudrate of the hm-18 device. 
*/
const int HM_18_DEFAULT_BAUDRATE = 9600; 

/*!
*   @brief The Serial pointer device that we are trying to read from. 
*/
HardwareSerial *serial_device; 

void start_hm18(HardwareSerial *serial_ptr); 
void sleep_hm18(void); 
void wake_hm18(void);
void hm18_disconnect_device(void);
void change_bluetooth_speed(hm18_serial_speed_t serial_speed); 

void start_hm18(HardwareSerial *serial_ptr){   
    serial_device = serial_ptr; 
    serial_device->begin(HM_18_DEFAULT_BAUDRATE);
    
    hm18_disconnect_device();
}

void sleep_hm18(void){
    serial_device->println("AT+SLEEP"); 
}

void wake_hm18(void){
    // Why not you know?
    serial_device->println("wakewakewakewakewakewakewakewakewakewakewakewakewakewakewake");
}

void hm18_disconnect_device(void){
    serial_device->println("AT");
}

void change_bluetooth_speed(hm18_serial_speed_t serial_speed){
    serial_device->print("AT+BAUD"); 
    serial_device->println(int(serial_speed)); 
    
}