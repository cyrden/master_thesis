//
// Created by cyril on 03/04/19.
//

#include "plugins.h"

#define OSPF_MY_LSA_TYPE 13

static struct ospf_lsa *ospf_my_lsa_originate(struct ospf_area *area, struct ospf_area *area_copy);

uint64_t originate_my_lsa(void *data) {
    struct arg_plugin_spf_calc *plugin_arg = (struct arg_plugin_spf_calc *) data;

    struct ospf_area *area = my_malloc(sizeof(struct ospf_area));
    if(get_ospf_area(plugin_arg->area, area) != 1) return 0;
    struct ospf *ospf = my_malloc(sizeof(struct ospf));
    if(get_ospf(area->ospf, ospf) != 1) return 0;
    area->ospf= ospf;
    struct ospf_lsa *lsa = ospf_my_lsa_originate(plugin_arg->area, area);
    if(lsa == NULL) return 0;
    my_free(ospf);
    my_free(area);
    struct lsa_header lsah;
    my_get_lsah(lsa, &lsah);
    return send_data(SPF_LSA, (void *) &lsah);
}

/* Originate my-LSA. */
static struct ospf_lsa *ospf_my_lsa_originate(struct ospf_area *area, struct ospf_area *area_copy)
{
    struct ospf_lsa *new;

    // TODO: Here I should go through interfaces and say what metric I want to put for each one -> add a parameter to new LSA
    /* Create new my-LSA instance. */
    if ((new = ospf_my_lsa_new(area, OSPF_MY_LSA_TYPE, 14, 16)) == NULL) {
        return NULL;
    }

    /* Install LSA to LSDB. */
    new = my_ospf_lsa_install(area_copy->ospf, NULL, new);

    /* Update LSA origination count. */
    //area->ospf->lsa_originate_count++;

    /* Flooding new LSA through area. */
    my_ospf_flood_through_area(area, NULL, new);

    return new;
}
