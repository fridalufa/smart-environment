#include "temperature_manager.h"
#include "periph/gpio.h"
#include <stdio.h>

static float temperatureWindow[WINDOW_SIZE];
static int record_count = 0;
static int current_index = 0;

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

    int average_temp = getAverage();

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
    (void) scratch;
    (void) inpkt;
    (void) outpkt;
    (void) id_hi;
    (void) id_lo;

    return 0;
}