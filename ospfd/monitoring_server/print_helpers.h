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

struct my_lsa {
    struct lsa_header header;
    uint8_t flags;
    uint8_t zero;
    uint16_t links;
    struct {
        struct in_addr link_id;
        struct in_addr link_data;
        uint8_t type;
        uint8_t tos;
        uint16_t metric;
        uint32_t color;
    } link[3];
};

void my_ospf_packet_dump(struct stream *s);

void my_ospf_header_dump(struct ospf_header *ospfh);

void my_ospf_lsa_header_dump(struct lsa_header *lsah);

void test_print_router_lsa(struct router_lsa *rl);


#endif //FRR_THESIS_PRINT_HELPERS_H
