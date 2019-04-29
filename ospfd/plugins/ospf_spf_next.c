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
 */
/*static void ospf_spf_next(struct vertex *v, struct ospf *ospf,
                          struct ospf_area *area, struct pqueue *candidate)*/
uint64_t ospf_spf_next(void *data)
{
    struct vertex *v;
    struct ospf *ospf;
    struct ospf_area *area;
    struct pqueue *candidate;

    //struct ospf_lsa *w_lsa = NULL;
    struct ospf_lsa *w_lsa;
    uint8_t *p;
    uint8_t *lim;
    struct router_lsa_link *l = NULL;
    struct in_addr *r;
    int type = 0, lsa_pos = -1, lsa_pos_next = 0;

    /* If this is a router-LSA, and bit V of the router-LSA (see Section
       A.4.2:RFC2328) is set, set Area A's TransitCapability to TRUE.  */
    if (v->type == OSPF_VERTEX_ROUTER) {
        if (IS_ROUTER_LSA_VIRTUAL((struct router_lsa *)v->lsa))
            area->transit = OSPF_TRANSIT_TRUE;
    }

    p = ((uint8_t *)v->lsa) + OSPF_LSA_HEADER_SIZE + 4;
    //lim = ((uint8_t *)v->lsa) + my_ntohs(v->lsa->length);

    while (p < lim) {
        struct vertex *w;
        unsigned int distance;

        /* In case of V is Router-LSA. */
        if (v->lsa->type == OSPF_ROUTER_LSA) {
            l = (struct router_lsa_link *)p;

            int ignored = 0;
            struct ospf_lsa *test_lsa;
            //test_lsa = ospf_lsa_lookup(ospf, area, 13, v->id, v->id);
           /* if(test_lsa != NULL) {
                uint8_t *my_p = ((uint8_t *) test_lsa->data) + OSPF_LSA_HEADER_SIZE + 4;
                while (my_p < ((uint8_t  *) test_lsa->data) + my_ntohs(test_lsa->data->length)) {
                    struct my_link *my_link = (struct my_link *) my_p;
                    if (l->link_id.s_addr == my_link->link_id.s_addr) {
                        if(my_ntohl(my_link->color) == 14) {
                            ignored = 1;
                            break;
                        }
                    }
                    my_p += sizeof(struct my_link);
                }
            }*/

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
                    //w_lsa = ospf_lsa_lookup(ospf, area, OSPF_ROUTER_LSA, l->link_id, l->link_id);
                    break;
                case LSA_LINK_TYPE_TRANSIT:
                    //w_lsa = ospf_lsa_lookup_by_id(area, OSPF_NETWORK_LSA, l->link_id);
                    break;
                default:
                    continue;
            }
        } else {
            /* In case of V is Network-LSA. */
            r = (struct in_addr *)p;
            p += sizeof(struct in_addr);

            /* Lookup the vertex W's LSA. */
            //w_lsa = ospf_lsa_lookup_by_id(area, OSPF_ROUTER_LSA, *r);
        }

        /* (b cont.) If the LSA does not exist, or its LS age is equal
           to MaxAge, or it does not have a link back to vertex V,
           examine the next link in V's LSA.[23] */
        if (w_lsa == NULL) {
            continue;
        }

        if (IS_LSA_MAXAGE(w_lsa)) {
            continue;
        }

        /*if (ospf_lsa_has_link(w_lsa->data, v->lsa) < 0) {
            continue;
        }*/

        /* (c) If vertex W is already on the shortest-path tree, examine
           the next link in the LSA. */
        if (w_lsa->stat == LSA_SPF_IN_SPFTREE) {
            continue;
        }

        /* (d) Calculate the link state cost D of the resulting path
           from the root to vertex W.  D is equal to the sum of the link
           state cost of the (already calculated) shortest path to
           vertex V and the advertised cost of the link between vertices
           V and W.  If D is: */

        /* calculate link cost D. */
        if (v->lsa->type == OSPF_ROUTER_LSA)
            distance = v->distance + my_ntohs(l->m[0].metric);
        else /* v is not a Router-LSA */
            distance = v->distance;

        /* Is there already vertex W in candidate list? */
        if (w_lsa->stat == LSA_SPF_NOT_EXPLORED) {
            /* prepare vertex W. */
            //w = ospf_vertex_new(w_lsa);

            /* Calculate nexthop to W. */
            //if (ospf_nexthop_calculation(area, v, w, l, distance, lsa_pos))
            //    pqueue_enqueue(w, candidate);
        } else if (w_lsa->stat >= 0) {
            /* Get the vertex from candidates. */
            w = candidate->array[w_lsa->stat];

            /* if D is greater than. */
            if (w->distance < distance) {
                continue;
            }
                /* equal to. */
            else if (w->distance == distance) {
                /* Found an equal-cost path to W.
                 * Calculate nexthop of to W from V. */
                //ospf_nexthop_calculation(area, v, w, l, distance, lsa_pos);
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
                //if (ospf_nexthop_calculation(area, v, w, l, distance, lsa_pos))
                    /* Decrease the key of the node in the
                     * heap.
                     * trickle-sort it up towards root, just
                     * in case this
                     * node should now be the new root due
                     * the cost change.
                     * (next pqueu_{de,en}queue will fully
                     * re-heap the queue).
                     */
                    //trickle_up(w_lsa->stat, candidate);
            }
        } /* end W is already on the candidate list */
    }	 /* end loop over the links in V's LSA */
    return 1;
}