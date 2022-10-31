#include "OSQueueKernel.hpp"

bool VoidOSQueue::init(uint32_t queue_len){
    queue_lock.lockWaitIndefinite();

    this->queue_len = queue_len;
    this->data_buffer = (QueueData*)malloc(sizeof(QueueData) * queue_len);

    if(this->data_buffer == NULL){
        return false;
    }

    queue_lock.unlock();

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
