#ifndef _OS_I2C_H
#define _OS_I2C_H

#include "stdlib.h"
#include  "stdint.h"

typedef struct os_i2c_t{
    int fd;
    int speed;
}os_i2c_t;

extern os_i2c_t i2c_one;

/**
 * @brief Begins the I2C Interface
 * @param os_i2c_t *pointer to the I2C interface
*/
int os_i2c_begin(os_i2c_t *i2c);

/**
 * @brief Ends the I2C interface
 * @param os_i2c_t *pointer to the I2C interface
*/
int os_i2c_end(os_i2c_t *i2c);

/**
 * @brief Sets the interface speed of the I2C Interface
 * @param os_i2c_t *pointer to the I2C interface
 * @param uint32_t frequency to the I2C interface in Hertz
*/
int os_i2c_setbus(os_i2c_t *i2c, uint32_t freq_hz);

/**
 * @brief Sends out data through the I2C interface
 * @param os_i2c_t *pointer to the I2C interface
 * @param uint8_t addrs of device on I2C interface
 * @param uint8_t *buf pointer to buffer sending on I2C interface
 * @param size_t size of buffer
*/
int os_i2c_send(os_i2c_t *i2c, uint8_t addr, uint8_t *buf, size_t size);

/**
 * @brief Recieves data from the I2C Interface
 * @param os_i2c_t *pointer to the I2C interface
 * @param uint8_t addrs of device on I2C interface
 * @param uint8_t *buf pointer to buffer reciving on I2C interface
 * @param size_t size of buffer
*/
int os_i2c_recieve(os_i2c_t *i2c, uint8_t addr, uint8_t *buf, size_t size);
#endif