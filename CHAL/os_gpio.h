#ifndef _OS_GPIO_H
#define _OS_GPIO_H
#include "stdlib.h"
#include "stdbool.h"

/**
 * @brief How we are driving our gpio
*/
typedef enum os_gpio_drive_t{
    OS_GPIO_FLOAT = 0, 
    OS_GPIO_PULL_UP,
    OS_GPIO_PULL_DOWN,  
    OS_GPIO_BUSKEEPER
}os_gpio_drive_t;

/**
 * @brief Whether or not we are inputing voltage or outputting voltage to the GPIO
*/
typedef enum os_gpio_direction_t{
    OS_GPIO_INPUT, 
    OS_GPIO_OUTPUT
}os_gpio_direction_t;

/**
 * @brief Either applying to voltage or ground to GPIO
*/
typedef enum os_gpio_set_t{
    OS_GPIO_LOW = 0, 
    OS_GPIO_HIGH
}os_gpio_set_t;

/**
 * @brief What triggers an interrupt on the GPIO
*/
typedef enum os_gpio_isr_edge_t{
    OS_ISR_HIGH_EDGE, 
    OS_ISR_LOW_EDGE,  
    OS_ISR_RISING_EDGE,
    OS_ISR_FALLING_EDGE,
    OS_ISR_BOTH_EDGE
}os_gpio_isr_edge_t;

/**
 * @brief GPIO drive strength
*/
typedef enum os_gpio_drive_strength_t{
    OS_GPIO_NORMAL_STRENGTH = 0, 
    OS_GPIO_HIGH_STRENGTH = 1
}os_gpio_drive_strength_t;

/**
 * @brief Configuration struct that defines how you will use the GPIO pin
*/
typedef struct os_gpio_conf_t{
    os_gpio_drive_t drive;
    os_gpio_direction_t dir;
    os_gpio_drive_strength_t strength;
    int gpio_pin;
}os_gpio_conf_t;

/**
 * @brief The ISR function that runs during GPIO trigger
*/
typedef void(*isr_func_t)(void*);

/**
 * @brief Configuration module for attaching an interrupt to a GPIO
*/
typedef struct os_gpio_isr_conf_t{
    int gpio_pin;
    bool noise_filter;
    os_gpio_isr_edge_t edge;
    isr_func_t isr_fun;
}os_gpio_isr_conf_t;

/**
 * @brief  Configures the GPIO module
 * @param os_gpio_conf_t *configuration for the gpio
*/
int os_gpio_config(os_gpio_conf_t *conf);

/**
 * @brief Get's the current GPIO module status
*/
int os_gpio_get_config(os_gpio_conf_t *conf);

/**
 * @brief Attaches an interrupt to the GPIO module
 * @param os_gpio_isr_conf_t
*/
int os_gpio_isr_config(os_gpio_isr_conf_t *conf);

/**
 * @brief Get's the ISR config for the GPIO module
 * @param os_gpio_isr_conf_t
*/
int os_gpio_get_isr_conf(os_gpio_isr_conf_t *conf);

/**
 * @brief Enables interrupts for a specific GPIO pin
*/
int os_gpio_enable_int(int pin, bool en);

/**
 * @brief Sets a GPIO pin HIGH or low
 * @param int gpio_pin
 * @param os_gpio_set_t what we are setting the gpio to
*/
int os_gpio_set(int gpio_pin, os_gpio_set_t set);

/**
 * @brief Reads the current value of the gpio pin
 * @param int gpio_pin
*/
int os_gpio_read(int gpio_pin);
#endif