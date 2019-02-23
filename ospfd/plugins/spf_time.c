//
// Created by cyril on 04/01/19.
//

#include "plugins.h"

uint64_t spf_count(void *data) {
    return send_data(SPF_CALC_POST, data);
}