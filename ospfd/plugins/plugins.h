//
// Created by cyril on 17/11/18.
//

#ifndef PROJECT_PLUGINS_H
#define PROJECT_PLUGINS_H

#include <sys/time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "../../ubpf/tools/helpers.h"


/*
 * Possible locations to inject plugins
 */
#define TEST 2 // TEST location (very beginning of the main)
#define SEND_HELLO 4 // when sending hello (pre of
#define SPF_TIME 5
#define SEND_LSA 6
#define RCV_LSA 7
#define RCV_PACKET 8 // In

/*
 * Structure with th context needed by plugin spf_time
 */
typedef struct spf_mon {
    int spf_count;
    long time_spf;
    struct in_addr area_id;
} spf_mon_t;

#endif //PROJECT_PLUGINS_H
