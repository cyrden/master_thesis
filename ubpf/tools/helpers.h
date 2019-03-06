//
// Created by cyril on 30/11/18.
//

#ifndef PROJECT_HELPERS_H
#define PROJECT_HELPERS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>


uint64_t inc_hello_count(void);

uint64_t send_data(int type, void *txt);

int read_int(void *pointer);

void set_pointer_toInt(void *pointer, int value);

#endif //PROJECT_HELPERS_H
