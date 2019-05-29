//
// Created by cyril on 30/11/18.
//

/*
 * Helper functions are registered to the eBPF vm on creation to be used as external functions
 */

#include <ospfd/ospf_lsa.h>
#include "ospf_plugins_api.h"
#include "../../ospfd/plugins_manager/plugins_manager.h"

/*
 * Struct of the message buffer stored in kernel (used to communicate with monitoring server)
 */
struct mesg_buffer {
    long mesg_type;
    char mesg_text[SIZE_MESG];
} message;

/****** Debug functions to remove ******/

void print_helper(int a) {
    zlog_notice("%d", a);
}

void lsa_head_dump (struct lsa_header *lsah) {
    zlog_notice("lsa_head_dump :");
    ospf_lsa_header_dump(lsah);
}


/****** Own implem of network / host byte order functions ******/

uint32_t plugin_ntohl(uint32_t value) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return (((value & 0x000000FF) << 24) |
            ((value & 0x0000FF00) << 8) |
            ((value & 0x00FF0000) >> 8) |
            ((value & 0xFF000000) >> 24));
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return value;
#else
#    error unsupported endianness
#endif
}

uint16_t plugin_ntohs(uint16_t value) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return (uint16_t) (((value & 0x00FF) << 8) |
            ((value & 0xFF00) >> 8));
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return value;
#else
#    error unsupported endianness
#endif
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
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        printf("sender msgget error \n");
        return 0;
    }

    message.mesg_type = type;
    if(data != NULL) {
        memcpy((void *) message.mesg_text, data, SIZE_MESG * sizeof(char)); //copy the data to the message
    }
    msgsnd(msgid, &message, sizeof(message), IPC_NOWAIT);

    return 1;
}


/****** Getter functions ******/


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
        case ARG_PLUGIN_SPF_CALC:
            memcpy(oi_copy, oi, sizeof(struct ospf_interface));
            break;
        default:
            fprintf(stderr, "Argument type not recognized by helper function");
            return 0;
    }
    return 1;
}


int get_ospf_interface_list(struct list *oilist, struct ospf_interface **oilist_copy, struct ospf_interface **oi_list_addresses) {
    pluglet_context_t *pluglet_context = current_context;
    if(pluglet_context == NULL) {
        printf("NULL pointer \n");
        return 0;
    }
    if(oilist == NULL || oilist_copy == NULL) return 0;
    /* This switch is because depending on where the plugin that uses this helper function has been inserted, we need to cast to the good argument type */
    switch (pluglet_context->type_arg) {
        case ARG_PLUGIN_SPF_CALC: ;
            struct listnode *node;
            struct ospf_interface *oi;
            int i = 0;
            for (ALL_LIST_ELEMENTS_RO(oilist, node, oi)) {
                memcpy(oilist_copy[i], oi, sizeof(struct ospf_interface));
                oi_list_addresses[i] = oi;
                i++;
            }
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
    if(ifp_copy == NULL) return 0;
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
        case ARG_PLUGIN_SPF_CALC:
            memcpy(ifp_copy, ifp, sizeof(struct ospf_interface));
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
        return 0;
    }
    if(lsa == NULL) {
        return 0;
    }
    if(lsa_copy == NULL) {
        return 0;
    }
    /* This switch is because depending on where the plugin that uses this helper function has been inserted, we need to cast to the good argument type */
    switch (pluglet_context->type_arg) {
        case ARG_PLUGIN_LSA_FLOOD:
            if(lsa != ((struct arg_plugin_lsa_flood *) pluglet_context->original_arg)->lsa) return 0; // user didn't give the good pointer. We probably don't want it to access it.
            memcpy(lsa_copy, ((struct arg_plugin_lsa_flood *) pluglet_context->original_arg)->lsa, sizeof(struct ospf_lsa));
            break;
        case ARG_PLUGIN_OSPF_SPF_NEXT:
            //zlog_notice("get_ospf_lsa");
            memcpy(lsa_copy, lsa, sizeof(struct ospf_lsa));
            break;
        default:
            zlog_notice("Argument type not recognized by helper function");
            return 0;
    }
    return 1;
}


