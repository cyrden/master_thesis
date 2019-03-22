//
// Created by cyril on 19/01/19.
//

#include "../plugins/plugins.h"
#include "plugins_manager.h"

struct mesg_buffer {
    long mesg_type;
    char mesg_text[SIZE_MESG];
} message;

/*
 * CLI user interface. Allow the user to load a BPF file as a plugin at a position he choose in the code.
 * Sends messages to the plugins manager accordingly to what user asked for.
 */
int main(int argc, char **argv) {
    key_t key;
    int msgid; // message queue identifier
    // ftok to generate unique key
    key = ftok("/etc/frr/daemons", 64);
    if (key == -1) {
        printf("sender key error t \n");
        return 0;
    }

    // msgget creates a message queue and returns identifier of this queue
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        printf("sender msgget error \n");
        return 0;
    }
    char path[50];
    long location;
    char strloc[10];
    while(1) { // Todo, check if valid values for inputs & deal with string inputs instead of IDs
        printf("Please enter the path to your BPF file \n");
        fgets(path, 50, stdin);
        printf("Please enter the location where you want to inject the plugin (as a int) \n");
        fgets(strloc, 10, stdin);
        path[strlen(path) - 1] = '\0'; // Otherwise it is a \n and the path doesn't work ...
        location = strtol(strloc, NULL, 10);
        message.mesg_type = location;
        memcpy((void *) message.mesg_text, (void *) path, 50*sizeof(char));
        if(msgsnd(msgid, &message, sizeof(message), 0) != -1) {
            printf("Plugin at location %s sent to plugins_manager at position %d \n", message.mesg_text, (int) message.mesg_type);
        }
        else {
            printf("Error while sending message \n");
            return 0;
        }
    }
}