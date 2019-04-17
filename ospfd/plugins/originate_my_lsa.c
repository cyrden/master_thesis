//
// Created by cyril on 03/04/19.
//

#include "plugins.h"

#define OSPF_MY_LSA_TYPE 13

static struct ospf_lsa *ospf_my_lsa_originate(struct ospf_area *area);

uint64_t originate_my_lsa(void *data) {
    struct arg_plugin_spf_calc *plugin_arg = (struct arg_plugin_spf_calc *) data;

    struct ospf_area *area = my_malloc(sizeof(struct ospf_area));
    if(get_ospf_area(plugin_arg->area, area) != 1) return 0;
    area->ospf = my_malloc(sizeof(struct ospf));
    if(get_ospf(area->ospf, area->ospf) != 1) return 0;
    struct ospf_lsa *lsa = ospf_my_lsa_originate(area);
    if(lsa == NULL) return 0;
    my_free(area->ospf);
    my_free(area);
    struct lsa_header lsah;
    my_get_lsah(lsa, &lsah);
    return send_data(SPF_LSA, (void *) &lsah);
}

/* Originate my-LSA. */
static struct ospf_lsa *ospf_my_lsa_originate(struct ospf_area *area)
{
    struct ospf_lsa *new;

    /* Create new my-LSA instance. */
    if ((new = ospf_my_lsa_new(area, OSPF_MY_LSA_TYPE)) == NULL) {
        return NULL;
    }

    /* Sanity check. */
    /*if (new->data->adv_router.s_addr == 0) {
        return NULL;
    }*/

    /* Install LSA to LSDB. */
    //new = ospf_lsa_install(area->ospf, NULL, new); // Here area->ospf is a copy be careful

    /* Update LSA origination count. */
    //area->ospf->lsa_originate_count++; // I think this will work because we receive as argument real var from OSPF for the moment and not copy

    /* Flooding new LSA through area. */
    //ospf_flood_through_area(area, NULL, new);

    return new;
}
