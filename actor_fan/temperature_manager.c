#include "temperature_manager.h"
#include "periph/gpio.h"
#include <stdio.h>

int FAN_LOW_TEMP = DEFAULT_FAN_LOW_TEMP;
int FAN_TEMP = DEFAULT_FAN_TEMP;

uint32_t init_fan(void)
{
    uint32_t freq = pwm_init(FAN_DEVICE, MODE, FREQU, STEPS);

    /*if (freq != 0) {
        pwm_set(FAN_DEVICE, FAN_CHAN, 300);
    }*/

    return freq;
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

    if (average_temp < FAN_LOW_TEMP * 100) {
        pwm_set(FAN_DEVICE, FAN_CHAN, (int)(STEPS / 2.5));
    } else {
        if (average_temp < FAN_TEMP * 100) {
            pwm_set(FAN_DEVICE, FAN_CHAN, 2 * STEPS / 3);
        } else {
            pwm_set(FAN_DEVICE, FAN_CHAN, STEPS - (STEPS / 10));
        }
    }

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

    FAN_LOW_TEMP = atoi(medtemp);
    FAN_TEMP = atoi(hightemp);

    printf("FAN_LOW_TEMP set to: %d\n", FAN_LOW_TEMP);
    printf("FAN_TEMP set to: %d\n", FAN_TEMP);

    if (strcmp(windowsize, "-") != 0) {
        printf("Set windowsize to: %d\n", atoi(windowsize));
        setWindowSize(atoi(windowsize));
    }

    return 0;
}