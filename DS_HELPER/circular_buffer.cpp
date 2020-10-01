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
*   @brief Insert a string of defined size into the circular buffer
*   @param char* (character array pointer that we are inserting into the system)
*   @param size_t len(length of string that we are inserting into the system)
*   @return (1 if successful, 2 if successful and overwriting, 0 if too large)
*/
bool CircularBufferString::insert(char *arr, size_t len){
    if(len > this->buffer_length){
        return false; 
    }
    else{
        register int pos = this->current_position; 
        register char *current_char = this->buffer + current_position; 
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
*   @brief Deconstructs array
*/
void CircularBufferString::deinit(void){
    if(this->buffer != NULL){
        if(this->fast_mem)
            fast_malloc_free(this->buffer);
        else
            free(this->buffer); 
    }
}

#endif 