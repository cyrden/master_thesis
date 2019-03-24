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
 * Possible insertion points for plugins
 */
#define MAIN 1
#define ISM_CHANGE_STATE_PRE 2
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



/* Structures to pass as argument to plugins */

struct arg_plugin_hello_send {
    struct ospf_interface *oi;
    struct plugin_context *plugin_context; // pointer to the context of the associated plugin
};


uint64_t send_data(int type, void *txt);

int read_int(void *pointer);

void set_pointer_toInt(void *pointer, int value);

int interface_get_count_speed(struct plugin_context *plugin_context, struct hello_struct *s);

#endif //OSPF_PLUGINS_API_H
