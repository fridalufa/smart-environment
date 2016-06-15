#include "temperature_manager.h"
#include "periph/gpio.h"
#include <stdio.h>

static float temperatureWindow[WINDOW_SIZE];
static int record_count = 0;
static int current_index = 0;

int manage_temperature(int temperature)
{
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

    if (average_temp > FAN_TEMP * 100) {
        gpio_set(FAN_PIN);
    } else {
        gpio_clear(FAN_PIN);
    }

    return 0;
}