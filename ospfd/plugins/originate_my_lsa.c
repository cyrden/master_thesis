//
// Created by cyril on 03/04/19.
//

#include "plugins.h"

#define OSPF_MY_LSA 13

static struct ospf_lsa *ospf_my_lsa_originate(struct ospf_area *area);

uint64_t originate_my_lsa(void *data) {
    struct arg_plugin_spf_calc *plugin_arg = (struct arg_plugin_spf_calc *) data;
    /* TODO: can't let the user do this + even if I do it in an init method, how to ensure the user will not change these ? (Make crash ...)*/
    plugin_arg->heap.heap_start = &plugin_arg->heap.mem;
    plugin_arg->heap.heap_end = &plugin_arg->heap.mem;
    plugin_arg->heap.heap_last_block = NULL;

    struct ospf_area *area = my_malloc(&plugin_arg->heap, sizeof(struct ospf_area));
    if(get_ospf_area(plugin_arg->plugin_context, area) != 1) return 0;
    struct ospf_lsa *lsa;
    return send_data(SPF_LSA, (void *) lsa);
}

/* Create new router-LSA. */
static struct ospf_lsa *ospf_my_lsa_new(struct ospf_area *area)
{
    struct ospf *ospf = area->ospf;
    struct stream *s;
    struct lsa_header *lsah;
    struct ospf_lsa *new;
    int length;

    if (IS_DEBUG_OSPF(lsa, LSA_GENERATE))
        zlog_debug("LSA[Type1]: Create router-LSA instance");

    /* check whether stub-router is desired, and if this is the first
     * router LSA.
     */
    //ospf_stub_router_check(area);

    /* Create a stream for LSA. */
    s = stream_new(OSPF_MAX_LSA_SIZE);
    /* Set LSA common header fields. */
    lsa_header_set(s, LSA_OPTIONS_GET(area) | LSA_OPTIONS_NSSA_GET(area),
                   OSPF_MY_LSA, ospf->router_id, ospf->router_id);

    /* Set router-LSA body fields. */
    //ospf_router_lsa_body_set(&s, area);

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

/* Originate my-LSA. */
static struct ospf_lsa *ospf_my_lsa_originate(struct ospf_area *area)
{
    struct ospf_lsa *new;

    /* Create new router-LSA instance. */
    if ((new = ospf_my_lsa_new(area)) == NULL) {
        zlog_err("%s: ospf_router_lsa_new returned NULL", __func__);
        return NULL;
    }

    /* Sanity check. */
    if (new->data->adv_router.s_addr == 0) {
        if (IS_DEBUG_OSPF_EVENT)
            zlog_debug("LSA[Type1]: AdvRouter is 0, discard");
        ospf_lsa_discard(new);
        return NULL;
    }

    /* Install LSA to LSDB. */
    //new = ospf_lsa_install(area->ospf, NULL, new);

    /* Update LSA origination count. */
    //area->ospf->lsa_originate_count++;

    /* Flooding new LSA through area. */
    //ospf_flood_through_area(area, NULL, new);

    return new;
}