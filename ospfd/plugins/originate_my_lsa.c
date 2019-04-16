//
// Created by cyril on 03/04/19.
//

#include "plugins.h"

#define OSPF_MY_LSA 13

static struct ospf_lsa *ospf_my_lsa_originate(struct ospf_area *area);

uint64_t originate_my_lsa(void *data) {
    struct arg_plugin_spf_calc *plugin_arg = (struct arg_plugin_spf_calc *) data;

    struct ospf_area *area = my_malloc(sizeof(struct ospf_area));
    if(get_ospf_area(area) != 1) return 0;
    area->ospf = my_malloc(sizeof(struct ospf));
    if(get_ospf(area->ospf) != 1) return 0;
    struct ospf_lsa *lsa = ospf_my_lsa_originate(area);
    if(lsa == NULL) return 0;
    //struct ospf_lsa *lsa = my_malloc(&plugin_arg->heap, sizeof(struct ospf_lsa));
    //if(lsa == NULL) return 0;
    //lsa->data = my_malloc(&plugin_arg->heap, sizeof(struct lsa_header));
    //if(lsa->data == NULL) return 0;
    //lsa->data->type = OSPF_MY_LSA;
    //struct ospf_lsa *test = my_malloc(&plugin_arg->heap, sizeof(struct ospf_lsa));
    //my_memcpy(test, lsa, sizeof(struct ospf_lsa));
    return send_data(SPF_LSA, (void *) lsa);
}

/* Create new my-LSA. */
static struct ospf_lsa *ospf_my_lsa_new(struct ospf_area *area)
{
    struct stream *s;
    struct lsa_header *lsah;
    struct ospf_lsa *new;
    int length;

    /* Create a stream for LSA. */
    s = my_malloc(sizeof(struct stream) + OSPF_MAX_LSA_SIZE);
    if(s == NULL) return NULL;
    s->getp = s->endp = 0;
    s->next = NULL;
    s->size = OSPF_MAX_LSA_SIZE;

    /* Set LSA common header fields. */
    lsah = (struct lsa_header *) s->data;

    //lsah->ls_age = htons(OSPF_LSA_INITIAL_AGE);
    lsah->ls_age = 0;
    lsah->options = 0;
    lsah->type = OSPF_MY_LSA;
    lsah->id = area->ospf->router_id;
    lsah->adv_router = area->ospf->router_id;
    //lsah->ls_seqnum = htonl(OSPF_INITIAL_SEQUENCE_NUMBER);
    lsah->ls_seqnum = 80;

    s->endp += OSPF_MAX_LSA_SIZE;

    /* Set router-LSA body fields. */
    //ospf_router_lsa_body_set(&s, area);

    /* Set length. */
    length = s->endp;
    lsah = (struct lsa_header *) s->data;
    //lsah->length = htons(length);

    /* Now, create OSPF LSA instance. */
    new = my_malloc(sizeof(struct ospf_lsa));
    if(new == NULL) return NULL;
    new->flags = 0;
    new->lock = 1;
    new->retransmit_counter = 0;
    //monotime(&new->tv_recv);
    //new->tv_orig = new->tv_recv;
    new->refresh_list = -1;
    //new->vrf_id = VRF_DEFAULT;
    //new->data = my_malloc(heap, length);
    new->data = my_malloc(sizeof(struct lsa_header));
    if(new->data == NULL) return NULL;

    new->area = area;
    new->flags = OSPF_LSA_SELF | OSPF_LSA_SELF_CHECKED;
    new->vrf_id = area->ospf->vrf_id;

    /* Copy LSA data to store, discard stream. */
    //my_memcpy(new->data, lsah, length);
    my_memcpy(new->data, lsah, sizeof(struct lsa_header));
    if(new->data->type != 13) return NULL;
    //my_memcpy(new->data, lsah, OSPF_LSA_HEADER_SIZE);

    my_free(s);

    return new;
}

/* Originate my-LSA. */
static struct ospf_lsa *ospf_my_lsa_originate(struct ospf_area *area)
{
    struct ospf_lsa *new;

    /* Create new my-LSA instance. */
    if ((new = ospf_my_lsa_new(area)) == NULL) {
        return NULL;
    }

    /* Sanity check. */
    if (new->data->adv_router.s_addr == 0) {
        return NULL;
    }

    /* Install LSA to LSDB. */
    new = ospf_lsa_install(area->ospf, NULL, new);

    /* Update LSA origination count. */
    //area->ospf->lsa_originate_count++;

    /* Flooding new LSA through area. */
    //ospf_flood_through_area(area, NULL, new);

    return new;
}
