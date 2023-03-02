#include "OSQueueKernel.hpp"

bool VoidOSQueue::init(uint32_t queue_len){
    queue_lock.lockWaitIndefinite();

    this->queue_len = queue_len;
    this->data_buffer = (QueueData*)malloc(sizeof(QueueData) * queue_len);

    queue_lock.unlock();

    if(this->data_buffer == NULL){
        return false;
    }

    return true;
}

bool VoidOSQueue::push(QueueData data){
    this->queue_lock.lockWaitIndefinite();
    // No more space for any more elements in the queue.
    if(this->queue_len == this->current_elements){
        this->queue_lock.unlock();
        return false;
    }
    this->current_elements++;
    this->data_buffer[tail] = data;
    this->tail++;
    if(this->tail == this->queue_len)
        this->tail = 0;

    // If a thread is not in queue
    // Also an atomic operation so stop preemptive switching
    int os_state = os_stop();
    if(this->consumer_thread_ptr != NULL)
        this->consumer_thread_ptr->flags = THREAD_RUNNING;
    os_start(os_state);

    this->queue_lock.unlock();
    return true;
}

QueueData VoidOSQueue::pop(void){
    QueueData new_data;
    memset((void*)&new_data, 0, sizeof(new_data));
    this->queue_lock.lockWaitIndefinite();
    if(this->current_elements == 0){
        this->queue_lock.unlock();
        new_data.data = NULL;
        return new_data;
    }
    new_data.data = this->data_buffer[this->head].data;
    new_data.type = this->data_buffer[this->head].type;
    this->current_elements--;
    this->head++;
    if(this->head == this->queue_len)
        this->head = 0;

    this->queue_lock.unlock();
    return new_data;
}

QueueData VoidOSQueue::popBlocking(void){
    QueueData new_data;

    consumer_lock.lockWaitIndefinite();
    if(this->current_elements == 0){
        thread_t *self_thread = _os_current_thread();
        this->consumer_thread_ptr = self_thread;
        int os_state = os_stop();
        self_thread->flags = THREAD_BLOCKED_QUEUE;
        os_start(os_state);
        // System blocking so yield
        _os_yield();

    }
    this->consumer_thread_ptr = NULL;
    consumer_lock.unlock();
    this->queue_lock.lockWaitIndefinite();

    new_data.data = this->data_buffer[this->head].data;
    new_data.type = this->data_buffer[this->head].type;
    this->current_elements--;
    this->head++;
    if(this->head == this->queue_len)
        this->head = 0;

    this->queue_lock.unlock();
    return new_data;
}
