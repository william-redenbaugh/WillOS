#include "queue.h"
#include "fast_malloc.hpp"

/*!
*   @brief Adding something to our queue
*   @param void *ptr (general purpose pointer.)
*/
void PointerQueue::enque(void *ptr){
    // The assumption is that if the tail is null, so is the head
    if(this->tail == NULL){
        this->tail = (QueueLinkedList*)fast_malloc(sizeof(QueueLinkedList())); 
        // Add everything
        this->head = this->tail; 
        this->tail->ptr = ptr; 
    } 
    else{
        // QueueLinkedList *new_tail = new QueueLinkedList(); 
        QueueLinkedList *new_tail = (QueueLinkedList*)fast_malloc(sizeof(QueueLinkedList())); 
        
        new_tail->ptr = ptr;
        // Since we are
        this->tail->next = new_tail;
        this->tail = this->tail->next; 
    }
}

/*!
*   @brief Removing something from our queue
*/
void* PointerQueue::deque(void){
    if(this->head == NULL)
        return NULL; 

    // Get the current pointer of the queue
    void *ptr = this->head->ptr; 

    QueueLinkedList *new_head = this->head->next; 

    // Make sure that memory is given back
    fast_malloc_free(this->head); 

    // Then increment the head. 
    this->head = new_head; 

    // If the head is null, we want to ensure that the tail becomes null as well(since it's memory is freed. )
    if(this->head == NULL)
        this->tail = NULL;

    return ptr; 
}

/*!
*   @brief Help's us see what's at the begining of our Queue
*/
void* PointerQueue::peak_head(void){
    if(this->head != NULL)
        return this->head->ptr; 
    
    return NULL; 
}

/*!
*   @brief Helps us see what's at the end of our queue
*/
void* PointerQueue::peak_tail(void){
    if(this->head != NULL)
        return this->head->ptr; 
    
    return NULL; 
}