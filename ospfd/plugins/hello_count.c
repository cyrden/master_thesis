//
// Created by cyril on 30/11/18.
//

#include "plugins.h"

/*
 *
 */
uint64_t hello_count(void *data) {
    struct arg_plugin_hello_send *plugin_arg = (struct arg_plugin_hello_send *) data;
    struct hello_struct *s1 = my_malloc(sizeof(struct hello_struct));
    struct ospf_interface *oi_read = my_malloc(sizeof(struct ospf_interface));
    int ret = get_ospf_interface(oi_read);
    if(ret != 1) return 0;
    struct interface *ifp = my_malloc(sizeof(struct interface));
    int ret2 = get_interface(ifp);
    if(ret2 != 1) return 0; // Error in the external function
    s1->hello_count = oi_read->hello_out;
    s1->itf_speed = ifp->speed;
    my_free(oi_read);
    my_free(ifp);
    return send_data(SEND_HELLO, s1);
}