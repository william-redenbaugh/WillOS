#include "CSAL_SHARED/os_error.h"
#include "os_mutx.h"

int os_mut_init(os_mut_t *mut){
    if(mut == NULL){
        return OS_RET_NULL_PTR;
    }
    
    mut->lock = MutexLock();

    return OS_RET_OK;
}

int os_mut_try_entry(os_mut_t *mut){
    if(mut == NULL){
        return OS_RET_NULL_PTR;
    }

    if(mut->lock.tryLock() == MUTEX_ACQUIRE_SUCESS){
        return OS_RET_OK;
    }
    else{
        return OS_RET_DEADLOCK;
    }
}

int os_mut_deinit(os_mut_t *mut){
    if(mut == NULL){
        return OS_RET_NULL_PTR;
    }
    // STUBBY
    return OS_RET_OK;
}

int os_mut_entry(os_mut_t *mut, uint32_t timeout_ms){

    if(mut == NULL){
        return OS_RET_NULL_PTR;
    }

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
    if(mut == NULL){
        return OS_RET_NULL_PTR;
    }

    return mut->lock.getState();
}

int os_mut_entry_wait_indefinite(os_mut_t *mut){
    if(mut == NULL){
        return OS_RET_NULL_PTR;
    }

    mut->lock.lockWaitIndefinite();
    return OS_RET_OK;
}

int os_mut_exit(os_mut_t *mut){
    if(mut == NULL){
        return OS_RET_NULL_PTR;
    }

    mut->lock.unlock();
    return OS_RET_OK;
}