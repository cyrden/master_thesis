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
#define TEST 2 // TEST location (very beginning of the main)
#define SEND_HELLO 4 // when sending hello (pre of
#define SPF_CALC 5
#define SEND_PACKET 6
#define LSA_FLOOD 7
#define RCV_PACKET 8 // In

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

#endif //PROJECT_PLUGINS_H
