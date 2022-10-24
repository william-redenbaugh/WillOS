#ifndef _EVENT_MANAGEMENT_H
#define _EVENT_MANAGEMENT_H
#include "event_type_list.h"

typedef struct{
    void *ptr;
    event_type_t event;
}event_data_t;

#define EVENT_PEEK_TIMEOUT 0

/**
 * @brief Publish an event to the local eventspace
 *
 * @param event Enumerated type of event
 * @param ptr Random state pointer to be passed between threads
 * @return int
 */
int publish_event(event_type_t event, void *ptr);

/**
 * @brief Thread that will handle all of our event management stuff.
 *
 * @param parameters
 * @note To be handled by our threads_init
 */
void event_management_thread(void *parameters);

/**
 * @brief Subscribe to a list oe
 *
 * @param event_list List event
 * @param num_events How many events are you subscribing to
 * @param event_length_max How many events do you want your queue to fit
 * @return int mk_err status if an error. Otherwise provides the index of the event_subscribe list. Keep that so that you can consume events that are published. Check out error.h to see more information
 * @note 1 Thread can only subscribe to a single list, implementation limitation
 */
int subscribe_eventlist(event_type_t *event_list, int num_events, int event_length_max);

/**
 * @brief Checks to see if there are any events in the currently selected local eventspace
 *
 * @param eventspace
 * @return true there are events
 * @return false there are no events, or there was an internal error
 */
bool available_events(int eventspace);

/**
 * @brief Sit and wait for events to come into an eventspace
 *
 * @param eventspace local eventspace index
 * @return event_data_t struct with all our event_data
 * @note Will return the EVENT_TYPE_NONE if there was no actual event returned
 */
event_data_t consume_event(int eventspace);
#endif