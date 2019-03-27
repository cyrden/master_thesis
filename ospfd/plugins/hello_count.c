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
    struct ospf_interface *oi = plugin_arg->oi; // argument, it is a copy
    struct ospf_interface oi_read;
    int ret = get_ospf_interface(plugin_arg->plugin_context, &oi_read);
    if(ret != 1) return 0; // Error in the external function
    struct interface ifp;
    int ret2 = get_interface(plugin_arg->plugin_context, &ifp);
    if(ret2 != 1) return 0; // Error in the external function
    s.hello_count = oi_read.hello_out;
    s.itf_speed = ifp.speed;
    return send_data(SEND_HELLO_PRE, &s);
}