#include "plugins.h"

/*
 * Dummy plugin to test the MAIN type of message
 */
uint64_t my_super_complicated_function(void *val) {
    struct test *te = (struct test *) val;
    te->a++;
    return send_data(MAIN, te);
}
