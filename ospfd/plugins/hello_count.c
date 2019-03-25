//
// Created by cyril on 30/11/18.
//

#include "plugins.h"

/*
 *
 */
uint64_t hello_count(void *data) {
    struct arg_plugin_hello_send *plugin_arg = (struct arg_plugin_hello_send *) data;
    struct hello_struct s;
    struct ospf_interface *oi = plugin_arg->oi;
    int ret = interface_get_count_speed(plugin_arg->plugin_context, &s);
    if(ret != 1) return 0; // Error in the external function
    return send_data(SEND_HELLO_PRE, &s);
}