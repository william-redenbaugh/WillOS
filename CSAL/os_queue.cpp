#include "os_queue.h"
#include "os_error.h"

inline void push_element(os_queue_t *queue, void *data){
    queue->data[queue->head] = data;
    queue->head++;
    queue->current_num_elements++;
}

inline void* pop_element(os_queue_t *queue){
    void *data = queue->data[queue->tail] = data;
    queue->tail++;
    queue->current_num_elements--;
}

int os_queue_init(os_queue_t *queue, int max_elements){
    if(queue == NULL){
        return OS_RET_INVALID_PARAM;
    }

    queue->data = (void**)malloc(sizeof(void*) * max_elements);

    // We weren't able to malloc enough memory for our queue
    if(queue->data == NULL){
        return OS_RET_LOW_MEM_ERROR;
    }
    queue->max_num_elemnts = max_elements;
    queue->current_num_elements = 0;
    queue->head = 0;
    queue->tail = 0;

    int mut_ret = os_mut_init(&queue->queue_lock);

    return mut_ret;
}

int os_queue_push_timeout(os_queue_t *queue, void *data, int timeout_ms){
    int ret = OS_RET_OK;
    os_mut_entry(&queue->queue_lock, timeout_ms);
    if(queue->current_num_elements == queue->max_num_elemnts){
        ret = OS_RET_LIST_EMPTY;
        goto queue_end;
    }
    
    push_element(queue, data);

    queue_end: 
    os_mut_exit(&queue->queue_lock);
    return ret;
}

int os_queue_push_indefinite(os_queue_t *queue, void *data, int timeout_ms){
    int ret = OS_RET_OK;

    os_mut_entry_wait_indefinite(&queue->queue_lock);
    
    if(queue->current_num_elements == queue->max_num_elemnts){
        ret = OS_RET_LIST_EMPTY;
        goto queue_end;
    }
    
    push_element(queue, data);

    queue_end: 
    os_mut_exit(&queue->queue_lock);
}

void* os_queue_pop_timeout(os_queue_t *queue, int timeout_ms){
    void *data = NULL;

    os_mut_entry(&queue->queue_lock, timeout_ms);

    if(queue->current_num_elements !=  0){        
        data = pop_element(queue);
    }
    os_mut_exit(&queue->queue_lock);
    return data;
}

void *os_queue_pop_indefinite(os_queue_t *queue){
    void *data = NULL;
    
    os_mut_entry_wait_indefinite(&queue->queue_lock);

    if(queue->current_num_elements !=  0){        
        data = pop_element(queue);
    }

    os_mut_exit(&queue->queue_lock);
    return data;
}
