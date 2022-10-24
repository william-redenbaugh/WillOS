#include "event_management.h"
#include "event_type_list.h"
#include "EventQueue.hpp"
#include "OS/OSMutexKernel.h"
#include "OS/OSThreadKernel.h"
#include "OS/error.hpp"

#define NUM_THREADS 2

static EventQueue global_eventspace;
static bool event_management_init = false;

static EventQueue *local_eventspaces[NUM_THREADS];
static event_type_t **subscribed_eventspaces;
static int num_subscribed_events[NUM_THREADS];

int num_eventspaces = 0;

static MutexLock event_management_mutx;

void event_management_thread(void *parameters){

    // Allow up to 48 events in the global workspace
    global_eventspace.init(48);

    subscribed_eventspaces = (event_type_t**)malloc(sizeof(event_type_t*) * NUM_THREADS);
    memset(num_subscribed_events, 0, sizeof(num_subscribed_events));
    event_management_init = true;

    for(;;){
        event_data_t event;
        event = global_eventspace.pop();

        // Go through all the local event spaces, publish to whoever is subscribed to them
        event_management_mutx.lockWaitIndefinite();
        for(int n = 0; n < num_eventspaces; n++){
            for(int i = 0; i < num_subscribed_events[n]; i++){
                if(subscribed_eventspaces[n][i] == event.event){
                    local_eventspaces[n]->push(event);
                }
            }
        }
        event_management_mutx.unlock();
    }
}

int publish_event(event_type_t event, void *ptr){
    if(!event_management_init){
        return MK_INT_ERR;
    }
    event_data_t data;
    data.event = event;
    data.ptr = ptr;
    int ret = global_eventspace.push(data);
    switch(ret){
        case false:
            return MK_LOW_MEM_ERR;
        case true:
            return MK_OK;
        default:
            return MK_INT_ERR;
    }
}

int subscribe_eventlist(event_type_t *event_list, int num_events, int event_length_max){
    if(!event_management_init){
        return MK_INT_ERR;
    }

    event_management_mutx.lockWaitIndefinite();
    local_eventspaces[num_eventspaces] = new EventQueue;
    subscribed_eventspaces[num_eventspaces] = (event_type_t*)malloc(sizeof(event_type_t) * num_events);

    // Add to list of events
    for(int n = 0; n < num_events; n++){
        subscribed_eventspaces[num_eventspaces][n] = event_list[n];
    }
    num_subscribed_events[num_eventspaces] = num_events;
    int ret = num_eventspaces;
    num_eventspaces++;
    event_management_mutx.unlock();
    return ret;
}

event_data_t consume_event(int eventspace){
    event_data_t event;
    memset(&event, 0, sizeof(event_data_t));

    if(!event_management_init){
        return event;
    }
    event = local_eventspaces[eventspace]->pop();

    return event;
}
