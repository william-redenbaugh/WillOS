#include <stdbool.h>
#include <stdlib.h>
#include "stdint.h"

#include "os_uart.h"
#include "CSAL/CSAL_SHARED/os_error.h"

int os_uart_begin(os_uart_t *uart){
    if(uart == NULL)
        return OS_RET_NULL_PTR;   

    return OS_RET_OK;
}

int os_uart_end(os_uart_t *uart){
    if(uart == NULL)
        return OS_RET_NULL_PTR;
}

int os_uart_setbus(os_uart_t *uart, uint32_t freq_baud){
    if(uart == NULL)
        return OS_RET_NULL_PTR;

    uart->baud = freq_baud;
}

int os_uart_transfer(os_uart_t *uart, uint8_t *rx, uint8_t*tx, size_t size){
    if(uart == NULL)
        return OS_RET_NULL_PTR;

    return OS_RET_OK;
}

int os_uart_send(os_uart_t *uart, uint8_t *buf, size_t size){
    if(uart == NULL)
        return OS_RET_NULL_PTR;

    write(uart->fd, buf, size);
}

int os_uart_recieve(os_uart_t *uart, uint8_t *buf, size_t size){
    if(uart == NULL)
        return OS_RET_NULL_PTR;

    read(uart->fd, buf, size);
}