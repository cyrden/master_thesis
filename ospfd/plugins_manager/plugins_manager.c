//
// Created by cyril on 15/01/19.
//

/*
 * The plugin manager will receive messages from a user interface and react accordingly by injecting plugins at specific positions.
 * For the moment the communication is not implemented and thus the plugin manager just loads plugins as wished by the programmer.
 */

#include "plugins_manager.h"

#include "../plugins/plugins.h"
#include "lib/log.h"


struct mesg_buffer {
    long mesg_type;
    char mesg_text[SIZE_MESG];
} message;

/*
 * Initialization of the structure that contains all the plugins
 */
int plugins_tab_init(plugins_tab_t *tab) {
    tab = malloc(sizeof(plugins_tab_t));
    for (int i = 0; i < MAX_NBR_PLUGINS; i++) {
        tab->plugins[i] = NULL;
    }
    return 1;
}

/*
 * Injects a plugin (file elfname) at a specific position (identified by the id)
 */
static int inject_plugins(plugins_tab_t *tab, int id, const char *elfname) {
    if(id < 1 || id > MAX_NBR_PLUGINS-1) {
        fprintf(stderr, "Id not valid \n");
        return -1;
    }
    if(tab->plugins[id] != NULL) {
        fprintf(stderr, "There is already a loaded plugin at this position \n");
        return -1;
    }
    tab->plugins[id] = load_elf_file(elfname);
    if (tab->plugins[id] == NULL) {
        perror("Failed to load file\n");
        return EXIT_FAILURE;
    }
    printf("Plugin %s injected at position: %d \n", elfname, id);
    return 1;
}

/*
 * Releases ressources of all the loaded plugins
 */
void release_all_plugins(void) {
    for(int i = 0; i < MAX_NBR_PLUGINS; i++) {
        if(plugins_tab.plugins[i] != NULL) {
            release_elf(plugins_tab.plugins[i]);
        }
    }
}

/*
 * In charge of handling messages received from user and activating asked plugins
 */
void *plugins_manager(void *tab) {
    key_t key;
    int msgid;

    // ftok to generate unique key
    key = ftok("/home/router/ospfd/plugins/plugins.h", 64);
    if(key==-1) {
        printf("receiver key error:  \n");
        return 0;
    }

    // msgget creates a message queue and returns identifier (or returns identifier to existing queue of same key if already exists)
    msgid = msgget(key, 0666 | IPC_CREAT);
    if(msgid==-1) {
        printf("receiver msgget error:  \n");
        return 0;
    }
    inject_plugins((plugins_tab_t *) tab, TEST, "/home/router/ospfd/plugins/test_plugin.o"); // Injects the plugin at position TEST (beginning of main)
    inject_plugins((plugins_tab_t *) tab, RCV_PACKET, "/home/router/ospfd/plugins/rcv_packet.o");
    inject_plugins((plugins_tab_t *) tab, SEND_HELLO, "/home/router/ospfd/plugins/hello_count.o");
    inject_plugins((plugins_tab_t *) tab, SPF_CALC, "/home/router/ospfd/plugins/spf_time.o");
    inject_plugins((plugins_tab_t *) tab, SEND_PACKET, "/home/router/ospfd/plugins/send_packet.o");
    inject_plugins((plugins_tab_t *) tab, LSA_FLOOD, "/home/router/ospfd/plugins/lsa_flood.o");
    inject_plugins((plugins_tab_t *) tab, ISM_CHANGE_STATE, "/home/router/ospfd/plugins/ism_change_state.o");
    /*while(1) {
        printf("Wait for message \n");
        if (msgrcv(msgid, &message, sizeof(message), 0, 0) != -1) { // blocking call
            printf("msg_rcveid \n");
            // TODO: check that it is a valid location etc
            if(message.mesg_type == 100) { // Just to be able to inject everything in one time for testing
                inject_plugins((plugins_tab_t *) tab, TEST, "/home/router/ospfd/plugins/test_plugin.o"); // Injects the plugin at position TEST (beginning of main)
                inject_plugins((plugins_tab_t *) tab, RCV_PACKET, "/home/router/ospfd/plugins/rcv_packet.o");
                inject_plugins((plugins_tab_t *) tab, SEND_HELLO, "/home/router/ospfd/plugins/hello_count.o");
                inject_plugins((plugins_tab_t *) tab, SPF_CALC, "/home/router/ospfd/plugins/spf_time.o");
                inject_plugins((plugins_tab_t *) tab, SEND_PACKET, "/home/router/ospfd/plugins/send_packet.o");
                inject_plugins((plugins_tab_t *) tab, LSA_FLOOD, "/home/router/ospfd/plugins/lsa_flood.o");
                inject_plugins((plugins_tab_t *) tab, ISM_CHANGE_STATE, "/home/router/ospfd/plugins/ism_change_state.o");
            }
            else {
                printf("compare: \n");
                printf("%s : %d \n", message.mesg_text, (int) strlen(message.mesg_text));
                printf("%s : %d \n", "/home/router/ospfd/plugins/send_packet.o", (int) strlen("/home/router/ospfd/plugins/send_packet.o"));
                printf("strcmp: %d \n",strcmp(message.mesg_text, "/home/router/ospfd/plugins/send_packet.o"));
                inject_plugins((plugins_tab_t *) tab, (int) message.mesg_type, (const char *) message.mesg_text);
                if(plugins_tab.plugins[SEND_PACKET] == NULL) {
                    printf("SEND PACKET NULL \n");
                }
                else {
                    printf("SEND PACKET NOT NULL !!! \n");
                }
            }
        }
        else {
            if(errno == EINTR) {
                perror("EINTR");
                sleep(1);
                continue;
            }
            perror("error ");
            printf("Error while receiving message \n");
            return NULL;
        }
    }*/
}

