//
// Created by cyril on 04/01/19.
//

#include "plugins.h"

uint64_t spf_count(void *data) {
    struct arg_plugin_spf_calc *plugin_arg = (struct arg_plugin_spf_calc *) data;
    shared_heap_malloc(sizeof(struct timeval));
    struct timeval t1;
    gettimeofday (&t1, NULL);
    shared_heap_set((void *) &t1, sizeof(struct timeval));
    return 1;
}