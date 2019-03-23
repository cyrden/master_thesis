//
// Created by cyril on 17/11/18.
//

#ifndef PROJECT_MONITORING_SERVER_H
#define PROJECT_MONITORING_SERVER_H

#include "../plugins_manager/plugins_manager.h"

/*
 * Structures used in plugins to send data to the monitoring server
 */
typedef struct test {
    int a;
    long b;
}test;

struct hello_struct{
    int hello_count;
    int itf_speed;
};


#endif //PROJECT_MONITORING_SERVER_H
