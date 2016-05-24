#include "coap_client.h"

#define CLIENT_PORT 9293
#define COAP_SERVER_PORT 5683

static uint8_t _udp_buf[512];

// TODO: refactor the header files, maybe create a single coap.h instead of a separate
// header for both client and server

/**
 * @brief constructs a CoAP packet and sends it to the targets endpoint via UDP
 *
 * @param      target    The targets adress
 * @param[in]  method    The request method
 * @param      endpoint  The endpoint URI
 * @param      payload   Optional payload
 */
void coap_client_send(ipv6_addr_t* target, coap_method_t method, char* endpoint, char* payload)
{

    char uri[strlen(endpoint)];
    strcpy(uri, endpoint);

    // count the amount of parts of the endpoint URI
    int parts = 1;
    for (uint8_t i = 0; i < strlen(uri); i++) {
        if (uri[i] == '/') {
            parts++;
        }
    }

    // construct option array containg the parts of the endpoint URI
    coap_option_t opts[parts];
    char* token = strtok(uri, "/");
    int i = 0;
    while (token != NULL) {

        coap_buffer_t optBuf = {
            (uint8_t*) token,
            strlen(token)
        };
        coap_option_t opt = {0};
        opt.num = 11;
        opt.buf = optBuf;

        opts[i] = opt;

        i++;
        token = strtok(NULL, "/");
    }

    // construct the packet
    coap_packet_t pkt = {0};

    // begin with constructing the header
    pkt.hdr = (coap_header_t) {1, COAP_TYPE_CON, 0, method, {0, 0}};
    // generate a random 16 bit message id
    uint32_t r = random_uint32_range(0, 0xFFFF);
    uint8_t id[] = {(r & 0xFF00) >> 8, r & 0xFF};
    memcpy(pkt.hdr.id, id, 2);

    // add options
    pkt.numopts = parts;
    memcpy(pkt.opts, opts , parts * sizeof(opts));

    // add the payload if there is one
    if (payload != NULL) {
        coap_buffer_t payloadBuf = {
            (uint8_t*) payload,
            strlen(payload)
        };
        pkt.payload = payloadBuf;
    }

    coap_dumpPacket(&pkt);

    // send the paket over UDP
    int rc = 0;
    size_t pktlen = sizeof(_udp_buf);
    if ((rc = coap_build(_udp_buf, &pktlen, &pkt)) != 0) {
        DEBUG("coap_build failed rc=%d\n", rc);
    } else {
        rc = conn_udp_sendto(_udp_buf, pktlen, NULL, 0, target, 16, AF_INET6, CLIENT_PORT , COAP_SERVER_PORT);
        if (rc < 0) {
            DEBUG("Error sending CoAP reply via udp; %u\n", rc);
        }
    }
}

/**
 * @brief      a simple receive function listening for an incoming udp packet on the client port.
 *             Then the function attempts to parse the packet as a CoAP packet. Upon success, the paylod
 *             will be echoed.
 */
void coap_client_receive(void)
{
    conn_udp_t conn;

    // initialize connection
    uint8_t laddr[16] = {0};
    if (conn_udp_create(&conn, laddr, sizeof(laddr), AF_INET6, CLIENT_PORT) < 0) {
        DEBUG("Error creating udp connection");
    }

    // receive an udp packet (blocking)
    int receivedBytes = conn_udp_recvfrom(&conn, (char*)_udp_buf, sizeof(_udp_buf), NULL, NULL, NULL);
    if (receivedBytes < 0) {
        DEBUG("Error in conn_udp_recvfrom(). rc=%u\n", rc);
    }

    // parse the received udp packet into a CoAP packets
    size_t n = receivedBytes;
    coap_packet_t pkt;
    if (0 != (rc = coap_parse(&pkt, _udp_buf, n))) {
        DEBUG("Bad packet rc=%d\n", rc);
    }

    coap_dumpPacket(&pkt);

    // echo payload
    // TODO: do something useful with the received packet
    if (&pkt.payload != NULL) {
        printf("%s\n", pkt.payload.p);
    }
}