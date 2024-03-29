//
// Created by cyril on 04/01/19.
//

#include "plugins.h"

/* Bytecode to store the current time in the heap with an ID (useful for measuring execution time) */
uint64_t spf_count(void *data) {
    struct arg_plugin_spf_calc *plugin_arg = (struct arg_plugin_spf_calc *) data;
    struct timeval *t1 = plugin_malloc_with_id(3, sizeof(struct timeval));
    gettimeofday(t1, NULL);
    return 1;
}