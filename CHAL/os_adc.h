#ifndef OS_ADC_H
#define OS_ADH_H
#include "stdint.h"
#include "stddef.h"
typedef struct os_adc_t{
    int fd;
    int bus;
    int fifosize;
    size_t num_bits;
}os_adc_t;

extern os_adc_t adc_one;
extern os_adc_t adc_two;
extern os_adc_t adc_three;
extern os_adc_t adc_four;
extern os_adc_t adc_five;
extern os_adc_t adc_size;

/**
 * @brief Initializes the ADC module
*/
int os_adc_begin(os_adc_t *adc);

/**
 * @brief Deinitializes the ADC Module
*/
int os_adc_end(os_adc_t *adc);

/**
 * @brief Get's a value from the adc module
*/
int os_adc_read(os_adc_t *adc, void *value);

/**
 * @brief Gathers a sequence of values from the ADC module
*/
int os_adc_read_values(os_adc_t *adc, uint32_t size, void *values);

#endif