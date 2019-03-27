//
// Created by cyril on 20/02/19.
//

#include "plugins.h"

uint64_t ism_change_state(void *data) {
    struct arg_plugin_ism_change_state *plugin_arg = (struct arg_plugin_ism_change_state *) data;
    struct ism_change_state s;
    struct ospf_interface *oi = plugin_arg->oi;
    //memcpy(&s.oi_name, ospf_if_name_string(oi), 50*sizeof(char));
    s.new_state = plugin_arg->new_state;
    //s.old_state = oi->state; make plugin crash, unallowed memory. need getter
    return send_data(ISM_CHANGE_STATE_PRE, (void *) &s);
}