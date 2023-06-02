#include "os_adc.h"
#include "CSAL/CSAL_SHARED/os_error.h"
#include "stdlib.h"
#include "wiring_constants.h"
#include "pins_arduino_analog.h"
#include "wiring_analog.h"

os_adc_t adc_one = {
    .bus = 1, 
    .num_bits = 16,
    .fifosize = 128
};
os_adc_t adc_two = {
    .bus = 2, 
    .num_bits = 16,
    .fifosize = 128
};
os_adc_t adc_three = {
    .bus = 3, 
    .num_bits = 16,
    .fifosize = 128
};
os_adc_t adc_four = {
    .bus = 4, 
    .num_bits = 16,
    .fifosize = 128
};
os_adc_t adc_five = {
    .bus = 5, 
    .num_bits = 16,
    .fifosize = 128
};
os_adc_t adc_size = {
    .bus = 6, 
    .num_bits = 16,
    .fifosize = 128
};

int os_adc_begin(os_adc_t *adc){
        
    if(adc == NULL){
        return OS_RET_NULL_PTR;
    }

    adc->fd = A0 + adc->bus - 1;
    pinMode(adc->fd, INPUT);
    analogReadResolution(adc->num_bits);

    return OS_RET_OK;
}

int os_adc_end(os_adc_t *adc){
        
    if(adc == NULL){
        return OS_RET_NULL_PTR;
    }

    return OS_RET_OK;
}

int os_adc_read(os_adc_t *adc, void *value){
    uint16_t *val = value;

    *val = analogRead(adc->fd);
    return OS_RET_OK;
}

int os_adc_read_values(os_adc_t *adc, uint32_t size, void *values){

    if(adc == NULL){
        return OS_RET_NULL_PTR;
    }

    uint16_t *val = values;

    for(int n = 0; n < size; n++){
        val[n] = analogRead(adc->fd);
    }

    return OS_RET_OK;
}