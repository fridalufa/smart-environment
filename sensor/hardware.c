#include <stdio.h>
#include "hardware.h"
#include "xtimer.h"

#define ENABLE_DEBUG  (0)
#include "debug.h"

#if I2C_NUMOF > 0

#include "tmp006.h"
tmp006_t sensor;

#else
#include "random.h"
#endif

int temp_sensor_init(void)
{
    int result = 0;

#if I2C_NUMOF > 0
    // Initialize temperature sensor
    result = tmp006_init(&sensor, I2C_0, 0x40, TMP006_CONFIG_CR_AS2);

    DEBUG("Init result: %d\n", result);

#endif

    return result;
}

int temp_sensor_read(void)
{
#if I2C_NUMOF > 0
    int16_t rawtemp = 0, rawvolt = 0;
    float tamb = 0.0, tobj = 0.0;
    uint8_t drdy = 0;

    if (tmp006_set_active(&sensor)) {
        DEBUG("Measurement start failed.\n");
        return 1;
    }

    while (!drdy) {
        if (tmp006_read(&sensor, &rawvolt, &rawtemp, &drdy) == -1) {
            DEBUG("Failed to read temperature\n");
            xtimer_usleep(TMP006_CONVERSION_TIME);
            continue;
        }
    }

    DEBUG("Raw data T: %5d   V: %5d\n", rawtemp, rawvolt);
    tmp006_convert(rawvolt, rawtemp,  &tamb, &tobj);
    DEBUG("Data Tabm: %d   Tobj: %d\n", (int)(tamb * 100), (int)(tobj * 100));

    return (int)(tobj * 100);
#else
    DEBUG("Random temperature value\n");
    return random_uint32_range(0, 40) * 100;
#endif
}