int get_lsa_header(struct lsa_header *lsah, struct lsa_header *lsah_copy) {
    pluglet_context_t *pluglet_context = current_context;
    if(pluglet_context == NULL) {
        printf("NULL pointer \n");
        return 0;
    }
    if(lsah == NULL) return 0; // check that plugin didn't send NULL pointer
    if(lsah_copy == NULL) return 0;
    /* This switch is because depending on where the plugin that uses this helper function has been inserted, we need to cast to the good argument type */
    switch (pluglet_context->type_arg) {
        case ARG_PLUGIN_LSA_FLOOD:
            if(lsah != ((struct arg_plugin_lsa_flood *) pluglet_context->original_arg)->lsa->data) return 0; // user didn't give the good pointer. We probably don't want it to access it.
            memcpy(lsah_copy, ((struct arg_plugin_lsa_flood *) pluglet_context->original_arg)->lsa->data, sizeof(struct lsa_header));
            break;
        case ARG_PLUGIN_OSPF_SPF_NEXT:
            memcpy(lsah_copy, lsah, sizeof(struct lsa_header));
            break;
        default:
            zlog_notice("Argument type not recognized by helper function");
            return 0;
    }
    return 1;
}

int get_lsa_with_length(struct lsa_header *lsah, struct lsa_header *lsah_copy) {
    pluglet_context_t *pluglet_context = current_context;
    if(pluglet_context == NULL) { // check that plugin didn't send null pointer
        printf("NULL pointer \n");
        return 0;
    }
    if(lsah == NULL) return 0; // otherwise memcpy could fail
    if(lsah_copy == NULL) return 0;
    /* This switch is because depending on where the plugin that uses this helper function has been inserted, we need to cast to the good argument type */
    switch (pluglet_context->type_arg) {
        case ARG_PLUGIN_OSPF_SPF_NEXT:
            memcpy(lsah_copy, lsah, ntohs(lsah->length));
            break;
        default:
            zlog_notice("Argument type not recognized by helper function");
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
    if(area_copy == NULL) return 0;
    /* This switch is because depending on where the plugin that uses this helper function has been inserted, we need to cast to the good argument type */
    switch (pluglet_context->type_arg) {
        case ARG_PLUGIN_SPF_CALC:
            if(area != ((struct arg_plugin_spf_calc *) pluglet_context->original_arg)->area) return 0; // user didn't give the good pointer. We probably don't want it to access it.
            memcpy(area_copy, ((struct arg_plugin_spf_calc *) pluglet_context->original_arg)->area, sizeof(struct ospf_area));
            break;
        case ARG_PLUGIN_OSPF_SPF_NEXT:
            if(area != ((struct arg_plugin_ospf_spf_next *) pluglet_context->original_arg)->area) return 0; // user didn't give the good pointer. We probably don't want it to access it.
            memcpy(area_copy, area, sizeof(struct ospf_area));
            break;
        case ARG_PLUGIN_HELLO_SEND:
            memcpy(area_copy, area, sizeof(struct ospf_area));
            break;
        default:
            zlog_notice("Argument type not recognized by helper function");
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

struct vertex *get_candidate(struct pqueue *candidate, int stat) {
    pluglet_context_t *pluglet_context = current_context;
    struct vertex *vertex = NULL;
    if(pluglet_context == NULL) { // check that plugin didn't send null pointer
        printf("NULL pointer \n");
        return NULL;
    }
    if(candidate == NULL) return 0;
    /* This switch is because depending on where the plugin that uses this helper function has been inserted, we need to cast to the good argument type */
    switch (pluglet_context->type_arg) {
        case ARG_PLUGIN_OSPF_SPF_NEXT:
            vertex = candidate->array[stat];
            break;
        default:
            fprintf(stderr, "Argument type not recognized by helper function");
            return NULL;
    }
    return vertex;
}

int get_vertex(struct vertex *vertex, struct vertex *vertex_copy) {
    pluglet_context_t *pluglet_context = current_context;
    if(pluglet_context == NULL) { // check that plugin didn't send null pointer
        printf("NULL pointer \n");
        return 0;
    }
    if(vertex == NULL) return 0;
    if(vertex_copy == NULL) return 0;
    /* This switch is because depending on where the plugin that uses this helper function has been inserted, we need to cast to the good argument type */
    switch (pluglet_context->type_arg) {
        case ARG_PLUGIN_OSPF_SPF_NEXT:
            memcpy(vertex_copy, vertex, sizeof(struct vertex));
            break;
        default:
            fprintf(stderr, "Argument type not recognized by helper function");
            return 0;
    }
    return 1;
}


/* Setter functions */

int set_ospf_area(struct ospf_area *area, struct ospf_area *area_copy) {
    pluglet_context_t *pluglet_context = current_context;
    if(pluglet_context == NULL) { // check that plugin didn't send null pointer
        printf("NULL pointer \n");
        return 0;
    }
    if(area == NULL) return 0;
    /* This switch is because depending on where the plugin that uses this helper function has been inserted, we need to cast to the good argument type */
    switch (pluglet_context->type_arg) {
        case ARG_PLUGIN_OSPF_SPF_NEXT:
            if(area != ((struct arg_plugin_ospf_spf_next *) pluglet_context->original_arg)->area) return 0; // user didn't give the good pointer. We probably don't want it to access it.
            memcpy(area, area_copy, sizeof(struct ospf_area));
            break;
        default:
            fprintf(stderr, "Argument type not recognized by helper function");
            return 0;
    }
    return 1;
}


int set_ospf_interface(struct ospf_interface *oi, struct ospf_interface *oi_copy) {
    pluglet_context_t *pluglet_context = current_context;
    if(pluglet_context == NULL) {
        printf("NULL pointer \n");
        return 0;
    }
    if(oi == NULL || oi_copy == NULL) return 0;
    /* This switch is because depending on where the plugin that uses this helper function has been inserted, we need to cast to the good argument type */
    switch (pluglet_context->type_arg) {
        case ARG_PLUGIN_SPF_CALC:
            memcpy(oi, oi_copy, sizeof(struct ospf_interface));
            break;
        default:
            fprintf(stderr, "Argument type not recognized by helper function");
            return 0;
    }
    return 1;
}


/****** OSPF functions *****/

struct vertex *plugin_ospf_vertex_new(struct ospf_lsa *lsa) {
    if(lsa == NULL) return NULL;
    return ospf_vertex_new(lsa);
}

int plugin_ospf_lsa_has_link(struct lsa_header *w, struct lsa_header *v) {
    if(w == NULL) return 0;
    if(v == NULL) return 0;
    return ospf_lsa_has_link(w, v);
}

void plugin_trickle_up(int index, struct pqueue *queue) {
    if(queue == NULL) return;
    trickle_up(index, queue);
}

struct ospf_lsa *plugin_ospf_lsa_lookup(struct ospf *ospf, struct ospf_area *area,
                                 uint32_t type, struct in_addr id,
                                 struct in_addr adv_router) {
    if(ospf == NULL) return NULL;
    if(area == NULL) return NULL;
    return ospf_lsa_lookup(ospf, area, type, id, adv_router);
}

void plugin_pqueue_enqueue(void *data, struct pqueue *queue) {
    if(data == NULL) return;
    if(queue == NULL) return;
    return pqueue_enqueue(data, queue);
}

struct ospf_lsa *plugin_ospf_lsa_lookup_by_id(struct ospf_area *area, uint32_t type,
                                       struct in_addr id) {
    if(area == NULL) return NULL;
    return ospf_lsa_lookup_by_id(area, type, id);
}


struct ospf_lsa *plugin_ospf_lsa_install(struct ospf *ospf, struct ospf_interface *oi, struct ospf_lsa *lsa) {
    pluglet_context_t *pluglet_context = current_context;
    if(pluglet_context == NULL) { // check that plugin didn't send null pointer
        printf("NULL pointer \n");
        return NULL;
    }
    if(lsa == NULL) return NULL;
    /* This switch is because depending on where the plugin that uses this helper function has been inserted, we need to cast to the good argument type */
    switch (pluglet_context->type_arg) {
        case ARG_PLUGIN_SPF_CALC:
            break;
        default:
            return NULL;
    }
    ospf_lsa_install(ospf, oi, lsa);
    return lsa;
}

int plugin_ospf_flood_through_area(struct ospf_area *area, struct ospf_neighbor *inbr, struct ospf_lsa *lsa) {
    pluglet_context_t *pluglet_context = current_context;
    if(pluglet_context == NULL) { // check that plugin didn't send null pointer
        printf("NULL pointer \n");
        return 0;
    }
    if(area == NULL) return 0;
    if(lsa == NULL) return 0;
    /* This switch is because depending on where the plugin that uses this helper function has been inserted, we need to cast to the good argument type */
    switch (pluglet_context->type_arg) {
        case ARG_PLUGIN_SPF_CALC:
            if(area != ((struct arg_plugin_spf_calc *) pluglet_context->original_arg)->area) return 0;
            break;
        default:
            return 0;
    }
    ospf_flood_through_area(area, inbr, lsa);
    return 1;
}

unsigned int ospf_nexthop_calculation(struct ospf_area *area,
                                      struct vertex *v, struct vertex *w,
                                      struct router_lsa_link *l,
                                      unsigned int distance, int lsa_pos);

unsigned int plugin_ospf_nexthop_calculation(struct arg_plugin_ospf_spf_next *s, struct vertex *w,
                                             struct router_lsa_link *l,
                                             unsigned int distance, int lsa_pos) {
    struct ospf_area *area = s->area;
    struct vertex *v = s->v;
    return ospf_nexthop_calculation(area, v, w, l, distance, lsa_pos);
}

/* Set a link information. */
static char my_link_info_set(struct stream **s, struct in_addr id,
                          struct in_addr data, uint8_t type, uint8_t tos,
                          uint16_t cost, uint32_t metric)
{

    /* TOS based routing is not supported. */
    stream_put_ipv4(*s, id.s_addr);   /* Link ID. */
    stream_put_ipv4(*s, data.s_addr); /* Link Data. */
    stream_putc(*s, type);		  /* Link Type. */
    stream_putc(*s, tos);		  /* TOS = 0. */
    stream_putw(*s, cost);		  /* Link Cost. */
    stream_putl(*s, metric);		  /* Link color */

    return 1;
}

static uint16_t my_ospf_link_cost(struct ospf_interface *oi)
{
    /* RFC3137 stub router support */
    if (!CHECK_FLAG(oi->area->stub_router_state, OSPF_AREA_IS_STUB_ROUTED))
        return oi->output_cost;
    else
        return OSPF_OUTPUT_COST_INFINITE;
}

/* Describe Broadcast Link. */
int plugin_lsa_link_broadcast_set(struct stream **s, struct ospf_interface *oi, uint32_t metric)
{
    struct ospf_neighbor *dr;
    struct in_addr id, mask;
    uint16_t cost = my_ospf_link_cost(oi);

    /* Describe Type 3 Link. */
    if (oi->state == 3) {
        masklen2ip(oi->address->prefixlen, &mask);
        id.s_addr = oi->address->u.prefix4.s_addr & mask.s_addr;
        return my_link_info_set(s, id, mask, LSA_LINK_TYPE_STUB, 0,
                             oi->output_cost, metric);
    }

    dr = ospf_nbr_lookup_by_addr(oi->nbrs, &DR(oi));
    /* Describe Type 2 link. */
    if (dr && (dr->state == 9
               || IPV4_ADDR_SAME(&oi->address->u.prefix4, &DR(oi)))
        && ospf_nbr_count(oi, 9) > 0) {
        return my_link_info_set(s, DR(oi), oi->address->u.prefix4,
                             LSA_LINK_TYPE_TRANSIT, 0, cost, metric);
    }
        /* Describe type 3 link. */
    else {
        masklen2ip(oi->address->prefixlen, &mask);
        id.s_addr = oi->address->u.prefix4.s_addr & mask.s_addr;
        return my_link_info_set(s, id, mask, LSA_LINK_TYPE_STUB, 0,
                             oi->output_cost, metric);
    }
}

struct ospf_lsa *plugin_ospf_lsa_new_and_data(struct stream *s, struct ospf_area *area) {
    int length = s->endp;
    struct lsa_header *lsah = (struct lsa_header *) s->data;
    lsah->length = htons(length);
    struct ospf_lsa *new = ospf_lsa_new_and_data(length);
    // TODO: This set should probably be done in the plugin
    new->area = area;
    SET_FLAG(new->flags, OSPF_LSA_SELF | OSPF_LSA_SELF_CHECKED);
    new->vrf_id = area->ospf->vrf_id;
    memcpy(new->data, lsah, length);
    return new;
}


int my_get_lsah(struct ospf_lsa *lsa, struct lsa_header *lsah) {
    memcpy(lsah, lsa->data, sizeof(struct lsa_header));
    return 1;
}

