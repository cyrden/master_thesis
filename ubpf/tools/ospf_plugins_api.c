//
// Created by cyril on 30/11/18.
//

/*
 * Helper functions are registered to the eBPF vm on creation to be used as external functions
 */

#include "ospf_plugins_api.h"
#include "../../ospfd/plugins_manager/plugins_manager.h"

/*
 * Struct of the message buffer stored in kernel (used to communicate with monitoring server)
 */
struct mesg_buffer {
    long mesg_type;
    char mesg_text[SIZE_MESG];
} message;

static int check_context_validity(plugin_context_t *context) {
    if(context == NULL) return 0;
    for(int i = 0; i < MAX_NBR_PLUGINS; i++) {
        if(contexts_tab.contexts[i] != NULL && context == contexts_tab.contexts[i]) {
            /*
             * The pointer in argument corresponds to one of the context we allocated (user probably didn't cheat)
             * If we are here, we know the pointer to the context (trusted part) has not been dereferenced. We also know that a user could only dereference this pointer,
             * he could not dereference pointers inside because it would make the eBPF VM crash (unallowed memory access)
             */
            return 1;
        }
    }
    return 0;
}

int heap_malloc(struct plugin_context *plugin_context, size_t size) {
    if(plugin_context == NULL) {
        printf("NULL pointer \n");
        return 0;
    }
    if(check_context_validity(plugin_context) != 1) {
        printf("The context is not valid \n");
        return 0;
    }
    plugin_context->heap = malloc(size);
    if(plugin_context->heap == NULL) return 0;
    return 1;
}

int heap_free(struct plugin_context *plugin_context) {
    if(plugin_context == NULL) {
        printf("NULL pointer \n");
        return 0;
    }
    if(check_context_validity(plugin_context) != 1) {
        printf("The context is not valid \n");
        return 0;
    }
    free(plugin_context->heap);
    plugin_context->heap = NULL;
    return 1;
}

int heap_get(struct plugin_context *plugin_context, void *heap_copy, size_t size) {
    if(plugin_context == NULL) {
        printf("NULL pointer \n");
        return 0;
    }
    if(check_context_validity(plugin_context) != 1) {
        printf("The context is not valid \n");
        return 0;
    }
    memcpy(heap_copy, plugin_context->heap, size);
    return 1;
}

int heap_set(struct plugin_context *plugin_context, void *val, size_t size) {
    if(plugin_context == NULL) {
        printf("NULL pointer \n");
        return 0;
    }
    if(check_context_validity(plugin_context) != 1) {
        printf("The context is not valid \n");
        return 0;
    }
    memcpy(plugin_context->heap, val, size);
    return 1;
}



/*
 * This function is used to put messages in the queue.
 * The arguments are the type of the plugin and a void* that contains the data the plugin wants to pass to the server (usually a structure specific to the plugin).
 */
uint64_t send_data(int type, void *data) {

    key_t key;
    int msgid; // message queue identifier
    // ftok to generate unique key
    key = ftok("/etc/frr/daemons", 65);
    if (key == -1) {
        printf("sender key error t \n");
        return 0;
    }

    // msgget creates a message queue and returns identifier of this queue
    msgid = msgget(key, 0);
    if (msgid == -1) {
        printf("sender msgget error \n");
        return 0;
    }

    message.mesg_type = type;
    if(data != NULL) {
        memcpy((void *) message.mesg_text, data, SIZE_MESG * sizeof(char)); //copy the data to the message
    }
    msgsnd(msgid, &message, sizeof(message), 0);

    return 0;
}

/*
 * Function to test reading OSPF variables from a plugin
 */
int read_int(void *pointer) {
    if(pointer == NULL) return -1;
    int *read = (int *) pointer;
    return *read;
}

/*
 * Function to test writing OSPF variables from a plugin
 */
void set_pointer_toInt(void *pointer, int value) {
    *((int *) pointer) = value;
}

/*
 * Getter function to get an ospf_interface.
 */
int get_ospf_interface(struct plugin_context *plugin_context, struct ospf_interface *oi) {
    if(plugin_context == NULL) { // check that plugin didn't send null pointer
        printf("NULL pointer \n");
        return 0;
    }
    if(check_context_validity(plugin_context) != 1) { // If the user changed the context pointer, we will detect it (otherwise it would segfault and crash OSPF process)
        printf("The context is not valid \n");
        return 0;
    }
    /* This switch is because depending on where the plugin that uses this helper function has been inserted, we need to cast to the good argument type */
    switch (plugin_context->type_arg) {
        case ARG_PLUGIN_HELLO_SEND:
            memcpy(oi, ((struct arg_plugin_hello_send *) plugin_context->original_arg)->oi, sizeof(struct ospf_interface));
            break;
        case ARG_PLUGIN_ISM_CHANGE_STATE:
            memcpy(oi, ((struct arg_plugin_ism_change_state *) plugin_context->original_arg)->oi, sizeof(struct ospf_interface));
            break;
        default:
            fprintf(stderr, "Argument type not recognized by helper function");
            return 0;
    }
    return 1;
}

