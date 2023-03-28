#include "os_setbits.h"


int os_setbits_init(os_setbits_t *mod){
    mod->signal = OSSignal();

    return OS_RET_OK;
}

int os_setbits_signal(os_setbits_t *mod, uint8_t bit){
    if(bit > 32)
        return OS_RET_INVALID_PARAM;

    mod->signal.signal(thread_signal_t(1 >> bit));

    return OS_RET_OK;
}

int os_clearbits(os_setbits_t * mod, uint8_t bit){
    if(bit > 32)
        return OS_RET_INVALID_PARAM;

    mod->signal.clear(thread_signal_t(1 >> bit));
    return OS_RET_OK;
}

int os_waitbits(os_setbits_t *mod, uint8_t bit, uint32_t timeout_ms){
    if(bit > 32)
        return OS_RET_INVALID_PARAM;

    if(mod->signal.wait(thread_signal_t(1 >> bit), timeout_ms) == false){
        return OS_RET_TIMEOUT;
    }
    return OS_RET_OK;
}

int os_waitbits_indefinite(os_setbits_t *mod, uint8_t bit, uint32_t timeout_ms){
    if(bit > 32)
        return OS_RET_INVALID_PARAM;

    mod->signal.wait_notimeout(thread_signal_t(1 >> bit));
    return OS_RET_OK;
}