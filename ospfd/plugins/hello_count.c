//
// Created by cyril on 30/11/18.
//

#include "plugins.h"

/*
 *
 */
uint64_t hello_count(void *data) {
    //struct plugin_arg *plugin_arg = (struct plugin_arg *) data;
    struct hello_struct s;
    //struct ospf_interface *arg = (struct ospf_interface *) plugin_arg->argument;
    struct ospf_interface *arg = (struct ospf_interface *) data;
    s.hello_count = arg->hello_out;
    //interface_get_speed(plugin_arg->plugin_context, arg, &s.itf_speed);
    interface_get_speed(arg, &s.itf_speed);
    return send_data(SEND_HELLO_PRE, &s);
}