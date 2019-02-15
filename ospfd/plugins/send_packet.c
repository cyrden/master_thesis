//
// Created by cyril on 14/02/19.
//


#include "plugins.h"

uint64_t send_packet(void *data) {
    return send_data(SEND_PACKET, data);
}