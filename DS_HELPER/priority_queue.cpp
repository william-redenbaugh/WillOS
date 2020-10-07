#include "priority_queue.hpp"


/*!
*   @brief Inserts an element with a defined priority into the priority queue/heap
*   @param void *ptr address pointer to whatever you want
*   @param uint16_t priority of data we are inputing into system  
*/
void PriorityQueuePointerNaive::insert(void *ptr, uint16_t priority){
    
    #ifdef OS_FAST_MALLOC_MODULE
    // Allocating new node on the heap.
    PriorityQueueNaiveNode *new_node = (PriorityQueueNaiveNode*)fast_malloc(sizeof(PriorityQueueNaiveNode)); 
    #else
    PriorityQueueNaiveNode *new_node = new PriorityQueueNaiveNode; 
    #endif
    
    // What's the priority of the data. 
    new_node->priority = priority; 

    // Needs to have some kind of relevant data.
    new_node->ptr = ptr; 
    
    if(this->head == NULL){
        // We need to populate our new head with relevant data. 
        this->head = new_node;
        // Since the head is null, we know there is no other data.  
        new_node->next = NULL; 
    }
    // We need to go through LinkedList and find where the priority node should sit. 
    else{
        // We will use this node to traverse through the head. 
        PriorityQueueNaiveNode *current_node = this->head; 
        // We go through the linked lists and keep iterating until we find something of lower priority
        // Or until we reach the end of the list
        // Note, this does mean that if we find something of equal priority, let's say 4 items have a priorty of 7, 
        // And we add a 5th item with a priority of 7, that node will be attached to the end of the list
        // Satisfying the fist in, last out stratagy. 
        
        if(this->head->priority <= new_node->priority){
            new_node->next = this->head; 
            this->head = new_node; 
            return; 
        }

        while(current_node->next != NULL && current_node->next->priority >= new_node->priority)
            current_node = current_node->next; 

        new_node->next = current_node->next; 
        current_node->next = new_node; 
    }
}

/*!
*   @brief "Pops" off the highest priority value from the priority queue
*   @returns void* ptr general purpose address pointer to hold location of real data. 
*/
void* PriorityQueuePointerNaive::pop(void){
    if(this->head == NULL)
        return NULL; 
    
    // Popping off data from the pointer 
    register void *ptr = this->head->ptr; 
    PriorityQueueNaiveNode *next_node = this->head->next; 
    
    // Free up in memory. 
    #ifdef OS_FAST_MALLOC_MODULE
    fast_malloc_free(this->head); 
    #else
    free(this->head); 
    #endif
    
    this->head = next_node; 
    return ptr; 
} 

/*!
*   @brief Looks at highest priority data, and just returns information, but doesn't pop off data. 
*   @returns void* ptr general purpose address pointer to hold location of real data. 
*/
void* PriorityQueuePointerNaive::peek_top(void){
    if(this->head == NULL)
        return NULL; 
    register void *ptr = this->head->ptr; 
    return ptr; 
}

/*!
*   @brief Checks to see if a particular pointer address can be found in the priority queue
*   @param *ptr to information
*   @returns priority of that pointer data, or -1 if it cannot be found
*/
int PriorityQueuePointerNaive::check_exists(void *ptr){
    if(this->head = NULL)
        return -1; 

    struct PriorityQueueNaiveNode *current_node = this->head; 
    while(current_node != NULL){
        if(current_node->ptr = ptr)
            return current_node->priority;

        current_node = current_node->next; 
    }
    return -1; 
}

/*!
*   @brief Looks at highest priority data, and returns the top of the node 
*   @returns PriorityQueueNaiveNode* top level node. Or null of no node exists.  
*/
PriorityQueueNaiveNode* PriorityQueuePointerNaive::peek_top_node(void){
    return this->head; 
}

#ifdef PRIORITY_QUEUE_HEAP

/*!
*   @brief We need to setup the priority queue. 
*/
void PriorityQueuePointerHeap::init_priority_queue(uint16_t priority_queue_size){
    // Saving max node count
    this->max_node_size = priority_queue_size; 
    // Allocating space of array and setting starting pointer 
    #ifdef FAST_MALLOC_MODULE
    this->node_list = (PriorityQueueHeapNode*) fast_malloc(sizeof(PriorityQueueHeapNode) * priority_queue_size);
    #else
    this->node_list = (PriorityQueueHeapNode*) malloc(sizeof(PriorityQueueHeapNode) * priority_queue_size);
    #endif
}

/*!
*   @brief Inserts an element with a defined priority into the priority queue/heap
*   @param void *ptr address pointer to whatever you want
*   @param uint16_t priority of data we are inputing into system  
*/
void PriorityQueuePointerHeap::insert(void *ptr, uint16_t priority){
    struct PriorityQueueHeapNode new_node; 
    new_node.priority = priority;
    new_node.ptr = ptr; 

    // If we have reached the total number of node available 
    if(this->total_nodes == this->max_node_size)
        return; 

    // Saving new node data information
    this->node_list[this->total_nodes] = new_node; 

    // Since we are adding another node, we bring it in
    this->total_nodes++;

    int m = this->total_nodes - 1; 
    while(m != 0 && this->node_list[this->parent(m)].priority < this->node_list[m].priority){
        this->swap(&this->node_list[parent(m)], &this->node_list[m]); 
        m = parent(m); 
    }
}

/*!
*   @brief "Pops" off the highest priority value from the priority queue
*   @returns void* ptr general purpose address pointer to hold location of real data. 
*/
void* PriorityQueuePointerHeap::pop(void){
    void *ptr = this->node_list[0].ptr; 

    this->total_nodes--; 
    this->node_list[0] = this->node_list[this->total_nodes]; 

    // Resort heap. 
    this->max_heapify(0); 

    // Return new pointer to data. 
    return ptr; 
}

/*!
*   @brief Moves element into the proper space in the array
*   @brief int (location of element in array)
*/
void PriorityQueuePointerHeap::max_heapify(int n){
    // Finding left and right child nodes
    int left = this->left_child(n); 
    int right = this->right_child(n); 

    // Finding the largest among three nodes. 
    int largest = n;

    // Check if left or right nodes are larger than the current node. 
    if(left <= this->total_nodes && this->node_list[left].priority > this->node_list[largest].priority)
        largest = left; 
    if(right <= this->total_nodes && this->node_list[right].priority > this->node_list[largest].priority)
        largest = right; 

    // Swap the largest with the current node, and repeat process(recursively)
    if(largest != n){
        // Swaping data. 
        struct PriorityQueueHeapNode temp = this->node_list[n]; 
        this->node_list[n] = this->node_list[largest]; 
        this->node_list[largest] = temp; 
        max_heapify(largest); 
    }
}

#endif 