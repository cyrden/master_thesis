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
    pluglet_context_t *pluglet_context = current_context;
    if(size > MAX_SIZE_SHARED_HEAP) {
        printf("Size exceeds maximum allowed shared heap size \n");
        return 0;
    }
    if(pluglet_context == NULL) {
        printf("NULL pointer \n");
        return 0;
    }
    if(pluglet_context->parent_plugin->shared_heap != NULL) return 0;
    pluglet_context->parent_plugin->shared_heap = malloc(size);
    if(pluglet_context->parent_plugin->shared_heap == NULL) return 0;
    return 1;
}

int shared_heap_free(void) {
    pluglet_context_t *pluglet_context = current_context;
    if(pluglet_context == NULL) {
        printf("NULL pointer \n");
        return 0;
    }
    if(pluglet_context->parent_plugin->shared_heap == NULL) return 0;
    free(pluglet_context->parent_plugin->shared_heap);
    pluglet_context->parent_plugin->shared_heap = NULL;
    return 1;
}

int shared_heap_get(void *heap_copy, size_t size) {
    pluglet_context_t * pluglet_context = current_context;
    if(size > MAX_SIZE_SHARED_HEAP) {
        printf("Size exceeds maximum allowed shared heap size \n");
        return 0;
    }
    if(pluglet_context == NULL) {
        printf("NULL pointer \n");
        return 0;
    }
    if(pluglet_context->parent_plugin->shared_heap == NULL) return 0;
    memcpy(heap_copy, pluglet_context->parent_plugin->shared_heap, size);
    return 1;
}

