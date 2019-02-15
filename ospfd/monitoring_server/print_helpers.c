//
// Created by cyril on 06/02/19.
//

/*
 * All these functions were taken from "ospf_dump.c". We just made some small modifications
 */

#include "print_helpers.h"


static void my_ospf_lsa_header_dump(struct lsa_header *lsah) {
    const char *lsah_type = lookup_msg(ospf_lsa_type_msg, lsah->type, NULL);

    printf("  LSA Header \n");
    printf("    LS age %d \n", ntohs(lsah->ls_age));
    printf("    Options %d (%s) \n", lsah->options,
           ospf_options_dump(lsah->options));
    printf("    LS type %d (%s) \n", lsah->type,
           (lsah->type ? lsah_type : "unknown type \n"));
    printf("    Link State ID %s \n", inet_ntoa(lsah->id));
    printf("    Advertising Router %s \n", inet_ntoa(lsah->adv_router));
    printf("    LS sequence number 0x%lx \n",
           (unsigned long)ntohl(lsah->ls_seqnum));
    printf("    LS checksum 0x%x \n", ntohs(lsah->checksum));
    printf("    length %d \n", ntohs(lsah->length));
}

static void my_ospf_opaque_lsa_dump(struct stream *s, uint16_t length) { // TODO: fix
    /*struct ospf_lsa lsa;

    lsa.data = (struct lsa_header *)stream_pnt(s);
    show_opaque_info_detail(NULL, &lsa);
    return;*/
}

static char *my_ospf_router_lsa_flags_dump(uint8_t flags, char *buf, size_t size)
{
    snprintf(buf, size, "%s|%s|%s",
             (flags & ROUTER_LSA_VIRTUAL) ? "V" : "-",
             (flags & ROUTER_LSA_EXTERNAL) ? "E" : "-",
             (flags & ROUTER_LSA_BORDER) ? "B" : "-");

    return buf;
}

static void my_ospf_router_lsa_dump(struct stream *s, uint16_t length)
{
    char buf[BUFSIZ];
    struct router_lsa *rl;
    int i, len;

    rl = (struct router_lsa *)stream_pnt(s);

    printf("  Router-LSA \n");
    printf("    flags %s \n",
           my_ospf_router_lsa_flags_dump(rl->flags, buf, BUFSIZ));
    printf("    # links %d \n", ntohs(rl->links));

    len = ntohs(rl->header.length) - OSPF_LSA_HEADER_SIZE - 4;
    for (i = 0; len > 0; i++) {
        printf("    Link ID %s \n", inet_ntoa(rl->link[i].link_id));
        printf("    Link Data %s \n",
               inet_ntoa(rl->link[i].link_data));
        printf("    Type %d \n", (uint8_t)rl->link[i].type);
        printf("    TOS %d \n", (uint8_t)rl->link[i].tos);
        printf("    metric %d \n", ntohs(rl->link[i].metric));

        len -= 12;
    }
}

static void my_ospf_network_lsa_dump(struct stream *s, uint16_t length)
{
    struct network_lsa *nl;
    int i, cnt;

    nl = (struct network_lsa *)stream_pnt(s);
    cnt = (ntohs(nl->header.length) - (OSPF_LSA_HEADER_SIZE + 4)) / 4;

    printf("  Network-LSA \n");
    printf("    Network Mask %s \n", inet_ntoa(nl->mask));
    printf("    # Attached Routers %d \n", cnt);
    for (i = 0; i < cnt; i++)
        printf("      Attached Router %s \n",
               inet_ntoa(nl->routers[i]));
}

static void my_ospf_summary_lsa_dump(struct stream *s, uint16_t length)
{
    struct summary_lsa *sl;
    //int size;
    //int i;

    sl = (struct summary_lsa *)stream_pnt(s);

    printf("  Summary-LSA \n");
    printf("    Network Mask %s \n", inet_ntoa(sl->mask));

    //size = ntohs(sl->header.length) - OSPF_LSA_HEADER_SIZE - 4;
    /*for (i = 0; size > 0; size -= 4, i++)
        printf("    TOS=%d metric %d", sl->tos,
                   GET_METRIC(sl->metric)); TODO: FIX*/
}

