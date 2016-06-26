#include "coap_server.h"
#include "endpoints.h"

static uint8_t _udp_buf[512];   /* udp read buffer (max udp payload size) */
uint8_t scratch_raw[1024];      /* microcoap scratch buffer */

coap_rw_buffer_t scratch_buf = { scratch_raw, sizeof(scratch_raw) };

#define COAP_SERVER_PORT    (5683)

void coap_server_loop(int (*handle)(coap_rw_buffer_t*,
                                    const coap_packet_t*,
                                    coap_packet_t*,
                                    uint8_t, uint8_t))
{
    uint8_t laddr[16] = { 0 };
    uint8_t raddr[16] = { 0 };
    size_t raddr_len;
    uint16_t rport;

    (void) handle;

    /*
    uint8_t id_lo = 2;

    THIS RESULTS IN SEG-FAULT:
    handle(NULL, NULL, NULL, id_lo, id_lo);

    set_data_handler(handleFunction);
    */

    conn_udp_t conn;

    int rc = conn_udp_create(&conn, laddr, sizeof(laddr), AF_INET6, COAP_SERVER_PORT);

    while (1) {
        DEBUG("Waiting for incoming UDP packet...\n");
        rc = conn_udp_recvfrom(&conn, (char*)_udp_buf, sizeof(_udp_buf), raddr, &raddr_len, &rport);
        if (rc < 0) {
            DEBUG("Error in conn_udp_recvfrom(). rc=%u\n", rc);
            continue;
        }

        size_t n = rc;

        coap_packet_t pkt;
        DEBUG("Received packet: ");
        coap_dump(_udp_buf, n, true);
        DEBUG("\n");

        /* parse UDP packet to CoAP */
        if (0 != (rc = coap_parse(&pkt, _udp_buf, n))) {
            DEBUG("Bad packet rc=%d\n", rc);
        } else {
            coap_packet_t rsppkt;
            DEBUG("content:\n");
            coap_dumpPacket(&pkt);

            /* handle CoAP request */
            coap_handle_req(&scratch_buf, &pkt, &rsppkt);
        }
    }
}