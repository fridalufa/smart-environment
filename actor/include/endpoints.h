#ifndef SMART_ENV_ENDPOINTSH
#define SMART_ENV_ENDPOINTSH

#include <coap.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_RESPONSE_LEN 500

int set_data_handler(int (*handle)(coap_rw_buffer_t*,

                                   const coap_packet_t*,
                                   coap_packet_t*,
                                   uint8_t, uint8_t));
#endif