/*
 * Getter function to get an interface.
 */
int get_interface(struct plugin_context *plugin_context, struct interface *ifp) {
    if(plugin_context == NULL) { // check that plugin didn't send null pointer
        printf("NULL pointer \n");
        return 0;
    }
    if(check_context_validity(plugin_context) != 1) { // If the user changed the context pointer, we will detect it (otherwise it would segfault and crash OSPF process)
        printf("The context is not valid \n");
        return 0;
    }
    /* This switch is because depending on where the plugin that uses this helper function has been inserted, we need to cast to the good argument type */
    switch (plugin_context->type_arg) {
        case ARG_PLUGIN_HELLO_SEND:
            memcpy(ifp, ((struct arg_plugin_hello_send *) plugin_context->original_arg)->oi->ifp, sizeof(struct interface));
            break;
        case ARG_PLUGIN_ISM_CHANGE_STATE:
            memcpy(ifp, ((struct arg_plugin_ism_change_state *) plugin_context->original_arg)->oi->ifp, sizeof(struct interface));
            break;
        default:
            fprintf(stderr, "Argument type not recognized by helper function");
            return 0;
    }
    return 1;
}

/*
 * Getter function to get an ospf_lsa
 */
int get_ospf_lsa(struct plugin_context *plugin_context, struct ospf_lsa *lsa) {
    if(plugin_context == NULL) { // check that plugin didn't send null pointer
        printf("NULL pointer \n");
        return 0;
    }
    if(check_context_validity(plugin_context) != 1) { // If the user changed the context pointer, we will detect it (otherwise it would segfault and crash OSPF process)
        printf("The context is not valid \n");
        return 0;
    }
    /* This switch is because depending on where the plugin that uses this helper function has been inserted, we need to cast to the good argument type */
    switch (plugin_context->type_arg) {
        case ARG_PLUGIN_LSA_FLOOD:
            memcpy(lsa, ((struct arg_plugin_lsa_flood *) plugin_context->original_arg)->lsa, sizeof(struct ospf_lsa));
            break;
        default:
            fprintf(stderr, "Argument type not recognized by helper function");
            return 0;
    }
    return 1;
}

/*
 * Getter function to get an lsa_header
 */
int get_lsa_header(struct plugin_context *plugin_context, struct lsa_header *lsah) {
    if(plugin_context == NULL) { // check that plugin didn't send null pointer
        printf("NULL pointer \n");
        return 0;
    }
    if(check_context_validity(plugin_context) != 1) { // If the user changed the context pointer, we will detect it (otherwise it would segfault and crash OSPF process)
        printf("The context is not valid \n");
        return 0;
    }
    /* This switch is because depending on where the plugin that uses this helper function has been inserted, we need to cast to the good argument type */
    switch (plugin_context->type_arg) {
        case ARG_PLUGIN_LSA_FLOOD:
            memcpy(lsah, ((struct arg_plugin_lsa_flood *) plugin_context->original_arg)->lsa->data, sizeof(struct lsa_header));
            break;
        default:
            fprintf(stderr, "Argument type not recognized by helper function");
            return 0;
    }
    return 1;
}

/*
 * Getter function to get an ospf area
 */
int get_ospf_area(struct plugin_context *plugin_context, struct ospf_area *area) {
    if(plugin_context == NULL) { // check that plugin didn't send null pointer
        printf("NULL pointer \n");
        return 0;
    }
    if(check_context_validity(plugin_context) != 1) { // If the user changed the context pointer, we will detect it (otherwise it would segfault and crash OSPF process)
        printf("The context is not valid \n");
        return 0;
    }
    /* This switch is because depending on where the plugin that uses this helper function has been inserted, we need to cast to the good argument type */
    switch (plugin_context->type_arg) {
        case ARG_PLUGIN_SPF_CALC:
            memcpy(area, ((struct arg_plugin_spf_calc *) plugin_context->original_arg)->area, sizeof(struct ospf_area));
            break;
        default:
            fprintf(stderr, "Argument type not recognized by helper function");
            return 0;
    }
    return 1;
}
