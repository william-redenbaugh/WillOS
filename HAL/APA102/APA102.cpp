#include "APA102.hpp"

#ifdef APA102_MODULE

/*!
*   @brief Default strip settings for SPI bus. 
*/
static SPISettings strip_default_settings(4000000, MSBFIRST, SPI_MODE1);

/*!
*   @brief Sets up our microcontroller to interface with our RGB LEDs
*   @param uint32_t num_pixels(how many pixels are we using?)
*/
void OSStripSPI::begin(uint32_t num_pixels){
    // Setting up our SPI device settings. 
    SPI.begin(); 

    // Setting up our transaction information
    SPI.beginTransaction(strip_default_settings);

    // Allocating drawing memory for our LED strips. 
    size_t total_memcount = (10 + 3 * num_pixels) * sizeof(uint8_t);
    this->drawing_mem = (uint8_t*)malloc(total_memcount); 
    // There's always garbage in this array during startup
    memset(this->drawing_mem, 0, total_memcount); 

    // Setting our ending bits so that we can complete the strip transaction
    for(int n = 0; n < 4; n++)
        this->drawing_mem[num_pixels * 4 + 4 + n] = 255; 
    
    // Set max brightness by default 
    this->set_global_brightness(31);

    // Since we didn't designnate a default SPI device, we choose this one instead. 
    this->spi_device = &SPI; 
}

/*!
*   @brief Sets up our microcontroller to interface with our RGB LEDs
*   @param uint32_t num_pixels(how many pixels are we using?)
*   @param SPIClass *spi pointer object that let's program know which spi target we are using 
*/
void OSStripSPI::begin(uint32_t num_pixels, SPIClass *spi){
    // Setting up our SPI device settings. 
    SPI.begin(); 

    // Setting up our transaction information
    SPI.beginTransaction(strip_default_settings);

    // Allocating drawing memory for our LED strips. 
    size_t total_memcount = (8 + 3 * num_pixels) * sizeof(uint8_t);
    this->mem_buff_size = total_memcount; 
    this->drawing_mem = (uint8_t*)malloc(total_memcount); 
    // There's always garbage in this array during startup
    memset(this->drawing_mem, 0, total_memcount); 

    // Setting our ending bits so that we can complete the strip transaction
    for(int n = 0; n < 4; n++)
        this->drawing_mem[num_pixels * 4 + 4 + n] = 255; 
    
    // Set max brightness by default 
    this->set_global_brightness(31);

    // Since we didn't designnate a default SPI device, we choose this one instead. 
    this->spi_device = spi;  
}

/*!
*   @brief Allows us to write an rgb value to a pixel. 
*   @param uint32_t pixel(which pixel are we targeting?)
*   @param uint8_t r(red)
*   @param uint8_t g(green)
*   @param uint8_t b(blue)
*/
void OSStripSPI::set_pixel(uint32_t pixel, uint8_t r, uint8_t g, uint8_t b){
    this->drawing_mem[4 + 4 * pixel] = b;
    this->drawing_mem[5 + 4 * pixel] = g;
    this->drawing_mem[6 + 4 * pixel] = r;
}

/*!
*   @brief  Allows us to set the global brightness value of all of our pixels
*   @param uint8_t brightness(up to 32)
*/
void OSStripSPI::set_global_brightness(uint8_t brightness){
    if(brightness > 31)
        brightness = 31; 
    // Setting up our global configuration settings
    register uint8_t global_settings_register = 0b11100000 | brightness;
    for(size_t i = 0; i < num_pixels; i++){
        // Since we need to configure the global brightness settings and setup our configuration bits, may as 
        // Well do this here/now
        this->drawing_mem[i * 4 + 4] = global_settings_register;  
    }
}

/*!
*   @brief Sends the LED buffer over the SPI interface VIA DMA
*   @note Also incorporates OS system calls so that we aren't overloading the SPI interface. 
*/
void OSStripSPI::update(void){
    EventResponder event_responder; 
    this->spi_device->transfer(drawing_mem, NULL, this->mem_buff_size, event_responder); 
    os_thread_delay_ms(2); 
}

#endif