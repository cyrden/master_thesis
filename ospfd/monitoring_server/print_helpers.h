//
// Created by cyril on 06/02/19.
//

#ifndef FRR_THESIS_PRINT_HELPERS_H
#define FRR_THESIS_PRINT_HELPERS_H

#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>

#include "prefix.h"
#include "ospfd/ospfd.h"
#include "ospfd/ospf_asbr.h"
#include "ospfd/ospf_lsa.h"
#include "ospfd/ospf_packet.h"
#include "ospfd/ospf_lsdb.h"
#include "ospfd/ospf_neighbor.h"
#include "ospfd/ospf_dump.h"

void my_ospf_packet_dump(struct stream *s);

void my_ospf_header_dump(struct ospf_header *ospfh);

#endif //FRR_THESIS_PRINT_HELPERS_H
