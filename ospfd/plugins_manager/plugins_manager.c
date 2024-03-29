//
// Created by cyril on 15/01/19.
//

/*
 * The plugin manager will receive messages from a user interface and react accordingly by injecting plugins at specific positions.
 */

#include "plugins_manager.h"

#include "../plugins/plugins.h"
#include "lib/log.h"

/* Buffer for the communication with the user (injection of plugins) */
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
 * Injects a pluglet (file elfname) at a specific insertion point (identified by the id) and anchor
 */
static int inject_pluglet(plugins_tab_t *tab, int id, const char *elfname, int anchor) {
    if(id < 0 || id > MAX_NBR_PLUGINS-1) {
        fprintf(stderr, "Id not valid \n");
        return 0;
    }
    if(tab->plugins[id] == NULL) { // First pluglet injected at this insertion point, we need to create the plugin
        tab->plugins[id] = (plugin_t *) malloc(sizeof(plugin_t));
        if (!tab->plugins[id]) {
            return 0;
        }
        tab->plugins[id]->pluglet_REP = NULL;
        for(int i = 0; i < MAX_NBR_PLUGLETS; i++) {
            tab->plugins[id]->pluglets_PRE[i] = NULL;
            tab->plugins[id]->pluglets_POST[i] = NULL;
        }
        tab->plugins[id]->heap = NULL;
        tab->plugins[id]->type_arg = -1;
        tab->plugins[id]->arguments = NULL;
    }

    pluglet_t *created_pluglet = NULL;
    if(anchor == PRE) {
        int i;
        for(i = 0; i < MAX_NBR_PLUGLETS; i++) {
            if(tab->plugins[id]->pluglets_PRE[i] == NULL){
                tab->plugins[id]->pluglets_PRE[i] = malloc(sizeof(pluglet_t));
                created_pluglet = tab->plugins[id]->pluglets_PRE[i];
                break;
            }
        }
        if(i == 10) {
            fprintf(stderr, "Error: There are already too many pluglets in PRE mode \n");
            return 0;
        }
    }
    else if(anchor == REP) {
        if(tab->plugins[id]->pluglet_REP != NULL) {
            fprintf(stderr, "Error, there is already a pluglet in REP mode \n");
            return 0;
        }
        tab->plugins[id]->pluglet_REP = malloc(sizeof(pluglet_t));
        created_pluglet = tab->plugins[id]->pluglet_REP;
    }
    else if(anchor == POST) {
        int i;
        for(i = 0; i < MAX_NBR_PLUGLETS; i++) {
            if(tab->plugins[id]->pluglets_POST[i] == NULL){
                tab->plugins[id]->pluglets_POST[i] = malloc(sizeof(pluglet_t));
                created_pluglet = tab->plugins[id]->pluglets_POST[i];
                break;
            }
        }
        if(i == 10) {
            fprintf(stderr, "Error: There are already too many pluglets in POST mode \n");
            return 0;
        }
    }
    else {
        fprintf(stderr, "Error, wrong position to inject a pluglet \n");
        return 0;
    }

    if(created_pluglet == NULL) {
        fprintf(stderr, "Failed to create the pluglet \n");
        return 0;
    }

    /*
     * Set pluglet context to NULL. Create the vm of the pluglet. Register functions to the vm. Load code into the VM.
     * So we are ready to execute. Still need to set the context of the pluglet before running it.
     */
    if(load_elf_file(created_pluglet, elfname) == NULL) return 0;

    /* Init of the pluglet context */
    if(created_pluglet->pluglet_context == NULL) {
        created_pluglet->pluglet_context = malloc(sizeof(pluglet_context_t));
        created_pluglet->pluglet_context->original_arg = NULL;
        created_pluglet->pluglet_context->type_arg = -1; // error value
        created_pluglet->pluglet_context->heap = NULL;
        created_pluglet->pluglet_context->parent_plugin = tab->plugins[id];
    }
    return 1;
}

/*
 * Releases resources of all the loaded plugins
 */
void release_all_plugins(void) {
    for(int i = 0; i < MAX_NBR_PLUGINS; i++) {
        if(plugins_tab.plugins[i] != NULL) {
            if(plugins_tab.plugins[i]->arguments != NULL) {
                free(plugins_tab.plugins[i]->arguments);
            }
            release_elf(plugins_tab.plugins[i]->pluglet_REP);
            for(int j = 0; j < MAX_NBR_PLUGLETS; j++) {
                release_elf(plugins_tab.plugins[i]->pluglets_PRE[j]);
                release_elf(plugins_tab.plugins[i]->pluglets_POST[j]);
            }
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
    //inject_pluglet((plugins_tab_t *) tab, MAIN, "/plugins/test_plugin.o", PRE);
    //inject_pluglet((plugins_tab_t *) tab, RCV_PACKET, "/plugins/rcv_packet.o", PRE);
    //inject_pluglet((plugins_tab_t *) tab, SEND_HELLO, "/plugins/hello_count.o", PRE);
    //inject_pluglet((plugins_tab_t *) tab, SPF_CALC, "/plugins/spf_time.o", PRE);
    //inject_pluglet((plugins_tab_t *) tab, SPF_CALC, "/plugins/spf_time_post.o", POST);
    //inject_pluglet((plugins_tab_t *) tab, SEND_PACKET, "/plugins/send_packet.o", PRE);
    //inject_pluglet((plugins_tab_t *) tab, LSA_FLOOD, "/plugins/lsa_flood.o", PRE);
    //inject_pluglet((plugins_tab_t *) tab, ISM_CHANGE_STATE, "/plugins/ism_change_state.o", PRE);
    //inject_pluglet((plugins_tab_t *) tab, SPF_CALC, "/plugins/originate_my_lsa.o", PRE);
    //inject_pluglet((plugins_tab_t *) tab, OSPF_SPF_NEXT, "/plugins/ospf_spf_next.o", REP);

    while(1) { // In that loop receives messages from CLI to inject plugins
        printf("Wait for message \n");
        if (msgrcv(msgid, &message, sizeof(message), 0, 0) != -1) { // blocking call
            int insertion_point;
            int anchor;
            anchor = ((int) message.mesg_type) % 100;
            insertion_point = (((int) message.mesg_type) - anchor) / 100;
            if(!inject_pluglet((plugins_tab_t *) tab, insertion_point, (const char *) message.mesg_text, anchor)) {
                zlog_notice("Failed to inject plugin \n");
            }
        }
        else {
            if(errno == EINTR) { // Because FRR is sending signals that are making msgrcv to fail ... This solution is not perfect though
                perror("EINTR");
                sleep(1);
                continue;
            }
            perror("error ");
            zlog_notice("Error while receiving message \n");
            return NULL;
        }
    }
    return NULL;
}

