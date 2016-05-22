#include "coap_client.h"

static uint8_t _udp_buf[512];

// TODO: add a receive function (details are to discuss)
// TODO: refactor the header files, maybe create a single coap.h instead of a separate
// header for both client and server

void coap_client_send(ipv6_addr_t* target, coap_method_t method, char* endpoint, char* payload)
{

    char uri[strlen(endpoint)];
    strcpy(uri, endpoint);

    // count the amount of /
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
    pkt.hdr = (coap_header_t) {1, COAP_TYPE_CON, 0, method, {42, 24}};
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

    // send the paket over udp
    int rc = 0;
    size_t pktlen = sizeof(_udp_buf);
    if ((rc = coap_build(_udp_buf, &pktlen, &pkt)) != 0) {
        DEBUG("coap_build failed rc=%d\n", rc);
    } else {
        DEBUG("Sending packet: ");
        coap_dump(_udp_buf, pktlen, true);

        rc = conn_udp_sendto(_udp_buf, pktlen, NULL, 0, target, 16, AF_INET6, 9293 , 5683);
        if (rc < 0) {
            DEBUG("Error sending CoAP reply via udp; %u\n", rc);
        }
    }
}