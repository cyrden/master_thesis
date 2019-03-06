//
// Created by cyril on 30/11/18.
//

/*
 * Helper functions are registered to the eBPF vm on creation to be used as external functions
 */

#include "helpers.h"
#include "../../ospfd/plugins_manager/plugins_manager.h"

/*
 * Struct of the message buffer stored in kernel (used to comunicate with monitoring server)
 */
struct mesg_buffer {
    long mesg_type;
    char mesg_text[SIZE_MESG];
} message;

uint64_t hello_count = 0; // The counter is kept in the global variables area

/*
 * Increments the hello counter
 */
uint64_t inc_hello_count(void) {
    hello_count++;
    return hello_count;
}

/*
 * This function is used to put messages in the queue.
 * The arguments are the type of the plugin and a void* that contains the data the plugin wants to pass to the server (usually a structure specific to the plugin).
 */
uint64_t send_data(int type, void *data) {

    key_t key;
    int msgid; // message queue identifier
    // ftok to generate unique key
    key = ftok("/home/router/ubpf/tools/helpers.h", 65);
    if (key == -1) {
        printf("sender key error t \n");
        return 0;
    }

    // msgget creates a message queue and returns identifier of this queue
    msgid = msgget(key, 0);
    if (msgid == -1) {
        printf("sender msgget error \n");
        return 0;
    }

    message.mesg_type = type;
    memcpy((void *) message.mesg_text, data, SIZE_MESG*sizeof(char)); //copy the data to the message
    msgsnd(msgid, &message, sizeof(message), 0);

    return 0;
}

int read_int(void *pointer) {
    if(pointer == NULL) return -1;
    int *read = (int *) pointer;
    return *read;
}

void set_pointer_toInt(void *pointer, int value) {
    *((int *) pointer) = value;
}