//
// Created by cyril on 28/03/19.
//

#include "plugins.h"

uint64_t spf_count(void *data) {
    struct arg_plugin_spf_calc *plugin_arg = (struct arg_plugin_spf_calc *) data;
    struct timeval t1, t2;
    gettimeofday (&t2, NULL);
    shared_heap_get(plugin_arg->plugin_context, &t1, sizeof(struct timeval));
    struct spf_mon mon;
    mon.time_spf = ((t2.tv_sec - t1.tv_sec) * 1000000 + t2.tv_usec) - t1.tv_usec;
    struct ospf_area area;
    if(get_ospf_area(plugin_arg->plugin_context, &area) != 1) return 0;
    mon.area_id = area.area_id;
    mon.spf_count = area.spf_calculation;
    shared_heap_free(plugin_arg->plugin_context);
    return send_data(SPF_CALC, (void *) &mon);
}