#include "fast_malloc.hpp"

/*!
*   @brief Got a lot of pointers from this article: https://tharikasblogs.blogspot.com/p/how-to-write-your-own-malloc-and-free.html
*   @note Implamentation derived from this example
*/

#ifdef FAST_MALLOC_MODULE

/*!
*   @brief Data Structure that helps memory free memory block spaces. 
*/
struct mem_block{
    size_t size; 
    uint8_t free; 
    struct mem_block *next; 
};

/*!
*   @brief Statically Allocated Array that we will put all of our "dynamic" data onto
*/
static volatile uint8_t fast_malloc_array[FAST_MALLOC_SIZE_BYTES]; 

/*!
*   @brief Pointing to original block of memory, which is free at startup
*/  
struct mem_block *free_list = (void*)fast_malloc_array; 

/*!
*   @brief Function declaration
*/
void fast_malloc_init(void); 
static void fast_malloc_split(struct mem_block *fitting_slot, size_t size); 
void* fast_malloc(size_t size); 
void fast_malloc_free(void *ptr); 
void fast_malloc_merge(void); 

/*!
*   @brief Set's up the malloc
*/
void fast_malloc_init(void){
    free_list->size = FAST_MALLOC_SIZE_BYTES - sizeof(struct mem_block); 
    free_list->free = 1; 
    free_list->next = NULL; 
}

/*!
*   @brief Setting up new block allocation by splitting a free block; 
*/
static void fast_malloc_split(struct mem_block *fitting_slot, size_t size){
    struct mem_block *new_block = (void*)((void*)fitting_slot+size+sizeof(struct mem_block));
    new_block->free = 1; 
    new_block->next = fitting_slot->next; 
    fitting_slot->size = size; 
    fitting_slot->free = 0; 
    fitting_slot->next = new_block; 
}

/*!
*   @brief Allocates memory on fast memory bank
*   @return Pointer to the area in memory. 
*/
void* fast_malloc(size_t size){
    
    struct mem_block *current, *previous; 
    register void *result; 

    // If we haven't setup our fast_malloc_module yet. 
    if(!free_list->size)
        fast_malloc_init(); 

    current = free_list; 
    
    // Siphing through the the lined lists and trying to find the smallest contiguous space needed 
    while(((current->size < size) || (current->free == 0)) && (current->next != NULL)){
        previous = current; 
        current = current->next; 
    }

    // If the block of space we find is exactly the same size as the current block
    if(current->size == size){
        // Space after that block
        result = (void*)(++current); 
        return result; 
    } 
    else if(current->size > size){
        fast_malloc_split(current, size); 
        result = (void*)(++current); 
        return result; 
    }
    // We couldn't find enough space, return null. 
    else{
        result = NULL;        
        return result; 
    }
}

/*!
*   @brief Frees up the memory in the data. 
*   @param Pointer to the memory bank data. 
*/
void fast_malloc_free(void *ptr){

    // If we chose a valid location to free data from
    if(((void*)fast_malloc_array <= ptr) && (ptr <= (void*)fast_malloc_array + FAST_MALLOC_SIZE_BYTES)){
        struct mem_block *curr = (struct mem_block*)ptr; 
        // Go back in memory
        --curr; 
        curr->free = 1; 
        fast_malloc_merge(); 
    }
}

/*!
*   @brief Merges all the unused blocks into contiguous spaces in memory. 
*/
void fast_malloc_merge(void){
    struct mem_block *current, *previous; 
    current = free_list; 
    
    if(current == NULL)
        return; 

    while(current && (current->next != NULL)){
        if(current->free && current->next->free){
            current->size += current->next->size + sizeof(struct mem_block); 
            current->next = current->next->next; 
        }
        previous = current; 
        current = current->next; 
        //__flush_cpu_pipeline(); 
    }
    return; 
}

#endif