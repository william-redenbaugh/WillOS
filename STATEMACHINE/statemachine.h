#ifndef _STATEMACHINE_H
#define _STATEMACHINE_H

#include "OS/error.hpp"

#define END_STATE (-1)
#define END_EVENT (-1)
#define NULL_STATE (-1)
#define NULL_EVENT (-1)

/**
 * @brief Function called when we enter a specific state
*/
typedef void (*statemachine_entry_function_t)(int event, int old_state, int *next_state, void* param);

/**
 * @brief Function called when we exit a specific state
*/
typedef void (*statemachine_exit_function_t)(int event, int old_state, int *next_state, void* param);

/**
 * @brief Function called when we switch between different events
*/
typedef void (*event_function_t)(int event, int old_state, int *next_state, void* param);

/**
 * @brief Structure containing specific event submission metadata
*/
typedef struct event_submission{
    event_function_t event_cb_function;
    void * cb_param_data;
    int event_id;

    int next_state;
    bool active;

}event_submission_t;

/*
*/
#define NUM_EVENTS(x) sizeof(x)/sizeof(event_submission_t)

/**
 * @brief Stucture containing all state specifc data for handling statemachine
*/
typedef struct statemachine_state
{
    int state;
    statemachine_entry_function_t entry_function;
    statemachine_exit_function_t exit_function;
    int num_events;
    event_submission_t *events_list;
} statemachine_state_t;

#define NUM_STATES(x) sizeof(x) /sizeof(statemachine_state_t)

/**
 * @brief Statemachine handler containing all data for entire statemachine
*/
typedef struct statemachine{
    int current_state;
    int latest_event;
    int num_states;
    int num_events;
    statemachine_state_t *states_list;
}statemachine_t;

/**
 * @brief Generates a new statemachine
*/
statemachine_t *init_new_statemachine(const int num_states, const int num_events, const int init_state, statemachine_state_t *states_list);

/**
 * @brief Submits event to existing statemachine
*/
int statemachine_submit_event(statemachine_t *statemachine, int event, void *params);

/**
 * @brief Without using events, we can switch states
 * @note still calls entry and exit functions for states
*/
int statemachine_set_state(statemachine_t *statemachine, int next_state, void *param);

#endif