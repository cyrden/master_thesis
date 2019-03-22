#include "plugins.h"

/*
 * Dummy plugin to test the MAIN_PRE type of message
 */

uint64_t my_super_complicated_function(void *val) {
    return send_data(MAIN_PRE, val);
}
