//
// Created by cyril on 17/11/18.
//

#ifndef PROJECT_PLUGINS_H
#define PROJECT_PLUGINS_H

#include <sys/time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../ubpf/tools/helpers.h"


/*
 * Plugins type (in fact it will probably be locations in the code where plugins can be injected)
 */
#define HELLO_COUNT 1
#define TEST 2
#define SEND_HELLO 4
#define SPF_COUNT 5
#define SEND_LSA 6
#define RCV_LSA 7
#define RCV_PACKET 8

#endif //PROJECT_PLUGINS_H
