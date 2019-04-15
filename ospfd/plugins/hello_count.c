//
// Created by cyril on 30/11/18.
//

#include "plugins.h"

/*
 *
 */
uint64_t hello_count(void *data) {
    struct arg_plugin_hello_send *plugin_arg = (struct arg_plugin_hello_send *) data;
    /* TODO: can't let the user do this + even if I do it in an init method, how to ensure the user will not change these ? (Make crash ...)*/
    plugin_arg->heap.heap_start = &plugin_arg->heap.mem;
    plugin_arg->heap.heap_end = &plugin_arg->heap.mem;
    plugin_arg->heap.heap_last_block = NULL;

    struct hello_struct *s1 = my_malloc(&plugin_arg->heap, sizeof(struct hello_struct));
    // TODO: check what makes it fail
    struct hello_struct s;
    struct ospf_interface *oi = plugin_arg->oi; // argument, it is a copy
    struct ospf_interface oi_read;
    int ret = get_ospf_interface(&oi_read);
    if(ret != 1) return 0; // Error in the external function
    struct interface ifp;
    int ret2 = get_interface(&ifp);
    if(ret2 != 1) return 0; // Error in the external function
    s1->hello_count = oi_read.hello_out;
    s1->itf_speed = ifp.speed;
    return send_data(SEND_HELLO, s1);
}