#ifndef _VOLTAGE_READ_H
#define _VOLTAGE_READ_H

// So we can configure modules
#include "../../enabled_modules.h"

#ifdef VOLTAGE_READ_MODULE

// Primary Arduino Module 
#include <Arduino.h>

/*!
*   @brief Helper module that lets us read data from battery easily and intuitively. s
*/
class VoltageRead{
    public: 
        void init(uint8_t gpio, uint8_t divider); 
        bool configureBattery(float min_bounds, float max_bounds); 
        float getVoltage(void); 
        float batteryPercentage(void); 

    private: 
        uint8_t divider = 1; 
        uint8_t gpio;

        float min_bounds; 
        float max_bounds; 
        float delta; 
}; 

#endif
#endif 