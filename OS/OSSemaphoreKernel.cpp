#include "OSSemaphoreKernel.h"

#ifdef SEMAPHORE_MODULE

/*!
 *   Author: William Redenbaugh
 *   Last Edite Date: 3/27/2023
 */

/*!
 *   @brief Get's the current entrants / states of the semaphore
 *   @return SemaphoreLockState_t state of the semaphore
 */
uint32_t SemaphoreLock::getState(void)
{
    int os_state = os_stop();
    uint32_t state = this->state;
    os_start(os_state);
    return state;
}

/*!
 * @brief Allows us to acquire our semaphore
 * @param timeout_ms
 * @returns SemaphoreLockReturnStatus or whether or not we were able to get the mutex
 */
SemaphoreRet __attribute__((noinline)) SemaphoreLock::entry(uint32_t timeout_ms)
{
    SemaphoreRet ret = this->tryEntry();

    if (ret.ret_status == SEMAPHORE_ACQUIRE_SUCCESS)
        return ret;

    int state = os_stop();

    thread_t *this_thread = _os_current_thread();
    this_thread->flags = THREAD_BLOCKED_SEMAPHORE_TIMEOUT;
    this_thread->interval = timeout_ms;
    this_thread->previous_millis = millis();

    // Setting up the semaphore current and future counts here
    this_thread->mutex_semaphore = &this->state;
    this_thread->semaphore_max_count = this->max_entry;

    os_start(state);
    _os_yield();

    __flush_cpu_pipeline();

    return this->tryEntry();
}

/*!
 *   @brief Trying to enter our semaphore
 *   @returns SemaphoreLockReturnStatus or whether or not we were able to get the mutex
 */
SemaphoreRet SemaphoreLock::tryEntry(void)
{
    int os_state = os_stop();
    SemaphoreRet ret;

    if (this->state < this->max_entry)
    {
        this->state++;
        os_start(os_state);
        ret.count = this->state;
        ret.ret_status = SEMAPHORE_ACQUIRE_SUCCESS;
        return ret;
    }

    os_start(os_state);
    ret.count = this->state;
    ret.ret_status = SEMAPHORE_ACQUIRE_FAIL;
    return ret;
}

/*!
 * @brief Waits for the semaphore indefinitely
 */
int __attribute__((noinline)) SemaphoreLock::entryWaitIndefinite(void)
{
    SemaphoreRet sem_ret = this->tryEntry();
    if (sem_ret.ret_status == SEMAPHORE_ACQUIRE_SUCCESS)
        return sem_ret.count;

    int state = os_stop();

    // Pointer to the current thread.
    thread_t *this_thread = _os_current_thread();

    // Setting up the semaphore current and future counts here
    this_thread->flags = THREAD_BLOCKED_SEMAPHORE;
    this_thread->mutex_semaphore = &this->state;
    this_thread->semaphore_max_count = this->max_entry;

    os_start(state);
    _os_yield();
    __flush_cpu_pipeline();

    // Increment the semaphore so it's valid.
    sem_ret = this->tryEntry();

    // So the compiler stops giving me errors.
    return sem_ret.count;
}

/*!
 *   @brief Decrements the semaphore counter.
 */
SemaphoreExitReturnStatus __attribute__((noinline)) SemaphoreLock::exit(void)
{
    SemaphoreExitReturnStatus ret = SEMAPHORE_EXIT_SUCCCESS;

    int os_state = os_stop();

    if (this->state == 0)
        ret = SEMAPHORE_EXIT_FAIL;

    this->state--;
    __flush_cpu_pipeline();
    os_start(os_state);

    return ret;
}

#endif