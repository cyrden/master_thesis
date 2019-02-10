//
// Created by cyril on 30/11/18.
//

#include "plugins.h"

/*
 * Dummy plugin that increments the hello_count sends it to the message queue and returns the number of hello messages sent by this router
 * Takes no arguments
 */
uint64_t hello_count(void) {
    unsigned long hello_count = inc_hello_count();
    send_data(SEND_HELLO, (void *) &hello_count); // send the hello count on the message queue
    return hello_count;
}