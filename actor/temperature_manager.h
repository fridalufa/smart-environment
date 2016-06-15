#ifndef TEMPARATURE_MAN_H
#define TEMPARATURE_MAN_H

#include "periph/gpio.h"

#define WINDOW_SIZE (1)

#define YELLOW_TEMP (22)
#define RED_TEMP (25)

#define LEDRED_PIN            GPIO_PIN(PB, 23)
#define LEDYELLOW_PIN         GPIO_PIN(PA, 15)
#define LEDGREEN_PIN          GPIO_PIN(PB, 22)

int manage_temperature(int temperature);
int setLed(gpio_t led);

#endif