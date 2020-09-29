#ifndef _QUEUE_H
#define _QUEUE_H

#include <Arduino.h> 

struct QueueLinkedList{
    /*!
    *   @brief General Purpose pointer for our queue
    */
    void *ptr; 

    /*!
    *   @brief Next pointer for our queue
    */
    struct QueueLinkedList *next; 
}; 

/*!
*   @brief Queue that holds pointers 
*/
class PointerQueue{
    public:

        /*!
        *   @brief Adding something to our queue
        *   @param void *ptr (general purpose pointer.)
        */
        void enque(void *ptr); 
        
        /*!
        *   @brief Removing something from our queue
        */
        void* deque(void); 

        /*!
        *   @brief Help's us see what's at the begining of our Queue
        */
        void *peak_head(void); 

        /*!
        *   @brief Helps us see what's at the end of our queue
        */
        void *peak_tail(void); 
        
        /*!
        *   @brief Checks to see if something exists in our queue
        *   @note O(N) time complexity based off length of queue. Use wisely
        */
        bool check_exists(void *ptr); 

        /*!
        *   @returns the lenth of the queue
        */
        uint16_t get_size(void){
            return this->size; 
        } 
        
    private:

        /*!
        *   @brief Head of our linked list, AKA the first thing to pop off dueing a  queue
        */
        QueueLinkedList *head = nullptr; 

        /*!
        *   @brief Tail of our linked list, Helps us add things to the end of the LinkedList
        */
        QueueLinkedList *tail = nullptr; 

        /*!
        *   @brief Current number of elements in the queue
        */
        uint16_t size = 0; 
}; 

#endif 