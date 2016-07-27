#include "temperature_manager.h"

int YELLOW_TEMP = DEFAULT_YELLOW_TEMP;
int RED_TEMP = DEFAULT_RED_TEMP;

bool is_registered = false;

static unsigned char stream_data[100];
static cbor_stream_t stream = {stream_data, sizeof(stream_data), 0};

#define EXTERNAL_SERVER_ADDR "fe80::30f8:3bff:fe7b:87c8"

int setLed(gpio_t led)
{
    (void) led;
    gpio_clear(LEDRED_PIN);
    gpio_clear(LEDYELLOW_PIN);
    gpio_clear(LEDGREEN_PIN);

    gpio_set(led);

    return 0;
}

int getTemperature(coap_rw_buffer_t* scratch,
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

    return atoi(payloadString);
}

int registerGateway(int argc, char** argv)
{
    memset(stream_data, 0, sizeof(stream_data));

    cbor_clear(&stream);
    cbor_init(&stream, stream_data, sizeof(stream_data));

    cbor_serialize_array(&stream, 2);
    cbor_serialize_unicode_string(&stream, "1");
    cbor_serialize_array(&stream, 2);
    cbor_serialize_unicode_string(&stream, "Temperatur");
    cbor_serialize_unicode_string(&stream, "float");

    cbor_destroy(&stream);

    ipv6_addr_t target;
    ipv6_addr_from_str(&target, EXTERNAL_SERVER_ADDR);

    coap_client_send(&target, COAP_METHOD_POST, COAP_TYPE_CON, "register", (char*)stream_data, COAP_CONTENTTYPE_TEXT_PLAIN);

    coap_client_receive();

    is_registered = true;

    return 0;
}

int handleData(coap_rw_buffer_t* scratch,
               const coap_packet_t* inpkt,
               coap_packet_t* outpkt,
               uint8_t id_hi, uint8_t id_lo)
{
    int temperature = getTemperature(scratch, inpkt, outpkt, id_hi, id_lo);

    printf("Received temperature %d\n", temperature);

    addValue(temperature);

    int average_temp = getSum() / getCount();

    printf("Average temperature %d\n", average_temp);

    if (average_temp < YELLOW_TEMP * 100) {
        setLed(LEDGREEN_PIN);
    } else {
        if (average_temp < RED_TEMP * 100) {
            setLed(LEDYELLOW_PIN);
        } else {
            setLed(LEDRED_PIN);
        }
    }

    if (is_registered == false) {
        return 0;
    }

    memset(stream_data, 0, sizeof(stream_data));

    cbor_clear(&stream);
    cbor_init(&stream, stream_data, sizeof(stream_data));

    cbor_serialize_array(&stream, 2);
    cbor_serialize_unicode_string(&stream, "1");
    cbor_serialize_int(&stream, 2300);

    cbor_destroy(&stream);

    ipv6_addr_t target;
    ipv6_addr_from_str(&target, EXTERNAL_SERVER_ADDR);

    coap_client_send(&target, COAP_METHOD_PUT, COAP_TYPE_CON, "data", (char*)stream_data, COAP_CONTENTTYPE_TEXT_PLAIN);

    coap_client_receive();

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

    // create cbor stream
    cbor_stream_t stream = {data, sizeof(data), 0};

    size_t map_length;
    // read out length of the map
    size_t offset = cbor_deserialize_map(&stream, 0, &map_length);
    int key1, key2, key3;
    char medtemp[8], hightemp[8], windowsize[8];
    // read key 1
    offset += cbor_deserialize_int(&stream, offset, &key1);
    // read medtemp
    offset += cbor_deserialize_byte_string(&stream, offset, medtemp, sizeof(medtemp));
    // read key 2
    offset += cbor_deserialize_int(&stream, offset, &key2);
    // read hightemp
    offset += cbor_deserialize_byte_string(&stream, offset, hightemp, sizeof(hightemp));
    // read key 3
    offset += cbor_deserialize_int(&stream, offset, &key3);
    // read windowsize
    offset += cbor_deserialize_byte_string(&stream, offset, windowsize, sizeof(windowsize));

    cbor_destroy(&stream);

    puts("NEW CONFIGURATION RECEIVED");

    YELLOW_TEMP = atoi(medtemp);
    RED_TEMP = atoi(hightemp);

    printf("YELLOW_TEMP set to: %d\n", YELLOW_TEMP);
    printf("RED_TEMP set to: %d\n", RED_TEMP);

    if (strcmp(windowsize, "-") != 0) {
        printf("Set windowsize to: %d\n", atoi(windowsize));
        setWindowSize(atoi(windowsize));
    }

    return 0;
}