#ifndef TEMPARATURE_MAN_H
#define TEMPARATURE_MAN_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <coap.h>
#include "slidingwindow.h"
#include "cbor.h"
#include "periph/gpio.h"

#define WINDOW_SIZE (1)

#define DEFAULT_YELLOW_TEMP (30)
#define DEFAULT_RED_TEMP (33)

#define LEDRED_PIN            GPIO_PIN(PB, 23)
#define LEDYELLOW_PIN         GPIO_PIN(PA, 15)
#define LEDGREEN_PIN          GPIO_PIN(PB, 22)

int handleData(coap_rw_buffer_t* scratch,
               const coap_packet_t* inpkt,
               coap_packet_t* outpkt,
               uint8_t id_hi, uint8_t id_lo);
int handleConfig(coap_rw_buffer_t* scratch,
                 const coap_packet_t* inpkt,
                 coap_packet_t* outpkt,
                 uint8_t id_hi, uint8_t id_lo);
int getPayload(coap_rw_buffer_t* scratch,
               const coap_packet_t* inpkt,
               coap_packet_t* outpkt,
               uint8_t id_hi, uint8_t id_lo);
int setLed(gpio_t led);

#endif