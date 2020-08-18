#include "teensy_coms.h"

// Defining the stack space that we are putting all of our message management stuff into 
#define MESSAGE_MANAGEMENT_STACK_SIZE 3084
static uint32_t message_management_stack[MESSAGE_MANAGEMENT_STACK_SIZE]; 

/*
* @brief: Struct that helps us deal with message subroutine callbackets
* @notes: Switched from linkedlist to predefined array for preformance reasons. 
*/
struct MessageSubroutine{
    // Pass in function as arguement
    void (*func)(MessageReq *ptr);
    // Which type of message we are looking for
    MessageData_MessageType msg_type;
    // Whether or not we want to disable the callback    
    bool en = false;
    // If we "delete" a callback, since we statically allocated all this 
    // Stuff, we just say whether or not the message subroute is used or not. 
    bool used = false; 
};
#define MAX_MESSAGE_SUBROUTINE_NUM 64
static MessageSubroutine message_subroutine_list[MAX_MESSAGE_SUBROUTINE_NUM]; 
static uint32_t callback_num = 0;
static MutexLock message_callback_mutex; 

static void run_threads(void);

/*
*   @brief The thread function that we will do all of our message management stuff in
*/
void message_management_thread(void *parameters){
    uint32_t last_millis; 
    for(;;){
        last_millis = millis(); 
        
        run_threads();

        // Ensures that we sleep for 14 milliseconds every time 
        uint32_t latest_millis = millis() - last_millis; 
        // If it took longer than 14 milliseconds to complete last loop
        // Then we don't delay at all
        if(latest_millis < 14)
            // Otherwise delay for the remainder of the 14ms period so that we delay as close to 
            // 14 ms as we can get. 
            os_thread_delay_ms(14 - latest_millis);
    }
}

__attribute__((always_inline)) static void run_threads(void){

}

/* 
* @brief Easy method for dealing with new messages coming into the system 
* @notes Just makes callbacks easy to deal with, you still need to deal with the deserialization
* And unpacking yourself. 
* params MessageData_MessageType which type of message data are we sending?
* params void(*func)(MessageReq *ptr) callback for dealing with subroutines. 
* returns MessageSubroutineSetupStatus whether or not we actually go the information we needed and the handler ID
*/
extern MessageSubroutineSetupReturn add_message_callback(MessageData_MessageType msg_type, void(*func)(MessageReq *ptr)){
    MessageSubroutineSetupReturn setup_return; 

    uint8_t msg_id;  
    // Running through and looking for the first spot in the array
    // That doesn't has a thread used spot. 
    for(msg_id = 0; msg_id < MAX_MESSAGE_SUBROUTINE_NUM; msg_id++){
        if(message_subroutine_list[msg_id].used == false)
            break;     
    }
    
    // If we ran out of spots to put our message subroutine stuff
    if((msg_id+1) == MAX_MESSAGE_SUBROUTINE_NUM){
        setup_return.setup_status = SUBROUTINE_ADD_FAIL_MAX_NUM_REACHED;
        return setup_return; 
    }

    // Flags that tell machine that we are using this spot, and that 
    // This callback is enabled 
    message_subroutine_list[msg_id].en = true; 
    message_subroutine_list[msg_id].used = true; 
    
    message_subroutine_list[msg_id].msg_type = msg_type; 
    message_subroutine_list[msg_id].func = func; 

    // Let's owner function know that we were able to setup the subroutine
    // And the id of our message handler. 
    setup_return.callback_handler_id = msg_id; 
    setup_return.setup_status = SUBROUTINE_ADD_SUCCESS; 
    callback_num++; 
    return setup_return; 
}

/*
*   @brief Removes a message callback, so we aren't getting callbacks from that anymore 
*   @params uint32_t callback_handler_id(which thread are we trying to remove)
*   @returns Whether or not we were able to remove the callback and why. 
*/
extern MessageSubroutineSetupStatus remove_message_callback(uint32_t callback_handler_id){
    if(callback_handler_id >= MAX_MESSAGE_SUBROUTINE_NUM)
        return SUBROUTINE_REMOVE_OUT_OF_BOUMDS; 

    // Since we are no longer using this space
    // It will be there to use for the next person to ask for a callback!
    message_subroutine_list[callback_handler_id].used = false; 
    callback_num--; 
    return SUBROUTINE_REMOVE_SUCCESS; 
}

/*
*   @brief  Starts up all of the message management stuff so we can get messages!
*   @notes  Just call this, and then attach whatever event driven messaging stuff you feel you need to do 
*/
void message_management_begin(void){
    // Starts up the serial interface.
    os_usb_serial_begin(); 
    // We have our own thread that deals with the message management stuff. 
    os_add_thread(&message_management_thread, NULL, MESSAGE_MANAGEMENT_STACK_SIZE, &message_management_stack);
}