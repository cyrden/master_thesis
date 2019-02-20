//
// Created by cyril on 20/02/19.
//

#include "plugins.h"

uint64_t ism_change_state(void *data) {
    return send_data(ISM_CHANGE_STATE, data);
}