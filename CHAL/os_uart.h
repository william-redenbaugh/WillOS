#ifndef _OS_UART_H
#define _OS_UART_H

#include "stdlib.h"
#include "stdint.h"

typedef struct os_uart_t{
    int n;
}os_uart_t;

/**
 * @brief Begins the UART interface
 * @param os_uart_t *pointer to the UART interface
*/
int os_uart_begin(os_uart_t *uart);

/**
 * @brief Completes the UART Interface
 * @param os_uart_t *pointer to the UART interface
*/
int os_uart_end(os_uart_t *uart);

/**
 * @brief Sets the frequency of the UART interface
 * @param os_uart_t *pointer to the UART interface
 * @param uint32_t frequency/baud rate of UART interfaces
*/
int os_uart_setbus(os_uart_t *uart, uint32_t freq_baud);

/**
 * @brief Transfers data to/from the UART interface
 * @param os_uart_t *pointer to the UART interface
 * @param uint8_t *rx pointer to RX buffer
 * @param uint8_t 
*/
int os_uart_transfer(os_uart_t *uart, uint8_t *rx, uint8_t*tx, size_t size);

/**
 * @brief Sends data to the UART interface
 * @param os_uart_t *pointer to the UART interface
*/
int os_uart_send(os_uart_t *uart, uint8_t *buf, size_t size);

/**
 * @brief Recieves Data from the UART interface
 * @param os_uart_t *pointer to the UART interface
*/
int os_uart_recieve(os_uart_t *uart, uint8_t *buf, size_t size);
#endif