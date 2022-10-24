
#ifndef _LED_MANAGEMENT_RUNTIME_H
#define _LED_MANAGEMENT_RUNTIME_H

#include "Arduino.h"

/**
 * @brief LED management thread
 *
 * @param params
 * @note only to be called by threads_init.h
 */
void led_management_thread(void *params);

#endif