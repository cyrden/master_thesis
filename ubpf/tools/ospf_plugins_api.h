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


uint64_t send_data(int type, void *txt);

int read_int(void *pointer);

void set_pointer_toInt(void *pointer, int value);

//int interface_get_speed(struct plugin_context *plugin_context, struct ospf_interface *oi, int *speed);
int interface_get_speed(struct ospf_interface *oi, int *speed);

#endif //OSPF_PLUGINS_API_H
