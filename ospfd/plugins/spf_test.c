//
// Created by cyril on 06/03/19.
//

#include "plugins.h"

uint64_t spf_test(void *data) {
    int spf_calculations = read_int(data);
    if(spf_calculations == -1) return 0;
    spf_calculations++;
    set_pointer_toInt(data, spf_calculations);
    return 1;
}