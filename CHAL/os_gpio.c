#include "os_gpio.h"
#include "CSAL/CSAL_SHARED/os_error.h"
#include "pins_arduino_digital.h"
#include "stdlib.h"
#include "wiring_constants.h"
#include "pins_arduino_analog.h"
#include "wiring_analog.h"

int os_gpio_config(os_gpio_conf_t *conf){
    if(conf == NULL){
        return OS_RET_NULL_PTR;
    }
    switch(conf->drive){
        case OS_GPIO_BUSKEEPER:
        case OS_GPIO_FLOAT: 
            if(conf->dir == OS_GPIO_OUTPUT)
                pinMode(conf->gpio_pin, OUTPUT);
            else
                pinMode(conf->gpio_pin, INPUT);
        break;

        case OS_GPIO_PULL_DOWN:
            if(conf->dir == OS_GPIO_OUTPUT)
                pinMode(conf->gpio_pin, OUTPUT);
            else
                pinMode(conf->gpio_pin, INPUT_PULLDOWN);
        break;

        case OS_GPIO_PULL_UP:
            if(conf->dir == OS_GPIO_OUTPUT)
                pinMode(conf->gpio_pin, OUTPUT);
            else
                pinMode(conf->gpio_pin, INPUT_PULLUP);
        break;
    }

    return OS_RET_OK;
}

int os_gpio_get_config(os_gpio_conf_t *conf){

    return OS_RET_OK;
}

int os_gpio_isr_config(os_gpio_isr_conf_t *conf){
    if(conf == NULL){
        return OS_RET_NULL_PTR;
    }

    int mode;

    switch(conf->edge){
        case OS_ISR_HIGH_EDGE:
            attachInterrupt(digitalPinToInterrupt(conf->gpio_pin), conf->isr_fun, HIGH);
        mode = 2;
        break;
        case OS_ISR_LOW_EDGE:
            attachInterrupt(digitalPinToInterrupt(conf->gpio_pin), conf->isr_fun, LOW);
        mode = 3;
        break;
        case OS_ISR_FALLING_EDGE:
            attachInterrupt(digitalPinToInterrupt(conf->gpio_pin), conf->isr_fun, HIGH);
        mode = 5;
        break;
        case OS_ISR_RISING_EDGE:
        mode = 4;
        break;
        case OS_ISR_BOTH_EDGE:
        mode = 7;
        break;
        default:
        break;
    }

    return OS_RET_OK;
}

int os_gpio_get_isr_conf(os_gpio_isr_conf_t *conf){
    if(conf == NULL){
        return OS_RET_NULL_PTR;
    }

    int mode;
    bool filter; 
    bool enabled;

    if(board_gpio_intstatus(conf->gpio_pin, &mode, &filter, &enabled) < 0){
        return OS_RET_INT_ERR;
    }

    switch (mode)
    {
    case 2:
        conf->edge = OS_ISR_HIGH_EDGE;
        break;
    case 3:
        conf->edge = OS_ISR_LOW_EDGE;
        break;
    case 4:
        conf->edge = OS_ISR_RISING_EDGE;
        break;
    case 5: 
        conf->edge = OS_ISR_FALLING_EDGE;
        break;;

    case 7:
        conf->edge = OS_ISR_BOTH_EDGE;
    default:
        break;
    }

    conf->noise_filter = filter;
    return OS_RET_OK;
}

int os_gpio_enable_int(int pin, bool en){
    if(board_gpio_int(pin, en) < 0){
        return OS_RET_INT_ERR;
    }

    return OS_RET_OK;
}

int os_gpio_set(int gpio_pin, os_gpio_set_t set){
    board_gpio_write(gpio_pin, (int)set);
}

int os_gpio_read(int gpio_pin){
    return board_gpio_read(gpio_pin);
}