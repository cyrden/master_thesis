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
 * Initialization of the structure that contains all the contexts
 */
int contexts_tab_init(contexts_tab_t *tab) {
    tab = malloc(sizeof(contexts_tab_t));
    if(tab == NULL) return 0;
    for (int i = 0; i < MAX_NBR_PLUGINS; i++) {
        tab->contexts[i] = NULL;
    }
    return 1;
}

/*
 * Initialization of the structure that contains all the plugins
 */
int plugins_tab_init(plugins_tab_t *tab) {
    tab = malloc(sizeof(plugins_tab_t));
    if(tab == NULL) return 0;
    for (int i = 0; i < MAX_NBR_PLUGINS; i++) {
        tab->plugins[i] = NULL;
    }
    return 1;
}

/*
 * Injects a plugin (file elfname) at a specific position (identified by the id)
 */
static int inject_plugins(plugins_tab_t *tab, int id, const char *elfname, int pos) {
    if(id < 0 || id > MAX_NBR_PLUGINS-1) {
        fprintf(stderr, "Id not valid \n");
        return 0;
    }
    if(tab->plugins[id] == NULL) { // First BPF code injected
        tab->plugins[id] = (plugin_t *)malloc(sizeof(plugin_t));
        if (!tab->plugins[id]) {
            return 0;
        }
        tab->plugins[id]->vm[PRE] = NULL;
        tab->plugins[id]->vm[REP] = NULL;
        tab->plugins[id]->vm[POST] = NULL;
    }
    if(load_elf_file(tab->plugins[id], elfname, pos) == NULL) return 0;
    if (tab->plugins[id] == NULL) {
        perror("Failed to load file\n");
        return 0;
    }
    tab->plugins[id]->plugin_context = malloc(sizeof(struct plugin_context));
    tab->plugins[id]->plugin_context->heap = NULL;
    contexts_tab.contexts[id] = tab->plugins[id]->plugin_context; // Store the pointer to every context created
    return 1;
}

/*
 * Releases resources of all the loaded plugins
 */
void release_all_plugins(void) {
    for(int i = 0; i < MAX_NBR_PLUGINS; i++) {
        if(plugins_tab.plugins[i] != NULL) {
            free(plugins_tab.plugins[i]->plugin_context);
            release_elf(plugins_tab.plugins[i], PRE);
            release_elf(plugins_tab.plugins[i], REP);
            release_elf(plugins_tab.plugins[i], POST);
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
    //inject_plugins((plugins_tab_t *) tab, SPF_CALC, "/plugins/spf_time.o", PRE);
    //inject_plugins((plugins_tab_t *) tab, SPF_CALC, "/plugins/spf_time_post.o", POST);
    //inject_plugins((plugins_tab_t *) tab, SEND_PACKET, "/plugins/send_packet.o"); // TODO: Broken
    inject_plugins((plugins_tab_t *) tab, LSA_FLOOD, "/plugins/lsa_flood.o", PRE);
    inject_plugins((plugins_tab_t *) tab, ISM_CHANGE_STATE, "/plugins/ism_change_state.o", PRE);

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

