#include "test_handler.h"

int handle(coap_rw_buffer_t* scratch,
           const coap_packet_t* inpkt,
           coap_packet_t* outpkt,
           uint8_t id_hi, uint8_t id_lo)
{
    (void)scratch;
    (void)outpkt;
    (void)inpkt;
    (void)id_hi;
    (void)id_lo;

    //puts("hallo");
    //printf("%d", id_lo);

    /*
        coap_buffer_t payload = inpkt->payload;

        const char* plaintext = (char*) payload.p;

        // It is important, to take the length information of the payload buffer into account.
        // Otherwise there may be more characters before there is a null termination.
        char payloadString[payload.len + 1];
        strncpy(payloadString, plaintext, payload.len);
        payloadString[payload.len] = '\0';

        printf("%d", atoi(payloadString));
    */
    return 0;
}
