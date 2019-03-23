//
// Created by cyril on 17/11/18.
//

#ifndef PROJECT_PLUGINS_H
#define PROJECT_PLUGINS_H

#include <sys/time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>

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

#include <lib/version.h>
#include "getopt.h"
#include "thread.h"
#include "linklist.h"
#include "if.h"
#include "vector.h"
#include "vty.h"
#include "command.h"
#include "filter.h"
#include "plist.h"
#include "stream.h"
#include "log.h"
#include "memory.h"
#include "memory_vty.h"
#include "privs.h"
#include "sigevent.h"
#include "zclient.h"
#include "vrf.h"
#include "libfrr.h"

#include "../../ubpf/tools/ospf_plugins_api.h"
#include "../monitoring_server/monitoring_server.h"


/*
 * Possible insertion points for plugins TODO: It is probably a bad idea to list it here. Indeed, plugins.h is a user defined file and the insertion points must be defined in the implementation
 */
#define MAIN 1
#define ISM_CHANGE_STATE_PRE 2 // Change state (adjacency formation) - Interface state machine
#define ISM_CHANGE_STATE_POST 3
#define SEND_HELLO_PRE 4
#define SEND_HELLO_POST 5
#define SPF_CALC_PRE 6
#define SPF_CALC_POST 7
#define SEND_PACKET 8
#define RCV_PACKET 9
#define LSA_FLOOD_PRE 10
#define LSA_FLOOD_POST 11

#define SPF_TEST 13

/*
 * Possible types of arguments
 */
#define OSPF_INTERFACE 1


/*
 * Structure with the context needed by plugin spf_time
 */
typedef struct spf_mon {
    int spf_count;
    long time_spf;
    struct in_addr area_id;
} spf_mon_t;

typedef struct flood_ctxt {
    struct lsa_header lsah;
    struct router_lsa rlsa;
} flood_ctxt_t;

typedef struct ism_change_state_ctxt {
    int old_state;
    int new_state;
    char oi_name[50];
} ism_change_state_ctxt_t;

#endif //PROJECT_PLUGINS_H
