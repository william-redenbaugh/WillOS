#ifndef _APA102_HPP
#define _APA102_HPP

// So we can configure modules
#include "../../enabled_modules.h"

#ifdef APA102_MODULE

#include <Arduino.h> 
#include "SPI.h"

#include "OS/OSThreadKernel.h"
#include "OS/OSMutexKernel.h"

class OSStripSPI{
    public: 
        void begin(uint32_t num_pixels); 
        void begin(uint32_t num_pixels, SPIClass *spi);
        void set_pixel(uint32_t pixel, uint8_t r, uint8_t g, uint8_t b); 
        void set_global_brightness(uint8_t brightness); 
        void update(void); 

    private: 
    // Array PTR to the drawing memory for our display strips. 
    uint8_t *drawing_mem; 

    // How many pixels do we have?
    uint32_t num_pixels = 0; 

    // How large is our memory buffer. 
    size_t mem_buff_size = 0;

    // Global brightness settings. 
    uint32_t brightness = 31; 

    // To keep things threadsafe(on the offchance that we call LED strip changes from multiple threads)
    MutexLock strip_lock; 

    // Pointer to the SPI device 
    SPIClass *spi_device; 
};

#endif
#endif 