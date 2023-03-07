#include "statemachine.h"
#include <Arduino.h>

statemachine_t *init_new_statemachine(const int num_states, const int init_state, statemachine_state_t *states_list){

    // Basic bounds check
    if(num_states <= 0 || states_list == NULL)
        return NULL;

    statemachine_t *statemachine = (statemachine_t*)malloc(sizeof(statemachine_t));

    statemachine->current_state = init_state;
    statemachine->latest_event = 0;
    statemachine->num_states = num_states;
    statemachine->latest_event = NULL_EVENT;
    statemachine->states_list = states_list;

    for (int n = 0; n < num_states; n++)
    {
        // Init and clear all event submission data.
        for(int k = 0; k < statemachine->states_list[n].num_events; k++){
            event_submission_t *event_sb = &statemachine->states_list[n].events_list[k];

            if(event_sb == NULL){
                free(statemachine);
                return NULL;
            }
        }
    }

    return statemachine;
}

int statemachine_submit_event(statemachine_t *statemachine, int event, void *params){
    if(statemachine == NULL)
        return MK_INT_ERR;
    int current_state = statemachine->current_state;

    int next_state = -1;
    int event_index = -1;

    // Look through list of events, then add to state list
    for (int n = 0; n < statemachine->states_list[current_state].num_events; n++){
        // Find correct event id, then submit!
        if(event == statemachine->states_list[current_state].events_list[n].event_id){
            // Set next state value
            next_state = statemachine->states_list[current_state].events_list[n].next_state;
            // Then next event index value
            event_index = statemachine->states_list[current_state].events_list[n].event_id;
            break;
        }

        if(END_EVENT == statemachine->states_list[current_state].events_list[n].event_id){
            return MK_NOT_INITED;
        }
    }
    // Couldn't find correct event in current statemachine
    // Return error and don't exit/enter states or run event callbacks
    if (next_state == -1)
        return MK_INVALID_PARAM;

    // Run exit function
    if(statemachine->states_list[current_state].exit_function != NULL)
        statemachine->states_list[current_state].exit_function(
            event,
            current_state,
            &next_state,
            params);

    // Run event callback
    if(statemachine->states_list[current_state].events_list[event_index].event_cb_function != NULL)
        statemachine->states_list[current_state].events_list[event_index].event_cb_function(
            event,
            current_state,
            &next_state,
            params);

    // Run entry function
    if(statemachine->states_list[next_state].entry_function != NULL){
        statemachine->states_list[next_state].entry_function(
            event,
            current_state,
            &next_state,
            params);
    }

    statemachine->current_state = next_state;

    return MK_OK;
}

int statemachine_set_state(statemachine_t *statemachine, int next_state, void *param){
    if(statemachine == NULL || statemachine->num_events <= next_state)
        return MK_INT_ERR;

    int current_state = statemachine->current_state;

    // Run exit function
    if(statemachine->states_list[current_state].exit_function != NULL)
        statemachine->states_list[current_state].exit_function(
            -1,
            current_state,
            &next_state,
            param);

    // Run entry function
    if(statemachine->states_list[next_state].entry_function != NULL){
        statemachine->states_list[next_state].entry_function(
            -1,
            current_state,
            &next_state,
            param);
    }

    return MK_OK;
}
