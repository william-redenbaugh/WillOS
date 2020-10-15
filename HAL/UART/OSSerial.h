#ifndef _OSSERIAL_H
#define _OSSERIAL_H

#include "../../enabled_modules.h"
#ifdef SERIAL_MODULE

#include <Arduino.h> 
#include "OS/OSMutexKernel.h"
#include "OS/OSSignalKernel.h" 
#include "OS/OSThreadKernel.h" 

class OSSerial{
    public: 
        void setup(HardwareSerial *serial_ptr); 

    private: 
        HardwareSerial *serial_ptr; 
}; 

#endif
#endif 