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

int shared_heap_malloc(size_t size) {
    plugin_context_t *plugin_context = current_context;
    if(size > MAX_SIZE_SHARED_HEAP) {
        printf("Size exceeds maximum allowed shared heap size \n");
        return 0;
    }
    if(plugin_context == NULL) {
        printf("NULL pointer \n");
        return 0;
    }
    plugin_context->shared_heap = malloc(size);
    if(plugin_context->shared_heap == NULL) return 0;
    return 1;
}

int shared_heap_free() {
    plugin_context_t *plugin_context = current_context;
    if(plugin_context == NULL) {
        printf("NULL pointer \n");
        return 0;
    }
    free(plugin_context->shared_heap);
    plugin_context->shared_heap = NULL;
    return 1;
}

int shared_heap_get(void *heap_copy, size_t size) {
    plugin_context_t * plugin_context = current_context;
    if(size > MAX_SIZE_SHARED_HEAP) {
        printf("Size exceeds maximum allowed shared heap size \n");
        return 0;
    }
    if(plugin_context == NULL) {
        printf("NULL pointer \n");
        return 0;
    }
    memcpy(heap_copy, plugin_context->shared_heap, size);
    return 1;
}

int shared_heap_set(void *val, size_t size) {
    plugin_context_t *plugin_context = current_context;
    if(size > MAX_SIZE_SHARED_HEAP) {
        printf("Size exceeds maximum allowed shared heap size \n");
        return 0;
    }
    if(plugin_context == NULL) {
        printf("NULL pointer \n");
        return 0;
    }
    memcpy(plugin_context->shared_heap, val, size);
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
int get_ospf_interface(struct ospf_interface *oi) {
    plugin_context_t *plugin_context = current_context;
    if(plugin_context == NULL) { // check that plugin didn't send null pointer
        printf("NULL pointer \n");
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
int get_interface(struct interface *ifp) {
    plugin_context_t *plugin_context = current_context;
    if(plugin_context == NULL) { // check that plugin didn't send null pointer
        printf("NULL pointer \n");
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
int get_ospf_lsa(struct ospf_lsa *lsa) {
    plugin_context_t *plugin_context = current_context;
    if(plugin_context == NULL) { // check that plugin didn't send null pointer
        printf("NULL pointer \n");
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
int get_lsa_header(struct lsa_header *lsah) {
    plugin_context_t *plugin_context = current_context;
    if(plugin_context == NULL) { // check that plugin didn't send null pointer
        printf("NULL pointer \n");
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
int get_ospf_area(struct ospf_area *area) {
    plugin_context_t *plugin_context = current_context;
    if(plugin_context == NULL) { // check that plugin didn't send null pointer
        printf("NULL pointer \n");
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
