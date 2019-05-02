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
    char path[100];
    printf("--> Please enter the path to your input file \n");
    fgets(path, 100, stdin);
    path[strlen(path) - 1] = '\0'; // Otherwise it is a \n and the path doesn't work ...
    printf("Path: %s \n", path);
    FILE *fp;
    printf("before fopen \n");
    fp = fopen(path, "r");
    if(fp == NULL) {
        printf("fopen error \n");
        return 0;
    }
    printf("after fopen \n");
    char line[100];
    while(fgets (line, 100, fp) != NULL) {
        int i = 0;
        char filename[100];
        long location = -1;
        long position = -1;
        char *ptr = strtok(line, " ");
        while(ptr != NULL)
        {
            i++;
            if(i == 1) {
                strcpy(filename, ptr);
                printf("%s \n", ptr);
            }
            else if(i == 2) {
                location = strtol(ptr, NULL, 10);
                printf("%ld \n", location);
            }
            else if(i == 3) {
                position = strtol(ptr, NULL, 10);
                printf("%ld \n", position);
            }
            else {
                printf("Error \n");
                return 0;
            }
            ptr = strtok(NULL, " ");
        }
        if(i != 3) {
            printf("Invalid line \n");
            return 0;
        }
        message.mesg_type = location*100+position;
        strcpy((void *) message.mesg_text, (void *) filename);
        if(msgsnd(msgid, &message, sizeof(message), 0) != -1) {
            printf("--> Plugin at location %s sent to plugins_manager at location %d, position %d \n", message.mesg_text, (int) location, (int) position);
        }
        else {
            printf("Error while sending message \n");
            return 0;
        }
    }
    fclose(fp);
    return 1;
}