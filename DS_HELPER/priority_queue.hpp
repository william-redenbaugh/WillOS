#ifndef _PRIORITY_QUEUE_HPP
#define _PRIORITY_QUEUE_HPP

#include "../enabled_modules.h"
#include <Arduino.h> 
#include "fast_malloc.hpp"

struct PriorityQueueNaiveNode{
    /*!
    *   @brief Pointer to address of real data. 
    */
    void *ptr; 

    /*!
    *   @brief The priority number
    */
    uint16_t priority; 

    /*!
    *   @brief Pointer to the next node in the priority queue
    */
    struct PriorityQueueNaiveNode *next; 
};

class PriorityQueuePointerNaive{
public:
    /*!
    *   @brief Inserts an element with a defined priority into the priority queue/heap
    *   @param void *ptr address pointer to whatever you want
    *   @param uint16_t priority of data we are inputing into system  
    */
    void insert(void *ptr, uint16_t priority); 
    
    /*!
    *   @brief "Pops" off the highest priority value from the priority queue
    *   @returns void* ptr general purpose address pointer to hold location of real data. 
    */
    void* pop(void); 
    
    /*!
    *   @brief Looks at highest priority data, and just returns information, but doesn't pop off data. 
    *   @returns void* ptr general purpose address pointer to hold location of real data. 
    */
    void *peek_top(void); 

    /*!
    *   @brief Looks at highest priority data, and returns the top of the node 
    *   @returns PriorityQueueNaiveNode* top level node. Or null of no node exists.  
    */
    PriorityQueueNaiveNode *peek_top_node(void); 

    /*!
    *   @brief Checks to see if a particular pointer address can be found in the priority queue
    *   @param *ptr to information
    *   @returns priority of that pointer data, or -1 if it cannot be found
    */
    int check_exists(void *ptr);        

private: 

    /*!
    *   @brief Contains Highest Priority Node and head of list. 
    */
    struct PriorityQueueNaiveNode *head = NULL;    
};



#ifdef PRIORITY_QUEUE_HEAP

struct PriorityQueueHeapNode{
    /*!
    *   @brief General Purpose pointer that we can use for anything
    */
    void *ptr; 
    /*!
    *   @brief What the priority of the node is
    */
    uint16_t priority; 
};

/*!
*   @brief Heap implementation of a priority queue to allow us to have quick n easy access to our priority queueu
*   @note Ensure that whatever thread calls this has a large enough stack size to handle traversing through the 
*   @note The array recursively(otherwise use the naive approach if we run low on memory, but isn't not
*/
class PriorityQueuePointerHeap{
public:

    /*!
    *   @brief We need to setup the priority queue. 
    */
    void init_priority_queue(uint16_t priority_queue_size); 

    /*!
    *   @brief Deconstructor for priority queue
    */
    void deinit(void){
        #ifdef FAST_MALLOC_MODULE 
        fast_malloc_free(this->node_list);
        #else 
        free(this->node_list); 
        #endif
    }

    /*!
    *   @brief Inserts an element with a defined priority into the priority queue/heap
    *   @param void *ptr address pointer to whatever you want
    *   @param uint16_t priority of data we are inputing into system  
    */
    void insert(void *ptr, uint16_t priority); 
    
    /*!
    *   @brief "Pops" off the highest priority value from the priority queue
    *   @returns void* ptr general purpose address pointer to hold location of real data. 
    */
    void* pop(void); 
    
    /*!
    *   @brief Looks at highest priority data, and just returns information, but doesn't pop off data. 
    *   @returns void* ptr general purpose address pointer to hold location of real data. 
    */
    void *peek_top(void){
        return this->node_list[0].ptr; 
    }

    /*!
    *   @brief Looks at highest priority data, and returns the top of the node 
    *   @returns PriorityQueueNaiveNode* top level node. Or null of no node exists.  
    */
    PriorityQueueHeapNode *peek_top_node(void){
        if(this->total_nodes == 0)
            return NULL; 

        return this->node_list; 
    }

    /*!
    *   @brief Checks to see if a particular pointer address can be found in the priority queue
    *   @param *ptr to information
    *   @returns priority of that pointer data, or -1 if it cannot be found
    */
    int check_exists(void *ptr); 

    /*!
    *   @returns How many elements we have
    */
    int num_elemnts(void){return this->total_nodes; }   

    /*!
    *   @brief Array pointer to all elements
    */
    PriorityQueueHeapNode* all_elements(void){return this->node_list; }

private: 

    /*!
    *   @brief Find index of the parent. 
    */
    int parent(int i){
        return (i - 1) / 2; 
    }

    /*!
    *   @brief return the index of the left child
    */
    int left_child(int i){
        return 2 * i + 1; 
    }

    /*!
    *   @brief return the index of the right child
    */
    int right_child(int i){
        return 2 * i + 2; 
    }

    /*!
    *   @brief Allows us to swap two nodes. 
    */
    void swap(PriorityQueueHeapNode *x, PriorityQueueHeapNode *y){
        PriorityQueueHeapNode temp = *x; 
        *x = *y; 
        *y = temp; 
    }

    /*!
    *   @brief Moves element into the proper space in the array
    *   @brief int (location of element in array)
    */
    void max_heapify(int n); 

    // Numbers of nodes currently in the system
    uint16_t total_nodes = 0;  
    // Max number of nodes that the system will take
    uint16_t max_node_size; 
    // Pointer to node list that holds all of our priority queue stuff. 
    struct PriorityQueueHeapNode *node_list; 

};
#endif 
#endif 