int shared_heap_set(void *val, size_t size) {
    pluglet_context_t *pluglet_context = current_context;
    if(size > MAX_SIZE_SHARED_HEAP) {
        printf("Size exceeds maximum allowed shared heap size \n");
        return 0;
    }
    if(pluglet_context == NULL) {
        printf("NULL pointer \n");
        return 0;
    }
    if(pluglet_context->parent_plugin->shared_heap == NULL) return 0;
    memcpy(pluglet_context->parent_plugin->shared_heap, val, size);
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

    return 1;
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
int get_ospf_interface(struct ospf_interface *oi, struct ospf_interface *oi_copy) {
    pluglet_context_t *pluglet_context = current_context;
    if(pluglet_context == NULL) {
        printf("NULL pointer \n");
        return 0;
    }
    if(oi == NULL || oi_copy == NULL) return 0;
    /* This switch is because depending on where the plugin that uses this helper function has been inserted, we need to cast to the good argument type */
    switch (pluglet_context->type_arg) {
        case ARG_PLUGIN_HELLO_SEND:
            if(oi != ((struct arg_plugin_hello_send *) pluglet_context->original_arg)->oi) return 0; // user didn't give the good pointer. We probably don't want it to access it.
            memcpy(oi_copy, ((struct arg_plugin_hello_send *) pluglet_context->original_arg)->oi, sizeof(struct ospf_interface));
            break;
        case ARG_PLUGIN_ISM_CHANGE_STATE:
            if(oi != ((struct arg_plugin_ism_change_state *) pluglet_context->original_arg)->oi) return 0; // user didn't give the good pointer. We probably don't want it to access it.
            memcpy(oi_copy, ((struct arg_plugin_ism_change_state *) pluglet_context->original_arg)->oi, sizeof(struct ospf_interface));
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
int get_interface(struct interface *ifp, struct interface *ifp_copy) {
    pluglet_context_t *pluglet_context = current_context;
    if(pluglet_context == NULL) { // check that plugin didn't send null pointer
        printf("NULL pointer \n");
        return 0;
    }
    if(ifp == NULL) return 0;
    /* This switch is because depending on where the plugin that uses this helper function has been inserted, we need to cast to the good argument type */
    switch (pluglet_context->type_arg) {
        case ARG_PLUGIN_HELLO_SEND:
            if(ifp != ((struct arg_plugin_hello_send *) pluglet_context->original_arg)->oi->ifp) return 0; // user didn't give the good pointer. We probably don't want it to access it.
            memcpy(ifp_copy, ((struct arg_plugin_hello_send *) pluglet_context->original_arg)->oi->ifp, sizeof(struct interface));
            break;
        case ARG_PLUGIN_ISM_CHANGE_STATE:
            if(ifp != ((struct arg_plugin_ism_change_state *) pluglet_context->original_arg)->oi->ifp) return 0; // user didn't give the good pointer. We probably don't want it to access it.
            memcpy(ifp_copy, ((struct arg_plugin_ism_change_state *) pluglet_context->original_arg)->oi->ifp, sizeof(struct interface));
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
int get_ospf_lsa(struct ospf_lsa *lsa, struct ospf_lsa *lsa_copy) {
    pluglet_context_t *pluglet_context = current_context;
    if(pluglet_context == NULL) { // check that plugin didn't send null pointer
        printf("NULL pointer \n");
        return 0;
    }
    if(lsa == NULL) return 0;
    /* This switch is because depending on where the plugin that uses this helper function has been inserted, we need to cast to the good argument type */
    switch (pluglet_context->type_arg) {
        case ARG_PLUGIN_LSA_FLOOD:
            if(lsa != ((struct arg_plugin_lsa_flood *) pluglet_context->original_arg)->lsa) return 0; // user didn't give the good pointer. We probably don't want it to access it.
            memcpy(lsa_copy, ((struct arg_plugin_lsa_flood *) pluglet_context->original_arg)->lsa, sizeof(struct ospf_lsa));
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
int get_lsa_header(struct lsa_header *lsah, struct lsa_header *lsah_copy) {
    pluglet_context_t *pluglet_context = current_context;
    if(pluglet_context == NULL) { // check that plugin didn't send null pointer
        printf("NULL pointer \n");
        return 0;
    }
    if(lsah == NULL) return 0; // otherwise memcpy could fail
    /* This switch is because depending on where the plugin that uses this helper function has been inserted, we need to cast to the good argument type */
    switch (pluglet_context->type_arg) {
        case ARG_PLUGIN_LSA_FLOOD:
            if(lsah != ((struct arg_plugin_lsa_flood *) pluglet_context->original_arg)->lsa->data) return 0; // user didn't give the good pointer. We probably don't want it to access it.
            memcpy(lsah_copy, ((struct arg_plugin_lsa_flood *) pluglet_context->original_arg)->lsa->data, sizeof(struct lsa_header));
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
int get_ospf_area(struct ospf_area *area, struct ospf_area *area_copy) {
    pluglet_context_t *pluglet_context = current_context;
    if(pluglet_context == NULL) { // check that plugin didn't send null pointer
        printf("NULL pointer \n");
        return 0;
    }
    if(area == NULL) return 0;
    /* This switch is because depending on where the plugin that uses this helper function has been inserted, we need to cast to the good argument type */
    switch (pluglet_context->type_arg) {
        case ARG_PLUGIN_SPF_CALC:
            if(area != ((struct arg_plugin_spf_calc *) pluglet_context->original_arg)->area) return 0; // user didn't give the good pointer. We probably don't want it to access it.
            memcpy(area_copy, ((struct arg_plugin_spf_calc *) pluglet_context->original_arg)->area, sizeof(struct ospf_area));
            break;
        default:
            fprintf(stderr, "Argument type not recognized by helper function");
            return 0;
    }
    return 1;
}

/*
 * Getter function to get an ospf structure (bigger than stack --> need to allocate on heap)
 */
int get_ospf(struct ospf *ospf, struct ospf *ospf_copy) {
    pluglet_context_t *pluglet_context = current_context;
    if(pluglet_context == NULL) { // check that plugin didn't send null pointer
        printf("NULL pointer \n");
        return 0;
    }
    if(ospf == NULL) return 0;
    /* This switch is because depending on where the plugin that uses this helper function has been inserted, we need to cast to the good argument type */
    switch (pluglet_context->type_arg) {
        case ARG_PLUGIN_SPF_CALC:
            if(ospf != ((struct arg_plugin_spf_calc *) pluglet_context->original_arg)->area->ospf) return 0; // user didn't give the good pointer. We probably don't want it to access it.
            memcpy(ospf_copy, ((struct arg_plugin_spf_calc *) pluglet_context->original_arg)->area->ospf, sizeof(struct ospf));
            break;
        default:
            fprintf(stderr, "Argument type not recognized by helper function");
            return 0;
    }
    return 1;
}

/* Create new my-LSA. */
struct ospf_lsa *ospf_my_lsa_new(struct ospf_area *area, uint8_t type)
{
    pluglet_context_t *pluglet_context = current_context;
    if(pluglet_context == NULL) { // check that plugin didn't send null pointer
        printf("NULL pointer \n");
        return 0;
    }
    struct ospf_area *area_real;
    switch (pluglet_context->type_arg) {
        case ARG_PLUGIN_SPF_CALC:
            area_real = ((struct arg_plugin_spf_calc *) pluglet_context->original_arg)->area;
            area = area_real;
            break;
        default:
            return NULL;
    }
    struct ospf *ospf = area->ospf;
    struct stream *s;
    struct lsa_header *lsah;
    struct ospf_lsa *new;
    int length;

    /* Create a stream for LSA. */
    s = stream_new(OSPF_MAX_LSA_SIZE);
    /* Set LSA common header fields. */
    lsa_header_set(s, LSA_OPTIONS_GET(area) | LSA_OPTIONS_NSSA_GET(area),
                   type, ospf->router_id, ospf->router_id);

    /* Set length. */
    length = stream_get_endp(s);
    lsah = (struct lsa_header *)STREAM_DATA(s);
    lsah->length = htons(length);

    /* Now, create OSPF LSA instance. */
    new = ospf_lsa_new_and_data(length);

    new->area = area;
    SET_FLAG(new->flags, OSPF_LSA_SELF | OSPF_LSA_SELF_CHECKED);
    new->vrf_id = area->ospf->vrf_id;

    /* Copy LSA data to store, discard stream. */
    memcpy(new->data, lsah, length);
    stream_free(s);
    return new;
}

int my_get_lsah(struct ospf_lsa *lsa, struct lsa_header *lsah) {
    memcpy(lsah, lsa->data, sizeof(struct lsa_header));
    return 1;
}


