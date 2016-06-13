#include "temperature_manager.h"
#include <stdio.h>
#include <board.h>

static float temperatureWindow[WINDOW_SIZE];
static int record_count = 0;
static int current_index = 0;

int manage_temperature(float temperature)
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

    double average_temp = temperature_sum / (float)record_count;
    printf("Average temperature: %f\n", average_temp);

    if (average_temp < MAX_TEMP) {
        _native_LED_GREEN_ON();
    } else {
        _native_LED_GREEN_OFF();
    }

    return 0;
}