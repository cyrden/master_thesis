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
        return 0;
    }
    if(tab->plugins[id] != NULL) {
        fprintf(stderr, "There is already a loaded plugin at this position \n");
        return 0;
    }
    tab->plugins[id] = load_elf_file(elfname);
    if (tab->plugins[id] == NULL) {
        perror("Failed to load file\n");
        return 0;
    }
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
    key = ftok("/etc/frr/daemons", 64);
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
    // TODO: The following lines will be removed. Just used for debugging purposes
    //inject_plugins((plugins_tab_t *) tab, SPF_TEST, "/plugins/spf_test.o");
    inject_plugins((plugins_tab_t *) tab, MAIN_PRE, "/plugins/test_plugin.o"); // Injects the plugin at position MAIN_PRE (beginning of main)
    //inject_plugins((plugins_tab_t *) tab, RCV_PACKET, "/plugins/rcv_packet.o");
    //inject_plugins((plugins_tab_t *) tab, SEND_HELLO_PRE, "/plugins/hello_count.o");
    //inject_plugins((plugins_tab_t *) tab, SPF_CALC_POST, "/plugins/spf_time.o");
    //inject_plugins((plugins_tab_t *) tab, SEND_PACKET, "/plugins/send_packet.o");
    //inject_plugins((plugins_tab_t *) tab, LSA_FLOOD_PRE, "/plugins/lsa_flood.o");
    //inject_plugins((plugins_tab_t *) tab, ISM_CHANGE_STATE_PRE, "/plugins/ism_change_state.o");

    /*while(1) { // In that loop receives messages from UI to inject plugins
        printf("Wait for message \n");
        if (msgrcv(msgid, &message, sizeof(message), 0, 0) != -1) { // blocking call
            // TODO: check that it is a valid location etc
            if(!inject_plugins((plugins_tab_t *) tab, (int) message.mesg_type, (const char *) message.mesg_text)) {
                printf("Failed to inject plugin \n");
            }
        }
        else {
            if(errno == EINTR) { // Because FRR is sending signals that are making msgrcv to fail ...
                perror("EINTR");
                sleep(1);
                continue;
            }
            perror("error ");
            printf("Error while receiving message \n");
            return NULL;
        }
    }*/
    return NULL;
}