static void my_ospf_as_external_lsa_dump(struct stream *s, uint16_t length)
{
    struct as_external_lsa *al;
    //int size;
    //int i;

    al = (struct as_external_lsa *)stream_pnt(s);
    printf("  %s \n", ospf_lsa_type_msg[al->header.type].str);
    printf("    Network Mask %s \n", inet_ntoa(al->mask));

    //size = ntohs(al->header.length) - OSPF_LSA_HEADER_SIZE - 4;
    /*for (i = 0; size > 0; size -= 12, i++) {
        printf("    bit %s TOS=%d metric %d",
                   IS_EXTERNAL_METRIC(al->e[i].tos) ? "E" : "-",
                   al->e[i].tos & 0x7f, GET_METRIC(al->e[i].metric));
        printf("    Forwarding address %s",
                   inet_ntoa(al->e[i].fwd_addr));
        printf("    External Route Tag %" ROUTE_TAG_PRI,
                   al->e[i].route_tag);
    } TODO: FIX*/
}

static void my_ospf_lsa_header_list_dump(struct stream *s, uint16_t length)
{
    struct lsa_header *lsa;

    printf("  # LSA Headers %d \n", length / OSPF_LSA_HEADER_SIZE);

    while (length > 0) {
        lsa = (struct lsa_header *)stream_pnt(s);
        my_ospf_lsa_header_dump(lsa);

        stream_forward_getp(s, OSPF_LSA_HEADER_SIZE);
        length -= OSPF_LSA_HEADER_SIZE;
    }
}

static void my_ospf_packet_db_desc_dump(struct stream *s, uint16_t length)
{
    struct ospf_db_desc *dd;
    //char dd_flags[8];

    uint32_t gp;

    gp = stream_get_getp(s);
    dd = (struct ospf_db_desc *)stream_pnt(s);

    printf("Database Description \n");
    printf("  Interface MTU %d \n", ntohs(dd->mtu));
    printf("  Options %d (%s) \n", dd->options,
               ospf_options_dump(dd->options));
    /*printf("  Flags %d (%s)", dd->flags,
               ospf_dd_flags_dump(dd->flags, dd_flags, sizeof dd_flags));*/
    printf("  Sequence Number 0x%08lx \n",
               (unsigned long)ntohl(dd->dd_seqnum));

    length -= OSPF_HEADER_SIZE + OSPF_DB_DESC_MIN_SIZE;

    stream_forward_getp(s, OSPF_DB_DESC_MIN_SIZE);

    my_ospf_lsa_header_list_dump(s, length);

    stream_set_getp(s, gp);
}

static void my_ospf_packet_ls_req_dump(struct stream *s, uint16_t length)
{
    uint32_t sp;
    uint32_t ls_type;
    struct in_addr ls_id;
    struct in_addr adv_router;

    sp = stream_get_getp(s);

    length -= OSPF_HEADER_SIZE;

    printf("Link State Request \n");
    printf("  # Requests %d \n", length / 12);

    for (; length > 0; length -= 12) {
        ls_type = stream_getl(s);
        ls_id.s_addr = stream_get_ipv4(s);
        adv_router.s_addr = stream_get_ipv4(s);

        printf("  LS type %d \n", ls_type);
        printf("  Link State ID %s \n", inet_ntoa(ls_id));
        printf("  Advertising Router %s \n", inet_ntoa(adv_router));
    }

    stream_set_getp(s, sp);
}

