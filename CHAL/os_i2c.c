#include "os_i2c.h"
#include "CSAL/CSAL_SHARED/os_error.h"

int os_i2c_begin(os_i2c_t *i2c){
    if(i2c == NULL)
        return OS_RET_NULL_PTR;

   
    return OS_RET_OK;

}

int os_i2c_end(os_i2c_t *i2c){
    if(i2c == NULL)
        return OS_RET_NULL_PTR;

    if (i2c->fd <= 0){
        return OS_RET_INT_ERR;
    }
    
    return OS_RET_OK;
}

int os_i2c_setbus(os_i2c_t *i2c, uint32_t freq_hz){
    if(i2c == NULL)
        return OS_RET_NULL_PTR;

    i2c->speed = freq_hz;
    return OS_RET_OK;
}

int os_i2c_send(os_i2c_t *i2c, uint8_t addr, uint8_t *buf, size_t size){
   ;

    return OS_RET_OK;
}

int os_i2c_recieve(os_i2c_t *i2c, uint8_t addr, uint8_t *buf, size_t size){
    if(i2c == NULL)
        return OS_RET_NULL_PTR;

    return OS_RET_OK;
}