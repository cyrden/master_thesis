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
#include "../../ubpf/tools/helpers.h"

#include "defaults.h" // avoid autoconf error
#include "prefix.h"
#include "ospfd/ospfd.h"
#include "ospfd/ospf_asbr.h"
#include "ospfd/ospf_lsa.h"
#include "ospfd/ospf_packet.h"
#include "ospfd/ospf_lsdb.h"
#include "ospfd/ospf_neighbor.h"
#include "ospfd/ospf_dump.h"


/*
 * Possible locations to inject plugins
 */
#define TEST 1 // TEST location (very beginning of the main)
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
