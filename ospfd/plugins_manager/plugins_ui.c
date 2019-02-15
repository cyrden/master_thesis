//
// Created by cyril on 19/01/19.
//

/*
 * FOR THE MOMENT THIS FILE IS USELESS, I DIDN'T IMPLEMENTED THE USER INTERFACE (IT SHOULD BE RELATIVELY EASY ...)
 */

#include "../plugins/plugins.h"
#include "plugins_manager.h"

struct mesg_buffer {
    long mesg_type;
    char mesg_text[SIZE_MESG];
} message;

/*
 * CLI user interface. Prints the available plugins (or locations where he can inject ?) and describe how the user has to input.
 * Sends messages to the plugins manager accordingly to what user asked for.
 */
int main(int argc, char **argv) { // TODO: communicate with user and send appropriate message
    key_t key;
    int msgid; // message queue identifier
    // ftok to generate unique key
    key = ftok("/home/router/ospfd/plugins/plugins.h", 65);
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

    //message.mesg_type = ;
    //message.mesg_text = ;
    //msgsnd(msgid, &message, sizeof(message), 0);
}