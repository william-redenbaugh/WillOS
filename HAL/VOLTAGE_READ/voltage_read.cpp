#include "voltage_read.h"

/*!
*   @brief Setup function that get's Voltage reading properly. 
*   @param uint8_t gpio(pin that we are using on the teensy)
*   @param uint8_t divider(or how much we are cutting down the voltage compared to the input analog voltage. )
*/
void VoltageRead::init(uint8_t gpio, uint8_t divider){
    // Saving the information
    this->gpio = gpio; 
    this->divider = divider;
    // Setting up the gpio pin to what I want.  
    pinMode(gpio, INPUT); 
    analogReadResolution(16); 
}

/*!
*   @brief Just getting the voltage of whatever we are working on 
*   @return Voltage(as a floating point decimal)
*/
float VoltageRead::getVoltage(void){
    return float(analogRead(this->gpio))/float(this->divider); 
}

/*!
*   @brief Battery configuration so we can get a rough percentage on the battery
*   @param float minimum bounds of the battery
*   @param float maximum bounds of the battery. 
*   @return bool success of battery configuration
*/
bool VoltageRead::configureBattery(float min_bounds, float max_bounds){
    this->min_bounds = min_bounds; 
    this->max_bounds = max_bounds; 

    this->delta = max_bounds - min_bounds; 

    if(min_bounds <= max_bounds)
        return true; 
    
    return false; 
}

/*!
*   @brief Battery percentage. 
*   @return Current battery percentage
*/
float VoltageRead::batteryPercentage(void){
    float current_voltage = this->getVoltage(); 

    // Should the data be an outlier, we return these. 
    if(current_voltage <  this->min_bounds)
        return 0.00; 
    if(current_voltage > this->max_bounds)
        return 1.00; 
    
    // Should return a percentage. 
    return (current_voltage - this->min_bounds) / this->delta; 
}