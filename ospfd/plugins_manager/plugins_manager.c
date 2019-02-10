//
// Created by cyril on 15/01/19.
//

/*
 * The plugin manager will receive messages from a user interface and react accordingly by injecting plugins at specific positions.
 * For the moment the communication is not implemented and thus the plugin manager just loads plugins as wished by the programmer.
 */

#include "plugins_manager.h"

#include "../plugins/plugins.h"

struct mesg_buffer {
    long mesg_type;
    char mesg_text[SIZE_BUFFER];
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
    tab->plugins[id] = load_elf_file(elfname);
    if (tab->plugins[id] == NULL) {
        perror("Failed to load file\n");
        return EXIT_FAILURE;
    }
    return 1;
}

/*
 * In charge of handling messages received from user and activating asked plugins
 */
void *plugins_manager(void *tab) {
    key_t key;
    int msgid;

    // ftok to generate unique key
    key = ftok("/home/router/ospfd/plugins/plugins.h", 65);
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
    /*while(1) {
        // TODO: here handle messages received from UI
    }*/
    // TODO: This is temporary, plugins are injected "hardcoded"
    inject_plugins((plugins_tab_t *) tab, TEST, "/home/router/ospfd/plugins/test_plugin.o"); // Injects the plugin at position TEST (beginning of main)
    inject_plugins((plugins_tab_t *) tab, RCV_PACKET, "/home/router/ospfd/plugins/rcv_packet.o");
    inject_plugins((plugins_tab_t *) tab, SEND_HELLO, "/home/router/ospfd/plugins/hello_count.o");

    return NULL;
}

