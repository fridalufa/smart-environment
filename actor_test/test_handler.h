#ifndef TEST_MAN_H
#define TEST_MAN_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <coap.h>

int handle(coap_rw_buffer_t* scratch,
           const coap_packet_t* inpkt,
           coap_packet_t* outpkt,
           uint8_t id_hi, uint8_t id_lo);

#endif