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
#include <unistd.h>
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
#define SPF_LSA 14


/* Structures to pass as argument to plugins */

#define ARG_PLUGIN_HELLO_SEND 0
struct arg_plugin_hello_send {
    heap_t heap;
    struct ospf_interface *oi;
};

#define ARG_PLUGIN_ISM_CHANGE_STATE 1
struct arg_plugin_ism_change_state {
    int new_state; // TODO: This is a hack ! Because if I put it as last arg, it might fail to access in the plugin. I think that the VM can access memory of pointer --> pointer+mem_len. So if malloc goes on block not successive it is a bad memory access ...
    heap_t heap;
    struct ospf_interface *oi;
};

#define ARG_PLUGIN_LSA_FLOOD 2
struct arg_plugin_lsa_flood {
    heap_t heap;
    struct ospf_lsa *lsa;
};

#define ARG_PLUGIN_SPF_CALC 3
struct arg_plugin_spf_calc {
    heap_t heap;
    struct ospf_area *area;
};


int shared_heap_malloc(size_t size);

int shared_heap_free(void);

int shared_heap_get(void *heap_copy, size_t size);

int shared_heap_set(void *val, size_t size);

uint64_t send_data(int type, void *txt);

int read_int(void *pointer);

void set_pointer_toInt(void *pointer, int value);

int get_ospf_interface(struct ospf_interface *oi);

int get_interface(struct interface *ifp);

int get_ospf_lsa(struct ospf_lsa *lsa);

int get_lsa_header(struct lsa_header *lsah);

int get_ospf_area(struct ospf_area *area);

int get_ospf(struct ospf *ospf);

struct ospf_lsa *ospf_my_lsa_new(struct ospf_area *area, uint8_t type);

int my_get_lsah(struct ospf_lsa *lsa, struct lsa_header *lsah);

#endif //OSPF_PLUGINS_API_H
