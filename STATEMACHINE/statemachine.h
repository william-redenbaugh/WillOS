#ifndef _STATEMACHINE_H
#define _STATEMACHINE_H

typedef void (*statemachine_entry_function_t)(void);
typedef void (*statemachine_exit_function_t)(void);
typedef struct statemachine_state
{
    int state;
    statemachine_entry_function_t entry_function;
    statemachine_exit_function_t exit_function;
} statemachine_state_t;

#endif