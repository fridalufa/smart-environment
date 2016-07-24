#ifndef TEMPARATURE_MAN_H
#define TEMPARATURE_MAN_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <coap.h>
#include "slidingwindow.h"
#include "cbor.h"
#include "periph/gpio.h"
#include "periph/pwm.h"

#define WINDOW_SIZE (1)

#define DEFAULT_FAN_LOW_TEMP (30)
#define DEFAULT_FAN_TEMP (33)

// PWM Konfiguration - Should be revisited because of strange noises
#define MODE        PWM_LEFT
#define FREQU       (10000U)
#define STEPS       (1000U)
// We want to use PA7 for PWM, so we have
#define FAN_CHAN    1
#define FAN_DEVICE  PWM_DEV(0)

#define FAN_PIN            GPIO_PIN(PA, 15)

int handleData(coap_rw_buffer_t* scratch,
               const coap_packet_t* inpkt,
               coap_packet_t* outpkt,
               uint8_t id_hi, uint8_t id_lo);
int handleConfig(coap_rw_buffer_t* scratch,
                 const coap_packet_t* inpkt,
                 coap_packet_t* outpkt,
                 uint8_t id_hi, uint8_t id_lo);
uint32_t init_fan(void);

#endif