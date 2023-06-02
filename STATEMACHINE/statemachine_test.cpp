#include "CSAL/CSAL_SHARED/statemachine.h"
#include <Arduino.h>

#include "enabled_modules.h"

#ifdef STATEMACHINE

typedef enum test_states{
    TEST_STATE_ONE = 0,
    TEST_STATE_TWO,
    TEST_STATE_THREE,
    END_TEST_STATE = END_STATE,
}test_states_t;

typedef enum test_events{
    TEST_EVENT_ONE = 0,
    TEST_EVENT_TWO,
    TEST_EVENT_THREE,
    END_TEST_EVENT = END_EVENT,
}test_events_t;

void state_one_entry_function(int event, int old_state, int *next_state, void* param){
}

void state_one_exit_function(int event, int old_state, int *next_state, void* param){
}

void state_two_entry_function(int event, int old_state, int *next_state, void* param){

}

void state_two_exit_function(int event, int old_state, int *next_state, void* param){

}

void state_three_entry_function(int event, int old_state, int *next_state, void* param){

}

void state_three_exit_function(int event, int old_state, int *next_state, void* param){

}

void state_one_event_one_function(int event, int old_state, int *next_state, void *params){

}

void state_one_event_two_function(int event, int old_state, int *next_state, void *params){

}

void state_one_event_three_function(int event, int old_state, int *next_state, void *params){

}

void state_two_event_one_function(int event, int old_state, int *next_state, void *params){

}

void state_two_event_two_function(int event, int old_state, int *next_state, void *params){

}

void state_two_event_three_function(int event, int old_state, int *next_state, void *params){

}

void state_three_event_one_function(int event, int old_state, int *next_state, void *params){

}

void state_three_event_two_function(int event, int old_state, int *next_state, void *params){

}

void state_three_event_three_function(int event, int old_state, int *next_state, void *params){

}

event_submission_t state_one_eventlist[] = {
    {state_one_event_one_function, NULL, TEST_EVENT_ONE, TEST_STATE_TWO},
    {state_one_event_two_function, NULL, TEST_EVENT_TWO, TEST_STATE_ONE},
    {state_one_event_three_function, NULL, TEST_EVENT_THREE, TEST_STATE_THREE},
    {NULL, NULL, NULL_EVENT, NULL_STATE},
};

event_submission_t state_two_eventlist[] = {
    {state_two_event_one_function, NULL, TEST_EVENT_ONE, TEST_STATE_ONE},
    {state_two_event_two_function, NULL, TEST_EVENT_TWO, TEST_STATE_TWO},
    {state_two_event_three_function, NULL, TEST_EVENT_THREE, TEST_STATE_THREE},
    {NULL, NULL, NULL_EVENT, NULL_STATE},
};

event_submission_t state_three_eventlist[] = {
    {state_three_event_one_function, NULL, TEST_EVENT_ONE, TEST_STATE_ONE},
    {state_three_event_two_function, NULL, TEST_EVENT_TWO, TEST_STATE_ONE},
    {state_three_event_three_function, NULL, TEST_EVENT_THREE, TEST_STATE_TWO},
    {NULL, NULL, NULL_EVENT, NULL_STATE},
};

statemachine_state_t test_state_list[] = {
    {TEST_STATE_ONE, state_one_entry_function, state_one_exit_function, NUM_EVENTS(state_one_eventlist), state_one_eventlist},
    {TEST_STATE_ONE, state_two_entry_function, state_two_exit_function, NUM_EVENTS(state_two_eventlist), state_two_eventlist},
    {TEST_STATE_THREE, state_three_entry_function, state_three_exit_function, NUM_EVENTS(state_three_eventlist), state_three_eventlist},
    {NULL_STATE, NULL, NULL, 0, NULL},
};

void test_new_statemachine(void){

    int num_state_list = NUM_STATES(test_state_list);
    for(int k = 0; k < num_state_list; k++){
        int current_state = test_state_list[k].num_events;
        Serial.print(current_state);
    }
    statemachine_t *test_sm = init_new_statemachine(NUM_STATES(test_state_list), TEST_STATE_ONE, test_state_list);
    statemachine_submit_event(test_sm, TEST_EVENT_ONE, NULL);
} 
#endif