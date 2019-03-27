//
// Created by cyril on 20/02/19.
//

#include "plugins.h"

uint64_t ism_change_state(void *data) {
    struct arg_plugin_ism_change_state *plugin_arg = (struct arg_plugin_ism_change_state *) data;
    struct ism_change_state s;
    struct ospf_interface oi;
    if(get_ospf_interface(plugin_arg->plugin_context, &oi) != 1) return 0; // Error in external function
    s.new_state = plugin_arg->new_state; // can be accessed directly
    s.old_state = oi.state;
    strcpy(s.oi_name, ospf_if_name_string(&oi));
    return send_data(ISM_CHANGE_STATE_PRE, (void *) &s);
}