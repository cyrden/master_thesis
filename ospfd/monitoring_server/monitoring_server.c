//
// Created by cyril on 17/11/18.
//

/*
 * This file implements a server that reads messages in a queue. Messages are put by plugins. It then treats them in some ways that depends of the type of plugin.
 * The communication is highly inspired from https://www.geeksforgeeks.org/ipc-using-message-queues/
 */

#include "defaults.h" // avoid autoconf error

#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../plugins/plugins.h"

#include "prefix.h"
#include "ospfd/ospfd.h"
#include "ospfd/ospf_asbr.h"
#include "ospfd/ospf_lsa.h"
#include "ospfd/ospf_packet.h"
#include "ospfd/ospf_lsdb.h"
#include "ospfd/ospf_neighbor.h"
#include "ospfd/ospf_dump.h"

#include "monitoring_server.h"
#include "print_helpers.h"


struct mesg_buffer {
    long mesg_type;
    char mesg_text[MESG_TXT_SIZE];
} message;


int main(int argc, char **argv) {
    key_t key;
    int msgid;

    // ftok to generate unique key
    key = ftok("/home/router/ubpf/tools/helpers.h", 65);
    if(key==-1) {
        printf("receiver key error: \n");
        return 0;
    }

    // msgget creates a message queue and returns identifier (or returns identifier to existing queue of same key if already exists)
    msgid = msgget(key, 0666 | IPC_CREAT);
    if(msgid==-1) {
        printf("receiver msgget error: \n");
        return 0;
    }
    while(1) {
        // msgrcv to receive message
        if(msgrcv(msgid, &message, sizeof(message), 0, 0) != -1) { // blocking call --> good for us
            struct test *t;
            struct stream *s;
            unsigned long *hello_count;
            spf_mon_t *spf_mon;
            switch (message.mesg_type) {
                case TEST:
                    t = (struct test *) message.mesg_text;
                    printf("[TEST]: %d %ld \n", t->a, t->b);
                    break;
                case RCV_PACKET:
                    s = (struct stream *) message.mesg_text;
                    printf("[RCV_PACKET]: \n");
                    my_ospf_packet_dump(s);
                    break;
                case SEND_HELLO:
                    hello_count = (unsigned long *) message.mesg_text;
                    printf("[HELLO_COUNT]: %ld \n", *hello_count);
                    break;
                case SPF_TIME:
                    spf_mon = (spf_mon_t *) message.mesg_text;
                    printf("[SPF_TIME]: # spf calculation: %d, time spent calculation of Dijstra for area %s: %ld usec \n",
                            spf_mon->spf_count, inet_ntoa(spf_mon->area_id), spf_mon->time_spf);
                    break;
                default:
                    printf("[ERROR]: message has no valid type: %ld \n", message.mesg_type);
            }
        }
        else {
            printf("Error while receiving message:  \n");
            return 0;
        }
    }
    /* TODO: Never reached for the moment, need to delete the queue in signal handler or something else */
    // to destroy the message queue (never reached for the moment)
    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}


