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
#include "ospfd/ospf_flood.h"

#include "pqueue.h"
#include "linklist.h"
#include "plist.h"

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
#define OSPF_SPF_NEXT 15


/* Structures to pass as argument to plugins */

#define ARG_PLUGIN_HELLO_SEND 0
struct arg_plugin_hello_send {
    heap_t heap;
    struct ospf_interface *oi;

};

#define ARG_PLUGIN_ISM_CHANGE_STATE 1
struct arg_plugin_ism_change_state {
    heap_t heap;
    struct ospf_interface *oi;
    int new_state;
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

#define ARG_PLUGIN_OSPF_SPF_NEXT 4
struct arg_plugin_ospf_spf_next {
    heap_t heap;
    struct vertex *v;
    struct ospf *ospf;
    struct ospf_area *area;
    struct pqueue *candidate;
};

void print_helper(int a);

//void my_print(const char *format, ...);

void lsa_head_dump (struct lsa_header *lsah);

uint16_t plugin_ntohs(uint16_t value);

uint32_t plugin_ntohl(uint32_t value);

int ospf_lsa_has_link(struct lsa_header *w, struct lsa_header *v);

unsigned int my_ospf_nexthop_calculation(struct arg_plugin_ospf_spf_next *s, struct vertex *w,
                                      struct router_lsa_link *l,
                                      unsigned int distance, int lsa_pos);

struct vertex *ospf_vertex_new(struct ospf_lsa *lsa);

uint64_t send_data(int type, void *txt);

/* Getters */

int get_ospf_interface(struct ospf_interface *oi, struct ospf_interface *oi_copy);

int get_interface(struct interface *ifp, struct interface *ifp_copy);

int get_ospf_interface_list(struct list *oilist, struct ospf_interface **oilist_copy, struct ospf_interface **oi_list_addresses);

int get_ospf_lsa(struct ospf_lsa *lsa, struct ospf_lsa *lsa_copy);

int get_lsa_header(struct lsa_header *lsah, struct lsa_header *lsah_copy);

int get_lsa_with_length(struct router_lsa *lsa, struct router_lsa *lsa_copy);

int get_ospf_area(struct ospf_area *area, struct ospf_area *area_copy);

int get_ospf(struct ospf *ospf, struct ospf *ospf_copy);

int get_vertex(struct vertex *vertex, struct vertex *vertex_copy);

int get_candidate(struct pqueue *candidate, int stat, struct vertex *vertex);

/* Setters */

int set_ospf_interface(struct ospf_interface *oi, struct ospf_interface *oi_copy);

int set_ospf_area_transit(struct ospf_area *area, uint8_t transit);

int my_get_lsah(struct ospf_lsa *lsa, struct lsa_header *lsah);

/* OSPF functions */

struct ospf_lsa *my_ospf_lsa_install(struct ospf *ospf, struct ospf_interface *oi, struct ospf_lsa *lsa);

int my_ospf_flood_through_area(struct ospf_area *area, struct ospf_neighbor *inbr, struct ospf_lsa *lsa);

int my_lsa_link_broadcast_set(struct stream **s, struct ospf_interface *oi, uint32_t metric);

struct ospf_lsa *ospf_my_lsa_new_and_data(struct stream *s, struct ospf_area *area);

#endif //OSPF_PLUGINS_API_H
