#include "plugins.h"

/*
 * Dummy plugin to test if plugin injection and mon server works. If yes, monitoring server should print the message sent by this plugin on startup of OSPF
 */
uint64_t my_super_complicated_function(void *val) {
    struct test *te = (struct test *) val;
    te->a++;
    return send_data(MAIN, te);
}
