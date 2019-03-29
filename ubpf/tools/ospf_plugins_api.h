//
// Created by cyril on 30/11/18.
//

#ifndef OSPF_PLUGINS_API_H
#define OSPF_PLUGINS_API_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#include "log.h"

#include "defaults.h" // avoid autoconf error
#include "prefix.h"
#include "ospfd/ospfd.h"
#include "ospfd/ospf_interface.h"
#include "ospfd/ospf_packet.h"
#include "ospfd/ospf_asbr.h"
#include "ospfd/ospf_lsa.h"
#include "ospfd/ospf_lsdb.h"
#include "ospfd/ospf_neighbor.h"
#include "ospfd/ospf_dump.h"
#include "ospfd/ospf_route.h"
#include "ospfd/ospf_zebra.h"
#include "ospfd/ospf_vty.h"
#include "ospfd/ospf_errors.h"

#include "ospfd/monitoring_server/monitoring_server.h"

/*
 * PRE, POST, REPLACE
 */
#define PRE 0
#define REP 1
#define POST 2

/*
 * Possible insertion points for plugins
 */
#define MAIN 1
#define ISM_CHANGE_STATE 2
#define SEND_HELLO 4
#define SPF_CALC 6
#define SEND_PACKET 8
#define RCV_PACKET 9
#define LSA_FLOOD 10
#define SPF_TEST 13


/* Structures to pass as argument to plugins */

#define ARG_PLUGIN_HELLO_SEND 0
struct arg_plugin_hello_send {
    struct plugin_context *plugin_context; // pointer to the context of the associated plugin // TODO: maybe use it as a int. But doesn't work, don't know why
    struct ospf_interface *oi;
};

#define ARG_PLUGIN_ISM_CHANGE_STATE 1
struct arg_plugin_ism_change_state {
    struct plugin_context *plugin_context;
    struct ospf_interface *oi;
    int new_state;
};

#define ARG_PLUGIN_LSA_FLOOD 2
struct arg_plugin_lsa_flood {
    struct plugin_context *plugin_context;
    struct ospf_lsa *lsa;
};

#define ARG_PLUGIN_SPF_CALC 3
struct arg_plugin_spf_calc {
    struct plugin_context *plugin_context;
    struct ospf_area *area;
};


int heap_malloc(struct plugin_context *plugin_context, size_t size);

int heap_free(struct plugin_context *plugin_context);

int heap_get(struct plugin_context *plugin_context, void *heap_copy, size_t size);

int heap_set(struct plugin_context *plugin_context, void *val, size_t size);

uint64_t send_data(int type, void *txt);

int read_int(void *pointer);

void set_pointer_toInt(void *pointer, int value);

int get_ospf_interface(struct plugin_context *plugin_context, struct ospf_interface *oi);

int get_interface(struct plugin_context *plugin_context, struct interface *ifp);

int get_ospf_lsa(struct plugin_context *plugin_context, struct ospf_lsa *lsa);

int get_lsa_header(struct plugin_context *plugin_context, struct lsa_header *lsah);

int get_ospf_area(struct plugin_context *plugin_context, struct ospf_area *area);

#endif //OSPF_PLUGINS_API_H
