#include "os_sem.h"
#include "os_error.h"

int os_sem_init(os_sem_t *sem, int count){
    if(sem == NULL){
        return OS_RET_NULL_PTR;
    }

    sem->lock = SemaphoreLock(count);

    return OS_RET_OK;
}

int os_sem_entry(os_sem_t *sem, uint32_t timeout_ms){
    if(sem == NULL){
        return OS_RET_NULL_PTR;
    }
    SemaphoreRet ret = sem->lock.entry(timeout_ms);
    if(ret.ret_status == SEMAPHORE_ACQUIRE_FAIL)
        return OS_RET_TIMEOUT;
        
    return sem->lock.tryEntry().count;
}

int os_sem_entry_wait_indefinite(os_sem_t *sem){
    if(sem == NULL){
        return OS_RET_NULL_PTR;
    }
    
    return sem->lock.entryWaitIndefinite();
}

int os_sem_entry_try(os_sem_t *sem){
    if(sem == NULL){
        return OS_RET_NULL_PTR;
    }
    
    SemaphoreRet ret = sem->lock.tryEntry();
    if(ret.ret_status == SEMAPHORE_ACQUIRE_FAIL)
        return OS_RET_TIMEOUT;

    return sem->lock.tryEntry().count;
}

int os_sem_count(os_sem_t *sem){
    if(sem == NULL){
        return OS_RET_NULL_PTR;
    }
    
    sem->lock.getState();
}
