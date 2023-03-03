#ifndef _STATEMACHINE_H
#define _STATEMACHINE_H

#include "OS/error.hpp"

typedef void (*statemachine_entry_function_t)(void*);
typedef void (*statemachine_exit_function_t)(void*);

typedef void (*event_function_t)(void*);


typedef struct event_submission{
    event_function_t event_cb_function;
    void * cb_param_data;
    int event_id;
    int next_state;
    bool active;

}event_submission_t;

typedef struct statemachine_state
{
    int state;
    statemachine_entry_function_t entry_function;
    statemachine_exit_function_t exit_function;
    void *exit_function_param;
    void *entry_function_param;

    int num_events;
    event_submission_t *events_list;
} statemachine_state_t;

typedef struct statemachine{
    int current_state;
    int latest_event;
    int num_states;
    int num_events;
    statemachine_state_t *states_list;
}statemachine_t;


statemachine_t *init_new_statemachine(const int num_states, const int num_events, const statemachine_state_t *states_list);
#endif