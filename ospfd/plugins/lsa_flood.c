//
// Created by cyril on 16/02/19.
//

#include "plugins.h"

uint64_t lsa_flood(void *data) {
    struct arg_plugin_lsa_flood *plugin_arg = (struct arg_plugin_lsa_flood *) data;
    struct ospf_lsa lsa;
    if(get_ospf_lsa(&lsa) != 1) return 0; // Error in external function
    struct lsa_header lsah;
    if(get_lsa_header(&lsah) != 1) return 0; // Error in external function
    // I could do something with de lsa here.
    return send_data(LSA_FLOOD, (void *) &lsah);
}