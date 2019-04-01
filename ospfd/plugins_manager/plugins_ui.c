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
    char strpos[10];
    long position;
    while(1) { // Todo, check if valid values for inputs & deal with string inputs instead of IDs
        printf("--> Please enter the path to your BPF file \n");
        fgets(path, 50, stdin);
        path[strlen(path) - 1] = '\0'; // Otherwise it is a \n and the path doesn't work ...

        printf("--> Please enter the insertion point where you want to inject the plugin (as a int) \n");
        fgets(strloc, 10, stdin);
        location = strtol(strloc, NULL, 10);

        printf("--> Please enter the position where you want to inject the bytecode (as a int: 0 = PRE, 1 = REP, 2 = POST) \n");
        fgets(strpos, 10, stdin);
        position = strtol(strpos, NULL, 10);

        message.mesg_type = location*100+position;
        memcpy((void *) message.mesg_text, (void *) path, 50*sizeof(char));
        if(msgsnd(msgid, &message, sizeof(message), 0) != -1) {
            printf("--> Plugin at location %s sent to plugins_manager at location %d, position %d \n", message.mesg_text, (int) location, (int) position);
        }
        else {
            printf("Error while sending message \n");
            return 0;
        }
    }
}