static void my_ospf_packet_ls_upd_dump(struct stream *s, uint16_t length)
{
    uint32_t sp;
    struct lsa_header *lsa;
    int lsa_len;
    uint32_t count;

    length -= OSPF_HEADER_SIZE;

    sp = stream_get_getp(s);

    count = stream_getl(s);
    length -= 4;

    printf("Link State Update \n");
    printf("  # LSAs %d \n", count);

    while (length > 0 && count > 0) {
        if (length < OSPF_HEADER_SIZE || length % 4 != 0) {
            printf("  Remaining %d bytes; Incorrect length. \n",
                   length);
            break;
        }

        lsa = (struct lsa_header *)stream_pnt(s);
        lsa_len = ntohs(lsa->length);
        my_ospf_lsa_header_dump(lsa);

        switch (lsa->type) {
            case OSPF_ROUTER_LSA:
                my_ospf_router_lsa_dump(s, length);
                break;
            case OSPF_NETWORK_LSA:
                my_ospf_network_lsa_dump(s, length);
                break;
            case OSPF_SUMMARY_LSA:
            case OSPF_ASBR_SUMMARY_LSA:
                my_ospf_summary_lsa_dump(s, length);
                break;
            case OSPF_AS_EXTERNAL_LSA:
                my_ospf_as_external_lsa_dump(s, length);
                break;
            case OSPF_AS_NSSA_LSA:
                my_ospf_as_external_lsa_dump(s, length);
                break;
            case OSPF_OPAQUE_LINK_LSA:
            case OSPF_OPAQUE_AREA_LSA:
            case OSPF_OPAQUE_AS_LSA:
                my_ospf_opaque_lsa_dump(s, length);
                break;
            default:
                break;
        }

        stream_forward_getp(s, lsa_len);
        length -= lsa_len;
        count--;
    }

    stream_set_getp(s, sp);
}

static void my_ospf_packet_ls_ack_dump(struct stream *s, uint16_t length)
{
    uint32_t sp;

    length -= OSPF_HEADER_SIZE;
    sp = stream_get_getp(s);

    printf("Link State Acknowledgment \n");
    my_ospf_lsa_header_list_dump(s, length);

    stream_set_getp(s, sp);
}

void my_ospf_header_dump(struct ospf_header *ospfh)
{
    printf("Header \n");
    printf("  Version %d \n", ospfh->version);
    printf("  Type %d \n", ospfh->type);
    printf("  Packet Len %d \n", ntohs(ospfh->length));
    printf("  Router ID %s \n", inet_ntoa(ospfh->router_id));
    printf("  Area ID %s \n", inet_ntoa(ospfh->area_id));
    printf("  Checksum 0x%x \n", ntohs(ospfh->checksum));
}

static void my_ospf_packet_hello_dump(struct stream *s, uint16_t length)
{
    struct ospf_hello *hello;
    int i;

    hello = (struct ospf_hello *)stream_pnt(s);

    printf("Hello \n");
    printf("  NetworkMask %s \n", inet_ntoa(hello->network_mask));
    printf("  HelloInterval %d \n", ntohs(hello->hello_interval));
    printf("  Options %d (%s) \n", hello->options,
           ospf_options_dump(hello->options));
    printf("  RtrPriority %d \n", hello->priority);
    printf("  RtrDeadInterval %ld \n",
           (unsigned long)ntohl(hello->dead_interval));
    printf("  DRouter %s \n", inet_ntoa(hello->d_router));
    printf("  BDRouter %s \n", inet_ntoa(hello->bd_router));

    length -= OSPF_HEADER_SIZE + OSPF_HELLO_MIN_SIZE;
    printf("  # Neighbors %d \n", length / 4);
    for (i = 0; length > 0; i++, length -= sizeof(struct in_addr))
        printf("    Neighbor %s \n", inet_ntoa(hello->neighbors[i]));
}

void my_ospf_packet_dump(struct stream *s)
{
    struct ospf_header *ospfh;
    unsigned long gp;

    /* Preserve pointer. */
    gp = stream_get_getp(s);

    /* OSPF Header dump. */
    ospfh = (struct ospf_header *)stream_pnt(s);

    /* Show OSPF header detail. */
    my_ospf_header_dump(ospfh);
    stream_forward_getp(s, OSPF_HEADER_SIZE);

    switch (ospfh->type) {
        case OSPF_MSG_HELLO:
            my_ospf_packet_hello_dump(s, ntohs(ospfh->length));
            break;
            case OSPF_MSG_DB_DESC:
                my_ospf_packet_db_desc_dump(s, ntohs(ospfh->length));
                break;
            case OSPF_MSG_LS_REQ:
                my_ospf_packet_ls_req_dump(s, ntohs(ospfh->length));
                break;
        case OSPF_MSG_LS_UPD:
            my_ospf_packet_ls_upd_dump(s, ntohs(ospfh->length));
            break;
            case OSPF_MSG_LS_ACK:
                my_ospf_packet_ls_ack_dump(s, ntohs(ospfh->length));
                break;
        default:
            break;
    }

    stream_set_getp(s, gp);
}
