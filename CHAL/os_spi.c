#include "os_spi.h"
#include "CSAL/CSAL_SHARED/os_error.h"

os_spi_t spi_zero = {
    .fd = 0, 
    .xfer_speed = 1000000, 
    .bus = 0, 
    .spi_mode = 1
};

os_spi_t spi_three= {
    .fd = 0, 
    .xfer_speed = 1000000, 
    .bus = 3,
    .spi_mode = 1
};

os_spi_t spi_four = {
    .fd = 0, 
    .xfer_speed = 1000000, 
    .bus = 4,
    .spi_mode = 1
};

os_spi_t spi_five = {
    .fd = 0, 
    .xfer_speed = 1000000, 
    .bus = 5,
    .spi_mode = 1
};


int os_spi_begin(os_spi_t *spi){
    if(spi == NULL)
        return OS_RET_NULL_PTR;

    if(spi->fd < 0){
        return OS_RET_IO_ERROR;
    }

    return OS_RET_OK;
}

int os_spi_end(os_spi_t *spi){
    if(spi == NULL)
        return OS_RET_NULL_PTR;

    if (spi->fd <= 0){
        return OS_RET_INT_ERR;
    }
    
    close(spi->fd);

    return OS_RET_OK;    
}

int os_spi_setbus(os_spi_t *spi, uint32_t freq_hz){
    if(spi == NULL)
        return OS_RET_NULL_PTR;

    spi->xfer_speed = freq_hz;

    return OS_RET_OK;
}

int os_spi_transfer(os_spi_t *spi, uint8_t *rx, uint8_t *tx, size_t size){
    if(spi == NULL)
        return OS_RET_NULL_PTR;
    
    return OS_RET_OK;

}

int os_spi_send(os_spi_t *spi, uint8_t *buf, size_t size){
    if(spi == NULL)
        return OS_RET_NULL_PTR;

    return OS_RET_OK;
}

int os_spi_recieve(os_spi_t *spi, uint8_t *buf, size_t size){
    if(spi == NULL)
        return OS_RET_NULL_PTR;


    return OS_RET_OK;
}