#ifndef TEMPARATURE_MAN_H
#define TEMPARATURE_MAN_H

#include "periph/gpio.h"

#define WINDOW_SIZE (1)

#define FAN_TEMP (30)

#define FAN_PIN            GPIO_PIN(PA, 15)

int manage_temperature(int temperature);

#endif