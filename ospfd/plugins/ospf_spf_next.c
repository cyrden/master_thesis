//
// Created by cyril on 29/04/19.
//

#include "plugins.h"

struct my_link {
    struct in_addr link_id;
    struct in_addr link_data;
    uint8_t type;
    uint8_t tos;
    uint16_t metric;
    uint32_t color;
};


/* RFC2328 Section 16.1 (2).
 * v is on the SPF tree.  Examine the links in v's LSA.  Update the list
 * of candidates with any vertices not already on the list.  If a lower-cost
 * path is found to a vertex already on the candidate list, store the new cost.
 * The plugin does something more. For each router-LSA, it checks if there is a corresponding type 13 LSA. If yes, we check if the link is green or red.
 * If the link is red, we ignore it and thus "prune" the tree after it.
 */
uint64_t ospf_spf_next(void *data)
{
    struct arg_plugin_ospf_spf_next *plugin_arg = (struct arg_plugin_ospf_spf_next *) data;
    struct vertex *v = plugin_malloc(sizeof(struct vertex));
    if(v == NULL) return 0;
    if(get_vertex(plugin_arg->v, v) != 1) return 0;

    struct ospf_lsa *w_lsa = NULL;
    uint8_t *p;
    uint8_t *lim;
    struct router_lsa_link *l = NULL;
    struct in_addr *r;
    int type = 0, lsa_pos = -1, lsa_pos_next = 0;

    /* If this is a router-LSA, and bit V of the router-LSA (see Section
       A.4.2:RFC2328) is set, set Area A's TransitCapability to TRUE.  */
    struct lsa_header *lsah = plugin_malloc(sizeof(struct lsa_header));
    if(lsah == NULL) return 0;
    if(get_lsa_header(v->lsa, lsah) != 1) return 0;
    lsah = plugin_realloc(lsah, (unsigned int) (((lsah->length & 0x00FF) << 8) | ((lsah->length & 0xFF00) >> 8)));
    if (v->type == OSPF_VERTEX_ROUTER) { // This is a router LSA
        if(get_lsa_with_length(v->lsa, lsah) != 1) return 0;
        if (IS_ROUTER_LSA_VIRTUAL((struct router_lsa *) lsah)) {
            struct ospf_area *area_copy = plugin_malloc(sizeof(struct ospf_area));
            if(area_copy == NULL) return 0;
            if(get_ospf_area(plugin_arg->area, area_copy) != 1) return 0;
            area_copy->transit = OSPF_TRANSIT_TRUE;
            if (set_ospf_area(plugin_arg->area, area_copy) != 1) return 0;
            plugin_free(area_copy);
        }
    }
    else {
        if(get_lsa_with_length(v->lsa, lsah) != 1) return 0;
    }

    p = ((uint8_t *) lsah) + OSPF_LSA_HEADER_SIZE + 4;
    lim = ((uint8_t *) lsah) + ((uint8_t) plugin_ntohs(lsah->length));
    struct ospf_lsa *test_lsa;
    test_lsa = plugin_ospf_lsa_lookup(plugin_arg->ospf, plugin_arg->area, 13, v->id, v->id);
    while (p < lim) {
        struct vertex *w = NULL;
        unsigned int distance;

        /* In case of V is Router-LSA. */
        if (lsah->type == OSPF_ROUTER_LSA) {
            l = (struct router_lsa_link *)p;

            int ignored = 0;
            if(test_lsa != NULL) { // There is a type 13 correspondig LSA
                struct ospf_lsa *test_lsa_copy = plugin_malloc(sizeof(struct ospf_lsa));
                if(test_lsa_copy == NULL) return 0;
                if(get_ospf_lsa(test_lsa, test_lsa_copy) != 1) return 0;
                struct lsa_header *test_lsah = plugin_malloc(sizeof(struct lsa_header));
                if(test_lsah == NULL) return 0;
                if(get_lsa_header(test_lsa_copy->data, test_lsah) != 1) return 0;
                test_lsah = plugin_realloc(test_lsah, (unsigned int) (((test_lsah->length & 0x00FF) << 8) |
                                                                      ((test_lsah->length & 0xFF00) >> 8)));
                if(get_lsa_with_length(test_lsa_copy->data, test_lsah) != 1) return 0;
                uint8_t *my_p = ((uint8_t *) test_lsah) + OSPF_LSA_HEADER_SIZE + 4;
                while (my_p < ((uint8_t  *) test_lsah) + (((test_lsah->length & 0x00FF) << 8) | ((test_lsah->length & 0xFF00) >> 8))) {
                    struct my_link *my_link = (struct my_link *) my_p;
                    if (l->link_id.s_addr == my_link->link_id.s_addr) {
                        if(plugin_ntohl(my_link->color)== RED) { // If the link is red, we ignore it
                            ignored = 1;
                            break;
                        }
                    }
                    my_p += sizeof(struct my_link);
                }
                plugin_free(test_lsah);
                plugin_free(test_lsa_copy);
            }

            lsa_pos = lsa_pos_next; /* LSA link position */
            lsa_pos_next++;
            p += (OSPF_ROUTER_LSA_LINK_SIZE + (l->m[0].tos_count * OSPF_ROUTER_LSA_TOS_SIZE));

            if(ignored) { // skip this link, it is the wrong color
                continue;
            }

            /* (a) If this is a link to a stub network, examine the
               next
               link in V's LSA.  Links to stub networks will be
               considered in the second stage of the shortest path
               calculation. */
            if ((type = l->m[0].type) == LSA_LINK_TYPE_STUB)
                continue;

            /* (b) Otherwise, W is a transit vertex (router or
               transit
               network).  Look up the vertex W's LSA (router-LSA or
               network-LSA) in Area A's link state database. */
            switch (type) {
                case LSA_LINK_TYPE_POINTOPOINT:
                case LSA_LINK_TYPE_VIRTUALLINK:
                    w_lsa = plugin_ospf_lsa_lookup(plugin_arg->ospf, plugin_arg->area, OSPF_ROUTER_LSA, l->link_id, l->link_id);
                    break;
                case LSA_LINK_TYPE_TRANSIT:
                    w_lsa = plugin_ospf_lsa_lookup_by_id(plugin_arg->area, OSPF_NETWORK_LSA, l->link_id);
                    break;
                default:
                    continue;
            }
        } else {
            /* In case of V is Network-LSA. */
            r = (struct in_addr *)p;
            p += sizeof(struct in_addr);

            /* Lookup the vertex W's LSA. */
            w_lsa = plugin_ospf_lsa_lookup_by_id(plugin_arg->area, OSPF_ROUTER_LSA, *r);
        }

        /* (b cont.) If the LSA does not exist, or its LS age is equal
           to MaxAge, or it does not have a link back to vertex V,
           examine the next link in V's LSA.[23] */
        if (w_lsa == NULL) {
            continue;
        }

        struct ospf_lsa *w_lsa_copy = plugin_malloc(sizeof(struct ospf_lsa));
        if(w_lsa_copy == NULL) {
            return 0;
        }
        if(get_ospf_lsa(w_lsa, w_lsa_copy) != 1) return 0;
        struct lsa_header *w_lsah = plugin_malloc(sizeof(struct lsa_header));
        if(w_lsah == NULL) return 0;
        if(get_lsa_header(w_lsa_copy->data, w_lsah) != 1) return 0;
        if (w_lsah->ls_age == OSPF_LSA_MAXAGE) {
            plugin_free(w_lsa_copy);
            plugin_free(w_lsah);
            continue;
        }

        if (plugin_ospf_lsa_has_link(w_lsa_copy->data, v->lsa) < 0) {
            plugin_free(w_lsa_copy);
            plugin_free(w_lsah);
            continue;
        }

        /* (c) If vertex W is already on the shortest-path tree, examine
           the next link in the LSA. */
        if (w_lsa_copy->stat == LSA_SPF_IN_SPFTREE) {
            plugin_free(w_lsa_copy);
            plugin_free(w_lsah);
            continue;
        }

        /* (d) Calculate the link state cost D of the resulting path
           from the root to vertex W.  D is equal to the sum of the link
           state cost of the (already calculated) shortest path to
           vertex V and the advertised cost of the link between vertices
           V and W.  If D is: */

        /* calculate link cost D. */
        if (lsah->type == OSPF_ROUTER_LSA)
            distance = v->distance + (((l->m[0].metric & 0x00FF) << 8) | ((l->m[0].metric & 0xFF00) >> 8));
        else /* v is not a Router-LSA */
            distance = v->distance;


        /* Is there already vertex W in candidate list? */
        if (w_lsa_copy->stat == LSA_SPF_NOT_EXPLORED) {
            /* prepare vertex W. */
            w = plugin_ospf_vertex_new(w_lsa);

            /* Calculate nexthop to W. */
            if (plugin_ospf_nexthop_calculation(plugin_arg, w, l, distance, lsa_pos))
                plugin_pqueue_enqueue(w, plugin_arg->candidate);
        } else if (w_lsa_copy->stat >= 0) {
            /* Get the vertex from candidates. */
            if((w = get_candidate(plugin_arg->candidate, w_lsa_copy->stat)) == NULL) return 0;

            struct vertex *w_copy = plugin_malloc(sizeof(struct vertex));
            if(w_copy == NULL) return 0;
            if(get_vertex(w, w_copy) != 1) return 0;
            /* if D is greater than. */
            if (w_copy->distance < distance) {
                plugin_free(w_lsa_copy);
                plugin_free(w_lsah);
                continue;
            }
                /* equal to. */
            else if (w_copy->distance == distance) {
                /* Found an equal-cost path to W.
                 * Calculate nexthop of to W from V. */
                plugin_ospf_nexthop_calculation(plugin_arg, w, l, distance, lsa_pos);
            }
                /* less than. */
            else {
                /* Found a lower-cost path to W.
                 * nexthop_calculation is conditional, if it
                 * finds
                 * valid nexthop it will call spf_add_parents,
                 * which
                 * will flush the old parents
                 */
                if (plugin_ospf_nexthop_calculation(plugin_arg, w, l, distance, lsa_pos))
                    /* Decrease the key of the node in the
                     * heap.
                     * trickle-sort it up towards root, just
                     * in case this
                     * node should now be the new root due
                     * the cost change.
                     * (next pqueu_{de,en}queue will fully
                     * re-heap the queue).
                     */
                    plugin_trickle_up(w_lsa_copy->stat, plugin_arg->candidate);
            }
            plugin_free(w_copy);
        } /* end W is already on the candidate list */
        plugin_free(w_lsa_copy);
        plugin_free(w_lsah);
    }	 /* end loop over the links in V's LSA */
    plugin_free(v);
    plugin_free(lsah);
    return 1;
}