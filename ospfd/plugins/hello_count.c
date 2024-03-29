//
// Created by cyril on 30/11/18.
//

#include "plugins.h"
/*
 * Plugin that monitors the number of OSPF hello packets sent (+ itf speed & name)
 */
uint64_t hello_count(void *data) {
    struct arg_plugin_hello_send *plugin_arg = (struct arg_plugin_hello_send *) data;
    struct hello_struct *s1 = plugin_malloc(sizeof(struct hello_struct));
    if(s1 == NULL) return 0;
    struct ospf_interface *oi_read = plugin_malloc(sizeof(struct ospf_interface));
    if(oi_read == NULL) return 0;
    int ret = get_ospf_interface(plugin_arg->oi, oi_read);
    if(ret != 1) return 0;
    struct interface *ifp = plugin_malloc(sizeof(struct interface));
    if(ifp == NULL) return 0;
    int ret2 = get_interface(oi_read->ifp, ifp);
    if(ret2 != 1) return 0; // Error in the external function
    s1->hello_count = oi_read->hello_out;
    s1->itf_speed = ifp->speed;
    plugin_free(oi_read);
    plugin_free(ifp);
    plugin_free(s1);
    return send_data(SEND_HELLO, s1);
}