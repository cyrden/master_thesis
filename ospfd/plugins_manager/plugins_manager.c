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
    if(tab == NULL) return 0;
    for (int i = 0; i < MAX_NBR_PLUGINS; i++) {
        tab->plugins[i] = NULL; // create an empty tab of plugins
    }
    return 1;
}

/*
 * Injects a plugin (file elfname) at a specific position (identified by the id)
 */
static int inject_plugins(plugins_tab_t *tab, int id, const char *elfname, int pos) { // TODO: I should rename it inject_pluglet to be consistent ...
    if(id < 0 || id > MAX_NBR_PLUGINS-1) {
        fprintf(stderr, "Id not valid \n");
        return 0;
    }
    if(tab->plugins[id] == NULL) { // First pluglet injected
        tab->plugins[id] = (plugin_t *) malloc(sizeof(plugin_t));
        if (!tab->plugins[id]) {
            return 0;
        }
        tab->plugins[id]->pluglets[PRE] = NULL;
        tab->plugins[id]->pluglets[REP] = NULL;
        tab->plugins[id]->pluglets[POST] = NULL;
        tab->plugins[id]->shared_heap = NULL;
    }

    /*
     * This malloc pluglet. Set pluglet context to NULL. Create the vm of the pluglet. Register functions to the vm. Load code into the VM.
     * So we are ready to execute. Still need to set the context of the pluglet before running it.
     */
    if(load_elf_file(tab->plugins[id], elfname, pos) == NULL) return 0;

    if (tab->plugins[id]->pluglets[pos] == NULL) {
        perror("Failed to load file\n");
        return 0;
    }
    if(tab->plugins[id]->pluglets[pos]->pluglet_context == NULL) {
        tab->plugins[id]->pluglets[pos]->pluglet_context = malloc(sizeof(pluglet_context_t));
        tab->plugins[id]->pluglets[pos]->pluglet_context->original_arg = NULL;
        tab->plugins[id]->pluglets[pos]->pluglet_context->type_arg = -1; // error value
        tab->plugins[id]->pluglets[pos]->pluglet_context->heap = NULL;
        tab->plugins[id]->pluglets[pos]->pluglet_context->parent_plugin = tab->plugins[id];
    }
    return 1;
}

/*
 * Releases resources of all the loaded plugins
 */
void release_all_plugins(void) {
    for(int i = 0; i < MAX_NBR_PLUGINS; i++) {
        if(plugins_tab.plugins[i] != NULL) {
            release_elf(plugins_tab.plugins[i], PRE);
            release_elf(plugins_tab.plugins[i], REP);
            release_elf(plugins_tab.plugins[i], POST);
            free(plugins_tab.plugins[i]->shared_heap);
            free(plugins_tab.plugins[i]);
            plugins_tab.plugins[i] = NULL;
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
    inject_plugins((plugins_tab_t *) tab, MAIN, "/plugins/test_plugin.o", PRE);
    //inject_plugins((plugins_tab_t *) tab, RCV_PACKET, "/plugins/rcv_packet.o"); //TODO: Broken
    inject_plugins((plugins_tab_t *) tab, SEND_HELLO, "/plugins/hello_count.o", PRE);
    inject_plugins((plugins_tab_t *) tab, SPF_CALC, "/plugins/spf_time.o", PRE);
    inject_plugins((plugins_tab_t *) tab, SPF_CALC, "/plugins/spf_time_post.o", POST);
    //inject_plugins((plugins_tab_t *) tab, SEND_PACKET, "/plugins/send_packet.o"); // TODO: Broken
    //inject_plugins((plugins_tab_t *) tab, LSA_FLOOD, "/plugins/lsa_flood.o", PRE);
    inject_plugins((plugins_tab_t *) tab, ISM_CHANGE_STATE, "/plugins/ism_change_state.o", PRE);
    //inject_plugins((plugins_tab_t *) tab, SPF_LSA, "/plugins/originate_my_lsa.o", PRE);

    /*while(1) { // In that loop receives messages from UI to inject plugins
        printf("Wait for message \n");
        if (msgrcv(msgid, &message, sizeof(message), 0, 0) != -1) { // blocking call
            // TODO: check that it is a valid location etc
            int loc;
            int pos;
            pos = ((int) message.mesg_type) % 100;
            loc = (((int) message.mesg_type) - pos) / 100;
            if(!inject_plugins((plugins_tab_t *) tab, loc, (const char *) message.mesg_text, pos)) {
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

