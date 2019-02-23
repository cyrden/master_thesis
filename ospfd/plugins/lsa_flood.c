//
// Created by cyril on 16/02/19.
//

#include "plugins.h"

uint64_t lsa_flood(void *data) {
    return send_data(LSA_FLOOD_PRE, data);
}