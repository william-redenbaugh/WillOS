#include "statemachine.h"
#include <Arduino.h>

statemachine_t *init_new_statemachine(const int num_states, const int num_events, const int init_state, const statemachine_state_t *states_list){

    // Basic bounds check
    if(num_states <= 0 || num_events <= 0 || init_state >= num_states || states_list == NULL)
        return NULL;

    statemachine_t *statemachine = (statemachine_t*)malloc(sizeof(statemachine_t));

    statemachine->current_state = init_state;
    statemachine->latest_event = 0;
    statemachine->num_states = num_states;
    statemachine->latest_event = -1;
    statemachine->states_list = (statemachine_state_t*)malloc(sizeof(statemachine_t) * num_states);

    for(int n = 0; n < num_states; n++){
        statemachine->states_list[n] = states_list[n];
        statemachine->states_list[n].events_list = (event_submission_t*)malloc(sizeof(event_submission_t) * num_events);

        // Init and clear all event submission data.
        for(int k = 0; k < num_events; k++){
            event_submission_t *event_sb = &statemachine->states_list[n].events_list[k];
            event_sb->active = false;
            event_sb->cb_param_data = NULL;
            event_sb->event_cb_function = NULL;
            event_sb->event_id = 0;
            event_sb->next_state = 0;
        }
    }

    return statemachine;
}

int statemachine_submit_event(statemachine_t *statemachine, int event, void *params){
    if(statemachine == NULL || statemachine->num_events <= event)
        return MK_INT_ERR;
    int current_state = statemachine->current_state;

    // Event isn't active for state.
    if(statemachine->states_list[current_state].events_list[event].active == false){
        return MK_INT_ERR;
    }

    int next_state = statemachine->states_list[current_state].events_list[event].next_state;

    // Run exit function
    if(statemachine->states_list[current_state].exit_function != NULL)
        statemachine->states_list[current_state].exit_function(
            event,
            current_state,
            &next_state,
            params);

    // Run event callback
    if(statemachine->states_list[current_state].events_list[event].event_cb_function != NULL)
        statemachine->states_list[current_state].events_list[event].event_cb_function(
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

int set_statemachine_event_cb(statemachine_t *statemachine, int state, int event, int next_state, event_function_t func){

    if(statemachine == NULL || statemachine->num_states <= state || statemachine->num_events <= event)
        return MK_INT_ERR;

    event_submission_t *event_submit = &statemachine->states_list[state].events_list[event];

    event_submit->active = true;
    event_submit->event_cb_function = func;
    event_submit->event_id = event;
    event_submit->next_state = next_state;

    return MK_OK;
}

int clear_statemachine_event_cb(statemachine_t *statemachine, int state, int event){

    if(statemachine == NULL || statemachine->num_states <= state || statemachine->num_events <= event)
        return MK_INT_ERR;

    event_submission_t *event_submit = &statemachine->states_list[state].events_list[event];
    event_submit->active = false;

    event_submit->active = true;
    event_submit->event_cb_function = NULL;
    event_submit->cb_param_data = NULL;
    return MK_OK;
}