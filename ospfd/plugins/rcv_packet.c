//
// Created by cyril on 04/02/19.
//

#include "plugins.h"

uint64_t rcv_packet(void *data) {
    return send_data(RCV_PACKET, data);
}