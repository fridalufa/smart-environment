#ifndef TEMPARATURE_MAN_H
#define TEMPARATURE_MAN_H

#include "periph/gpio.h"
#include "periph/pwm.h"

#define WINDOW_SIZE (1)

#define FAN_LOW_TEMP (30)
#define FAN_TEMP (33)

// PWM Konfiguration - Should be revisited because of strange noises
#define MODE        PWM_LEFT
#define FREQU       (10000U)
#define STEPS       (1000U)
// We want to use PA7 for PWM, so we have
#define FAN_CHAN    1
#define FAN_DEVICE  PWM_DEV(0)

#define FAN_PIN            GPIO_PIN(PA, 15)

int manage_temperature(int temperature);
uint32_t init_fan(void);

#endif