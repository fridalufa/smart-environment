#include "gateway_handler.h"

int DEFAULT_VALUE = 30;

int handleData(coap_rw_buffer_t* scratch,
               const coap_packet_t* inpkt,
               coap_packet_t* outpkt,
               uint8_t id_hi, uint8_t id_lo)
{
    (void)scratch;
    (void)outpkt;
    (void)id_hi;
    (void)id_lo;

    coap_buffer_t payload = inpkt->payload;

    const char* plaintext = (char*) payload.p;

    // It is important, to take the length information of the payload buffer into account.
    // Otherwise there may be more characters before there is a null termination.
    char payloadString[payload.len + 1];
    strncpy(payloadString, plaintext, payload.len);
    payloadString[payload.len] = '\0';

    printf("%d\n", atoi(payloadString) + DEFAULT_VALUE);

    return 0;
}

int handleConfig(coap_rw_buffer_t* scratch,
                 const coap_packet_t* inpkt,
                 coap_packet_t* outpkt,
                 uint8_t id_hi, uint8_t id_lo)
{
    (void)scratch;
    (void)outpkt;
    (void)id_hi;
    (void)id_lo;

    coap_buffer_t payload = inpkt->payload;
    unsigned char* data = (unsigned char*) payload.p;

    cbor_stream_t stream = {data, sizeof(data), 0};

    size_t map_length;
    // read out length of the map
    size_t offset = cbor_deserialize_map(&stream, 0, &map_length);
    int key1, key2, key3;
    char medtemp[8], hightemp[8], windowsize[8];
    // read key 1
    offset += cbor_deserialize_int(&stream, offset, &key1);
    // read value 1
    offset += cbor_deserialize_byte_string(&stream, offset, medtemp, sizeof(medtemp));
    // read key 2
    offset += cbor_deserialize_int(&stream, offset, &key2);
    // read value 2
    offset += cbor_deserialize_byte_string(&stream, offset, hightemp, sizeof(hightemp));
    // read key 2
    offset += cbor_deserialize_int(&stream, offset, &key3);
    // read value 2
    offset += cbor_deserialize_byte_string(&stream, offset, windowsize, sizeof(windowsize));

    cbor_destroy(&stream);

    printf("WINDOWSIZE: %s\n", windowsize);

    return 0;
}