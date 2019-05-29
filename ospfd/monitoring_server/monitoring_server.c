//
// Created by cyril on 17/11/18.
//

/*
 * This file implements a server that reads messages in a kernel queue. Messages are put by plugins. It then treats them in some ways that depends of the type of plugin.
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
    char mesg_text[SIZE_MESG];
} message;


int main(int argc, char **argv) {
    key_t key;
    int msgid;

    // ftok to generate unique key
    key = ftok("/etc/frr/daemons", 65);
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
            struct hello_struct *hello_struct;
            //struct ospf_lsa *lsa;
            struct lsa_header *lsah;
            struct spf_mon *spf_mon;
            struct ism_change_state *ism;
            switch (message.mesg_type) {
                case MAIN:
                    t = (struct test *) message.mesg_text;
                    printf("[MAIN]: %d %ld \n", t->a, t->b);
                    break;
                case RCV_PACKET:
                    s = (struct stream *) message.mesg_text;
                    printf("[RCV_PACKET]: \n");
                    my_ospf_packet_dump(s);
                    break;
                case SEND_HELLO:
                    hello_struct = (struct hello_struct *) message.mesg_text;
                    printf("[HELLO_COUNT]: %d, itf_speed: %d \n", hello_struct->hello_count, hello_struct->itf_speed);
                    break;
                case SPF_CALC:
                    spf_mon = (struct spf_mon *) message.mesg_text;
                    printf("[SPF_CALC_POST]: # spf calculation: %d, time spent calculation of Dijstra for area %s: %ld usec \n",
                            spf_mon->spf_count, inet_ntoa(spf_mon->area_id), spf_mon->time_spf);
                    break;
                case SEND_PACKET:
                    s = (struct stream *) message.mesg_text;
                    printf("[SEND_PACKET]: \n");
                    my_ospf_packet_dump(s);
                    break;
                case LSA_FLOOD:
                    lsah = (struct lsa_header *) message.mesg_text;
                    printf("[LSA_FLOOD_PRE]: \n");
                    my_ospf_lsa_header_dump(lsah);
                    break;
                case ISM_CHANGE_STATE:
                    ism = (struct ism_change_state *) message.mesg_text;
                    printf("[ISM_CHANGE_STATE_PRE] : Interface: %s old: %s, new: %s \n",
                            ism->oi_name, lookup_msg(ospf_ism_state_msg, ism->old_state, NULL), lookup_msg(ospf_ism_state_msg, ism->new_state, NULL));
                    break;
                case SPF_LSA:
                    lsah = (struct lsa_header *) message.mesg_text;
                    printf("[MY_SUPER_LSA]: \n");
                    my_ospf_lsa_header_dump(lsah);
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
    // to destroy the message queue (never reached for the moment)
    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}


