#include "os_error.h"
#include "os_mutx.h"

int os_mut_init(os_mut_t *mut){
    mut->lock = MutexLock();

    return OS_RET_OK;
}

int os_mut_entry(os_mut_t *mut, uint32_t timeout_ms){

    if(timeout_ms == 0){
        if (mut->lock.tryLock() == MUTEX_ACQUIRE_SUCESS){
            return OS_RET_OK;
        }
        else{
            return OS_RET_TIMEOUT;
        }
    }

    if(mut->lock.lock(timeout_ms) == MUTEX_ACQUIRE_SUCESS){
        return OS_RET_OK;
    }
    else{
        return OS_RET_TIMEOUT;
    }
}

int os_mut_count(os_mut_t *mut){
    return mut->lock.getState();
}

int os_mut_entry_wait_indefinite(os_mut_t *mut){
    mut->lock.lockWaitIndefinite();
    return OS_RET_OK;
}

int os_mut_exit(os_mut_t *mut){
    mut->lock.unlock();
    return OS_RET_OK;
}