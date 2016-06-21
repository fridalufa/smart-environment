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

int setLed(gpio_t led)
{
    gpio_clear(LEDRED_PIN);
    gpio_clear(LEDYELLOW_PIN);
    gpio_clear(LEDGREEN_PIN);

    gpio_set(led);

    return 0;
}