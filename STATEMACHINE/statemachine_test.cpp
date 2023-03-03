#include "statemachine.h"
#include <Arduino.h>

typedef enum test_states{
    TEST_STATE_ONE = 0,
    TEST_STATE_TWO,
    TEST_STATE_THREE
}test_states_t;

typedef enum test_events{
    TEST_EVENT_ONE = 0,
    TEST_EVENT_TWO,
    TEST_EVENT_THREE
}test_events_t;

void state_one_entry_function(void* params){

}

void state_one_exit_function(void* params){

}

void state_two_entry_function(void* params){

}

void state_two_exit_function(void* params){

}

void state_three_entry_function(void* params){

}

void state_three_exit_function(void* params){

}

statemachine_state_t test_state_list[] = {
    {TEST_STATE_ONE, state_one_entry_function, state_one_exit_function, NULL, NULL, sizeof(test_events_t), NULL},
    {TEST_STATE_ONE, state_two_entry_function, state_two_exit_function, NULL, NULL, sizeof(test_events_t), NULL},
    {TEST_STATE_THREE, state_three_entry_function, state_three_exit_function, NULL, NULL, sizeof(test_events_t), NULL},
};

void test_new_statemachine(void){
    const int num_states = sizeof(test_state_list)/sizeof(statemachine_state_t);
    statemachine_t *test_sm = init_new_statemachine(num_states, sizeof(test_events), test_state_list);
}