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

int manage_temperature(int temperature)
{
    printf("Received temperature %d\n", temperature);
    if (record_count < WINDOW_SIZE) {
        record_count++;
    }
    if (current_index == WINDOW_SIZE) {
        current_index = 0;
    }

    temperatureWindow[current_index++] = temperature;

    int i;

    float temperature_sum = 0;
    for (i = 0; i < record_count; i++) {
        temperature_sum += temperatureWindow[i];
    }

    int average_temp = temperature_sum / record_count;

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