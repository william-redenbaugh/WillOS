#include "queue.h"
#include "fast_malloc.hpp"

/*!
*   @brief Adding something to our queue
*   @param void *ptr (general purpose pointer.)
*/
void PointerQueue::enque(void *ptr){
    QueueLinkedList *new_tail;

    // The assumption is that if the tail is null, so is the head
    if(this->tail == NULL){
        #ifdef OS_FAST_MALLOC_MODULE
        new_tail = (QueueLinkedList*)fast_malloc(sizeof(QueueLinkedList));
        #else
        new_tail = new QueueLinkedList; 
        #endif 
        this->tail = new_tail; 
        // Add everything
        this->head = this->tail; 
        this->tail->ptr = ptr; 
    } 
    
    else{
        #ifdef OS_FAST_MALLOC_MODULE
        new_tail = (QueueLinkedList*)fast_malloc(sizeof(QueueLinkedList));
        #else
        new_tail = new QueueLinkedList; 
        #endif 
        this->tail = new_tail; 

        // QueueLinkedList *new_tail =  
        new_tail->next = NULL; 
        new_tail->ptr = ptr;
        // Connecting the tail to the head 
        this->tail->next = new_tail;
        // Then since this is our new tail, we need to add it 
        this->tail = this->tail->next; 
    }

    // We increase the size by one 
    this->size++; 
}

/*!
*   @brief Removing something from our queue
*/
void* PointerQueue::deque(void){
    if(this->head == NULL)
        return NULL; 

    // Get the current pointer of the queue
    void *ptr = this->head->ptr; 

    // We want to access the next data member
    QueueLinkedList *new_head = this->head->next; 

    // Make sure that memory is given back to the system 
    #ifdef OS_FAST_MALLOC_MODULE
    fast_malloc_free(this->head); 
    #else
    free(this->head); 
    #endif

    // Then increment the head. 
    this->head = new_head; 

    // If the head is null, we want to ensure that the tail becomes null as well(since it's memory is freed. )
    if(this->head == NULL)
        this->tail = NULL;
    
    // The size of the queue is smaller s
    this->size--;
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