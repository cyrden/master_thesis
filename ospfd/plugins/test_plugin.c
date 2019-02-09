#include "plugins.h"

/*
 * Dummy plugin to test the TEST type of message
 */

uint64_t my_super_complicated_function(void *val) {
    return send_data(TEST, val);
}
