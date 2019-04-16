//
// Created by cyril on 20/02/19.
//

#include "plugins.h"

uint64_t ism_change_state(void *data) {
    struct arg_plugin_ism_change_state *plugin_arg = (struct arg_plugin_ism_change_state *) data;
    struct ism_change_state *s = my_malloc(sizeof(struct ism_change_state));
    struct ospf_interface *oi = my_malloc(sizeof(struct ospf_interface));
    if(get_ospf_interface(oi) != 1) return 0;
    s->new_state = plugin_arg->new_state; // can be accessed directly
    s->old_state = oi->state;
    strcpy(s->oi_name, ospf_if_name_string(oi));
    my_free(oi);
    return send_data(ISM_CHANGE_STATE, (void *) s);
}