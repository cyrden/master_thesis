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

    struct ospf_interface **oi_list = (struct ospf_interface **) my_malloc(sizeof(struct ospf_interface *) * 3);
    struct ospf_interface **oi_list_addresses = (struct ospf_interface **) my_malloc(sizeof(struct ospf_interface *) * 3);
    if(oi_list == NULL) return 0;
    struct interface **i_list = (struct interface **) my_malloc(sizeof(struct interface *) * 3);
    if(i_list == NULL) return 0;

    for (int i=0; i< 3; i++) {
        oi_list[i] = (struct ospf_interface *) my_malloc(sizeof(struct ospf_interface));
        if(oi_list[i] == NULL) return 0;
        oi_list[i]->hello_out = (uint32_t) -1;
    }
    if(get_ospf_interface_list(area->oiflist, oi_list, oi_list_addresses) != 1) return 0;
    for (int i=0; i< 3; i++) {
        if(oi_list[i]->hello_out != (uint32_t) -1) {
            i_list[i] = (struct interface *) my_malloc(sizeof(struct interface));
            if (i_list[i] == NULL) return 0;
            if (get_interface(oi_list[i]->ifp, i_list[i]) != 1) return 0;
        }
        else oi_list[i] = NULL;
    }


    struct stream *s;
    struct lsa_header *lsah;
    struct ospf_lsa *new;
    int length;

    /* Create a stream for LSA. */
    s = my_malloc(sizeof(struct stream) + OSPF_MAX_LSA_SIZE);

    s->getp = s->endp = 0;
    s->next = NULL;
    s->size = OSPF_MAX_LSA_SIZE;
    /* Set LSA common header fields. */

    lsah = (struct lsa_header *) s->data;

    lsah->ls_age = 0;
    lsah->options = (uint8_t) 0;
    lsah->type = OSPF_MY_LSA_TYPE;
    lsah->id = ospf->router_id;
    lsah->adv_router = ospf->router_id;
    lsah->ls_seqnum = my_ntohs(0x10);

    s->endp += OSPF_LSA_HEADER_SIZE;

    /* Lsa body */
    unsigned long putp;
    uint16_t cnt;
    /* Set flags. */
    s->data[s->endp++] = 0;
    /* Set Zero fields. */
    s->data[s->endp++] = 0;
    /* Keep pointer to # links. */
    putp = s->endp;
    /* Forward word */
    s->data[s->endp++] = (uint8_t)(0 >> 8);
    s->data[s->endp++] = (uint8_t)0;    /* Set all link information. */

    /* My links lsa set */
    int links = 0;

    struct stream **test = &s;
    for (int i = 0; i < 3; i++) {
        struct ospf_interface *oi = oi_list[i];
        struct interface *ifp = i_list[i];
        /* Check interface is up, OSPF is enable. */
        if (ifp != NULL) {
            if (oi->state != 1) {
                oi->lsa_pos_beg = links;
                /* Describe each link. */
                switch (oi->type) {
                    case OSPF_IFTYPE_BROADCAST:
                        if(ifp->speed > 500) {
                            links += my_lsa_link_broadcast_set(&s, oi_list_addresses[i], GREEN);
                        }
                        else{
                            links += my_lsa_link_broadcast_set(&s, oi_list_addresses[i], RED);
                        }
                        break;
                }
                oi->lsa_pos_end = links;
            }
        }
        set_ospf_interface(oi_list_addresses[i], oi_list[i]);
    }

    //cnt = my_lsa_link_set(&s, area, metric);
    /* Set # of links here. */
    s->data[putp] = (uint8_t)(cnt >> 8);
    s->data[putp + 1] = (uint8_t)cnt;
    /* Set length. */

    /* Now, create OSPF LSA instance. */
    new = ospf_my_lsa_new_and_data(s, plugin_arg->area);

    my_free(s);

    new = my_ospf_lsa_install(area->ospf, NULL, new);
    if(new == NULL) return 0;
    int ret = my_ospf_flood_through_area(plugin_arg->area, NULL, new);
    if(ret == 0) return 0;

    my_free(ospf);
    my_free(area);
    struct lsa_header print_lsah;
    my_get_lsah(new, &print_lsah);

    for (int i=0; i<3; i++) {
        my_free(oi_list[i]);
    }
    my_free(i_list);    for (int i=0; i<3; i++) {
        my_free(i_list[i]);
    }
    my_free(i_list);


    return send_data(SPF_LSA, (void *) &print_lsah);
}
