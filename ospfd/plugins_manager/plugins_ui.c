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
 * CLI user interface. Allow the user to load BPF files as plugins at a positions he chooses in the code.
 * Sends messages to the plugins manager accordingly to what user asked for.
 * User must provide a manifesto (.txt file formatted as plugins/input_file.txt)
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
    fp = fopen(path, "r");
    if(fp == NULL) {
        printf("fopen error : Cannot open the file (does not exist ?)\n");
        return 0;
    }
    char line[100];
    while(fgets (line, 100, fp) != NULL) {
        int i = 0;
        char filename[100];
        long insertion_point = -1;
        long anchor = -1;
        char *ptr = strtok(line, " ");
        while(ptr != NULL)
        {
            i++;
            if(i == 1) {
                strcpy(filename, ptr);
                printf("%s \n", ptr);
            }
            else if(i == 2) {
                insertion_point = strtol(ptr, NULL, 10);
                printf("%ld \n", insertion_point);
            }
            else if(i == 3) {
                anchor = strtol(ptr, NULL, 10);
                printf("%ld \n", anchor);
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
        message.mesg_type = insertion_point*100+anchor;
        strcpy((void *) message.mesg_text, (void *) filename);
        if(msgsnd(msgid, &message, sizeof(message), 0) != -1) {
            printf("--> Plugin at insertion_point %s sent to plugins_manager at insertion_point %d, anchor %d \n", message.mesg_text, (int) insertion_point, (int) anchor);
        }
        else {
            printf("Error while sending message \n");
            return 0;
        }
    }
    fclose(fp);
    return 1;
}