#ifndef _OS_SPI_H
#define _OS_SPI_H

#include "stdlib.h"
#include "stdint.h"

typedef struct os_spi_t{
    int fd;
    int xfer_speed;
    int bus;
    int spi_mode;
}os_spi_t;

/**
 * @brief There are four SPI busses connected to the Spressense
*/
extern os_spi_t spi_zero;
extern os_spi_t spi_three;
extern os_spi_t spi_four;
extern os_spi_t spi_five;

/**
 * @brief Begins the SPI interface
 * @param os_spi_t *pointer to the SPI interface
*/
int os_spi_begin(os_spi_t *spi);

/**
 * @brief Stops the SPI interface
 * @param os_spi_t *pointer to the SPI interface
*/
int os_spi_end(os_spi_t *spi);

/**
 * @brief Sets the SPI interface Speed
 * @param os_spi_t *pointer to the SPI interface
 * @param uint32_t freq_hz speed of interface
*/
int os_spi_setbus(os_spi_t *spi, uint32_t freq_hz);

/**
 * @brief Transfers data to/from the SPI interface
 * @param os_spi_t *pointer to the SPI interface
 * @param uint8_t* pointer to recieving buffer
 * @param uint8_t* pointer to sending buffer
 * @param size_t size of buffer transfering
*/
int os_spi_transfer(os_spi_t *spi, uint8_t *rx, uint8_t *tx, size_t size);

/**
 * @brief Sends data to the SPI interface
 * @param os_spi_t *pointer to the SPI interface
 * @param uint8_t *buf pointer to sending buffer
 * @param size_t size of buffer sending data out
*/
int os_spi_send(os_spi_t *spi, uint8_t *buf, size_t size);

/**
 * @brief Recieves Data from the SPI interface
 * @param os_spi_t *pointer to the SPI interface
 * @param uint8_t *buf pointer to recieving buffer
 * @param size_t size of buffer reciving data in
*/
int os_spi_recieve(os_spi_t *spi, uint8_t *buf, size_t size);
#endif