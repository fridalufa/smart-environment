#include "temperature_manager.h"

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

int handleData(coap_rw_buffer_t* scratch,
               const coap_packet_t* inpkt,
               coap_packet_t* outpkt,
               uint8_t id_hi, uint8_t id_lo)
{
    int temperature = getTemperature(scratch, inpkt, outpkt, id_hi, id_lo);

    printf("Received temperature %d\n", temperature);

    addValue(temperature);

    int average_temp = getSum() / getCount();

    printf("Average Tempertature: %d\n", average_temp);

    if (average_temp < YELLOW_TEMP * 100) {
        setLed(LEDGREEN_PIN);
    } else {
        if (average_temp < RED_TEMP * 100) {
            setLed(LEDYELLOW_PIN);
        } else {
            setLed(LEDRED_PIN);
        }
    }

    return 0;
}

int handleConfig(coap_rw_buffer_t* scratch,
                 const coap_packet_t* inpkt,
                 coap_packet_t* outpkt,
                 uint8_t id_hi, uint8_t id_lo)
{
    (void) scratch;
    (void) inpkt;
    (void) outpkt;
    (void) id_hi;
    (void) id_lo;

    return 0;
}