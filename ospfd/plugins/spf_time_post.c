//
// Created by cyril on 28/03/19.
//

#include "plugins.h"

/* Gets a previous time in the heap and computes the execution time */
uint64_t spf_count(void *data) {
    struct arg_plugin_spf_calc *plugin_arg = (struct arg_plugin_spf_calc *) data;
    struct timeval t2;
    gettimeofday (&t2, NULL);
    struct timeval *t1 = (struct timeval *) plugin_get_memory_with_id(3);
    struct spf_mon mon;
    mon.time_spf = ((t2.tv_sec - t1->tv_sec) * 1000000 + t2.tv_usec) - t1->tv_usec;
    struct ospf_area area;
    if(get_ospf_area(plugin_arg->area, &area) != 1) return 0;
    mon.area_id = area.area_id;
    mon.spf_count = area.spf_calculation;
    plugin_free(t1);
    return send_data(SPF_CALC, (void *) &mon);
}