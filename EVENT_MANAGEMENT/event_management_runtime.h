
#ifndef _EVENT_MANAGEMENT_RUNTIME_H
#define _EVENT_MANAGEMENT_RUNTIME_H

/**
 * @brief LED management thread
 *
 * @param params
 * @note only to be called by threads_init.h
 */
void event_management_thread(void *params);

#endif