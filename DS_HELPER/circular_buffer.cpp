#include "circular_buffer.hpp"

#ifdef CIRCULAR_BUFFER_STRING_MODULE

#ifdef __IMXRT1062__
/*!
*   @brief Initializes the circular buffer with initalized array
*   @param uint32_t buffer_size(size of buffer you want your str buffer to have)
*   @param bool whether or not we allocated onto the faster RAM bank, or the slower RAM bank
*/
void CircularBufferString::init(uint32_t buffer_size, bool fast_mem){
    this->fast_mem = fast_mem;
    if(fast_mem)
        this->buffer = (char*)fast_malloc(sizeof(char)* buffer_size); 
    else
        this->buffer = (char*)malloc(sizeof(char)* buffer_size); 

    this->buffer_length = buffer_size; 
    this->current_position = 0; 
}
#else
/*!
*   @brief Initializes the circular buffer with initalized array
*   @param uint32_t buffer_size(size of buffer you want your str buffer to have)
*/
void CircularBufferString::init(uint32_t buffer_size){
    this->buffer = (char*)malloc(sizeof(char)* buffer_size); 

    this->buffer_length = buffer_size; 
    this->current_position = 0
}
#endif

/*!
*   @brief Initializes the circular buffer with initalized array
*   @param uint32_t buffer_size(size of buffer you want your str buffer to have)
*   @param char* pointer to ringbuffer array memory. 
*/
void CircularBufferString::init(uint32_t buffer_size, char *arr){
    this->buffer = arr; 
    this->buffer_length = buffer_size; 
    this->current_position = 0; 
}

/*!
*   @brief Insert a string of defined size into the circular buffer
*   @param char* (character array pointer that we are inserting into the system)
*   @param size_t len(length of string that we are inserting into the system)
*   @return (1 if successful, 2 if successful and overwriting, 0 if too large)
*/
bool CircularBufferString::insert(char *arr, size_t len){
    // If the array is too big or the buffer isn't initialized. 
    if(len > this->buffer_length || this->buffer == NULL){
        return false; 
    }
    else{
        register int pos = this->current_position; 
        register char *current_char = this->buffer + pos;  
        for(int n = 0; n < len; n++){
            *current_char = arr[n];
            current_char++; 
            pos++; 
            // reset position index and character pointer position
            if(pos == this->buffer_length){
                pos = 0; 
                current_char = this->buffer;
            }
        }

        // Add null pointer for end of string
        *current_char = NULL; 
        pos++; 
        this->current_position = pos; 
        return true; 
    }
}

/*!
*   @brief Get_string contents
*   @returns RetCircularBufferString struct with char array pointer and string length 
*/
RetCircularBufferString CircularBufferString::get_entire_buffer(void){
    RetCircularBufferString ret; 
    ret.arr = this->buffer;
    ret.len = this->buffer_length; 
    return ret; 
}

#ifdef __IMXRT1062__
/*!
*   @brief Deconstructs array
*/
void CircularBufferString::deinit(void){
    if(this->buffer == NULL)
        return; 

    if(this->buffer != NULL){
        if(this->fast_mem)
            fast_malloc_free(this->buffer);
        else
            free(this->buffer); 
    }
}
#else
void CircularBufferString::deinit(void){
    if(this->buffer == NULL)
        return; 
    
    free(this->buffer); 
}
#endif 

#endif 

#ifdef CIRCULAR_BUFFER_POINTER_MODULE

#ifdef __IMXRT1062__
/*!
*   @brief Initializes the circular buffer with initalized array
*   @param uint32_t buffer_size(size of buffer you want your str buffer to have)
*   @param bool whether or not we allocated onto the faster RAM bank, or the slower RAM bank
*/
void CircularBufferPointer::init(uint32_t buffer_size, bool fast_mem){
    this->fast_mem = fast_mem;
    if(fast_mem)
        this->buffer = (void**)fast_malloc(sizeof(void*) * buffer_size); 
    else
        this->buffer = (void**)malloc(sizeof(void*)* buffer_size); 

    this->buffer_length = buffer_size; 
    this->current_position = 0; 
}
#else
/*!
*   @brief Initializes the circular buffer with initalized array
*   @param uint32_t buffer_size(size of buffer you want your str buffer to have)
*/
void void CircularBufferPointer::init(uint32_t buffer_size){
    this->buffer = (void**)malloc(sizeof(void*)* buffer_size); 
    this->buffer_length = buffer_size; 
    this->current_position = 0
}
#endif

/*!
*   @brief Initializes the circular buffer with initalized array
*   @param uint32_t buffer_size(size of buffer you want your str buffer to have)
*   @param char* pointer to ringbuffer array memory. 
*/
void CircularBufferPointer::init(uint32_t buffer_size, void **arr){
    this->buffer = arr; 
    this->buffer_length = buffer_size; 
    this->current_position = 0; 
}

/*!
*   @brief Insert a string of defined size into the circular buffer
*   @param void *ptr(general purpose pointer on the system)
*/

bool CircularBufferPointer::insert(void *ptr){
    if(this->buffer == NULL)
        return false; 

    // Get current position of ringbuffer
   this->buffer[this->current_position] = ptr; 
   this->current_position++; 
   if(this->current_position == this->buffer_length)
    this->current_position = 0; 
}

/*!
*   @return Struct that holds enough info to get entire buffer(void **ptr array and uint32_t len of ringbuffer array)
*/  
RetCircularBufferPointer CircularBufferPointer::get_entire_buffer(void){
    RetCircularBufferPointer ret; 
    ret.len = this->buffer_length; 
    ret.ptr = this->buffer; 
    return ret;
} 

#ifdef __IMXRT1062__
/*!
*   @brief Deconstructs array
*/
void CircularBufferPointer::deinit(void){
    if(this->buffer == NULL)
        return; 

    if(this->buffer != NULL){
        if(this->fast_mem)
            fast_malloc_free(this->buffer);
        else
            free(this->buffer); 
    }
}
#else
void CircularBufferPointers::deinit(void){
    if(this->buffer == NULL)
        return; 
        
    free(this->buffer); 
}
#endif

#endif 