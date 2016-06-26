#ifndef SMART_ENV_ENDPOINTSH
#define SMART_ENV_ENDPOINTSH

#include <coap.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_RESPONSE_LEN 500

static int handle_get_well_known_core(coap_rw_buffer_t* scratch,
                                      const coap_packet_t* inpkt,
                                      coap_packet_t* outpkt,
                                      uint8_t id_hi, uint8_t id_lo);

static int handle_get_riot_board(coap_rw_buffer_t* scratch,
                                 const coap_packet_t* inpkt,
                                 coap_packet_t* outpkt,
                                 uint8_t id_hi, uint8_t id_lo);

static int handle_greet(coap_rw_buffer_t* scratch,
                        const coap_packet_t* inpkt,
                        coap_packet_t* outpkt,
                        uint8_t id_hi, uint8_t id_lo);

static int set_data_handler(int (*functionPtr)(coap_rw_buffer_t* scratch,
                            const coap_packet_t* inpkt,
                            coap_packet_t* outpkt,
                            uint8_t id_hi, uint8_t id_lo));

static int handle_data(coap_rw_buffer_t* scratch,
                       const coap_packet_t* inpkt,
                       coap_packet_t* outpkt,
                       uint8_t id_hi, uint8_t id_lo);

#endif