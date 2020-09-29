#include "fast_malloc.hpp"

/*!
*   @brief Got a lot of pointers from this article: https://tharikasblogs.blogspot.com/p/how-to-write-your-own-malloc-and-free.html
*   @note Implamentation derived from this example
*/

typedef enum memory_block_status_t{
    MEMORY_BLOCK_TAKEN = 0, 
    MEMORY_BLOCK_FREE = 1
}; 

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
    free_list->free = MEMORY_BLOCK_FREE; 
    free_list->next = NULL; 
}

/*!
*   @brief Setting up new block allocation by splitting a free block; 
*   @param struct mem_block 
*   @param size_t size 
*/
static void fast_malloc_split(struct mem_block *fitting_slot, size_t size){
    // Create a pointer to a new block, this will be our new free space block
    // This can be see since we start the pointer after all the space required for our new block of 
    // defined size_t size
    struct mem_block *new_block = (void*)((void*)fitting_slot + size + sizeof(struct mem_block));
    
    // Our new block is the new "free block" that's resulted from siphoning off a space of memory for you
    new_block->free = MEMORY_BLOCK_FREE; 

    // Since our new block is supposed to come after the used block, we set to point to the 
    // next block pointer
    new_block->next = fitting_slot->next; 

    // Our newly 'used' memory is defined with a size value
    // Flagged as no longer free
    // And it points to the new memory block
    fitting_slot->size = size; 
    fitting_slot->free = MEMORY_BLOCK_TAKEN; 
    fitting_slot->next = new_block; 
}

/*!
*   @brief Allocates memory on fast memory bank
*   @return Pointer to the area in memory. 
*/
void* fast_malloc(size_t size){

    struct mem_block *current, *previous; 
    
    // Poiner to our new resulting information
    register void *result; 

    // If we haven't setup our fast_malloc_module yet. 
    if(!free_list->size)
        fast_malloc_init(); 

    // We want to start looking at the begining of our memory arry
    current = free_list; 
    
    // Siphing through the the linked lists, and looks for first block that has enough space 
    // For our newly allocated array
    while(((current->size < size) || (current->free == MEMORY_BLOCK_TAKEN)) && (current->next != NULL)){
        previous = current; 
        current = current->next; 
    }

    // If the block of space we find is exactly the same size as the current block
    if(current->size == size){
        // Space after that block
        result = (void*)(++current); 
        return result; 
    } 
    // If the block doesn't fit properly(is smaller than the empty block)
    // We need to split up the blocks
    else if(current->size > size){

        // We split the memory block
        fast_malloc_split(current, size);

        // Our result is just whatever happens after current;  
        result = (void*)(++current); 
        // We return the pointer to our newly allocated array 
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
        // Go back in memory to begining of block signature, which
        // Holds memory block state
        --curr; 
        // We allocate memory block as free
        curr->free = MEMORY_BLOCK_FREE; 

        // We the go through memory block and merge unused blocks 
        fast_malloc_merge(); 
    }
}

/*!
*   @brief Merges all the unused blocks into contiguous spaces in memory. 
*/
void fast_malloc_merge(void){
    
    struct mem_block *current; 
    current = free_list;
    
    while(current->next != NULL){
        // Checking to see if the current and next blocks are free
        // For sake of reducing CPU cycles we don't conditionaly check anythingMEMORY_BLOCK_FREE
        if(current->free && current->next->free){
            current->size += current->next->size + sizeof(struct mem_block); 
            current->next = current->next->next; 
            // We don't wanna iterate to the next module until we've found all available free mergable modules
            continue; 
        }
        // Otherwise we iterate through the list and look for the next memory module
        current = current->next; 
    }
    return; 
}
