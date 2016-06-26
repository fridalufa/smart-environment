#include "endpoints.h"

#include <coap.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#define MAX_RESPONSE_LEN 500
static uint8_t response[MAX_RESPONSE_LEN] = { 0 };

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

static int handle_configuration(coap_rw_buffer_t* scratch,
                                const coap_packet_t* inpkt,
                                coap_packet_t* outpkt,
                                uint8_t id_hi, uint8_t id_lo);

static int handle_data(coap_rw_buffer_t* scratch,
                       const coap_packet_t* inpkt,
                       coap_packet_t* outpkt,
                       uint8_t id_hi, uint8_t id_lo);

static int default_handler(coap_rw_buffer_t* scratch,
                           const coap_packet_t* inpkt,
                           coap_packet_t* outpkt,
                           uint8_t id_hi, uint8_t id_lo);

int (*dataHandler)(coap_rw_buffer_t*,
                   const coap_packet_t*,
                   coap_packet_t*,
                   uint8_t, uint8_t) = &default_handler;

static const coap_endpoint_path_t path_well_known_core = { 2, { ".well-known", "core" } };

static const coap_endpoint_path_t path_riot_board = { 2, { "riot", "board" } };

static const coap_endpoint_path_t path_greet = { 1, { "greet"} };

static const coap_endpoint_path_t path_config = { 1, { "configuration"} };

static const coap_endpoint_path_t path_data = { 1, { "data"} };

const coap_endpoint_t endpoints[] = {
  {
    COAP_METHOD_GET,  handle_get_well_known_core,
    &path_well_known_core, "ct=40"
  },
  {
    COAP_METHOD_GET,  handle_get_riot_board,
    &path_riot_board,      "ct=0"
  },
  {
    COAP_METHOD_POST,  handle_greet,
    &path_greet,      "ct=0"
  },
  {
    COAP_METHOD_POST,  handle_data,
    &path_data,      "ct=0"
  },
  {
    COAP_METHOD_POST,  handle_configuration,
    &path_config,      "ct=0"
  },
  /* marks the end of the endpoints array: */
  { (coap_method_t)0, NULL, NULL, NULL }
};

static int handle_get_well_known_core(coap_rw_buffer_t* scratch,
                                      const coap_packet_t* inpkt, coap_packet_t* outpkt,
                                      uint8_t id_hi, uint8_t id_lo)
{
  // Clear out the previous response, if any
  memset(response, 0, sizeof(response));

  char* rsp = (char*)response;
  int len = sizeof(response);
  const coap_endpoint_t* ep = endpoints;
  int i;

  len--; // Null-terminated string

  while (NULL != ep->handler) {
    if (NULL == ep->core_attr) {
      ep++;
      continue;
    }

    if (0 < strlen(rsp)) {
      strncat(rsp, ",", len);
      len--;
    }

    strncat(rsp, "<", len);
    len--;

    for (i = 0; i < ep->path->count; i++) {
      strncat(rsp, "/", len);
      len--;

      strncat(rsp, ep->path->elems[i], len);
      len -= strlen(ep->path->elems[i]);
    }

    strncat(rsp, ">;", len);
    len -= 2;

    strncat(rsp, ep->core_attr, len);
    len -= strlen(ep->core_attr);

    ep++;
  }

  return coap_make_response(scratch, outpkt, (const uint8_t*)response,
                            strlen(rsp), id_hi, id_lo, &inpkt->tok,
                            COAP_RSPCODE_CONTENT,
                            COAP_CONTENTTYPE_APPLICATION_LINKFORMAT);
}

static int handle_get_riot_board(coap_rw_buffer_t* scratch,
                                 const coap_packet_t* inpkt, coap_packet_t* outpkt,
                                 uint8_t id_hi, uint8_t id_lo)
{
  const char* riot_name = RIOT_BOARD;
  int len = strlen(RIOT_BOARD);

  memcpy(response, riot_name, len);

  return coap_make_response(scratch, outpkt, (const uint8_t*)response, len,
                            id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CONTENT,
                            COAP_CONTENTTYPE_TEXT_PLAIN);
}

static int handle_greet(coap_rw_buffer_t* scratch,
                        const coap_packet_t* inpkt, coap_packet_t* outpkt,
                        uint8_t id_hi, uint8_t id_lo)
{

  coap_buffer_t payload = inpkt->payload;

  const char* name = (char*) payload.p;

  // It is important, to take the length information of the payload buffer into account.
  // Otherwise there may be more characters before there is a null termination.
  char payloadString[payload.len + 1];
  strncpy(payloadString, name, payload.len);
  payloadString[payload.len] = '\0';

  char buf[MAX_RESPONSE_LEN];
  sprintf(buf, "Hello %s!", payloadString);
  int len = strlen(buf);

  memcpy(response, buf, len);

  return coap_make_response(scratch, outpkt, (const uint8_t*)response, len,
                            id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CONTENT,
                            COAP_CONTENTTYPE_TEXT_PLAIN);
}

static int handle_configuration(coap_rw_buffer_t* scratch,
                                const coap_packet_t* inpkt, coap_packet_t* outpkt,
                                uint8_t id_hi, uint8_t id_lo)
{
  (void)scratch;
  (void)outpkt;
  (void)id_hi;
  (void)id_lo;

  coap_buffer_t payload = inpkt->payload;

  const char* plaintext = (char*) payload.p;

  // It is important, to take the length information of the payload buffer into account.
  // Otherwise there may be more characters before there is a null termination.
  char payloadString[payload.len + 1];
  strncpy(payloadString, plaintext, payload.len);
  payloadString[payload.len] = '\0';



  return 0;
}

int set_data_handler(int (*functionPtr)(coap_rw_buffer_t*,
                                        const coap_packet_t*,
                                        coap_packet_t*,
                                        uint8_t, uint8_t))
{
  puts("DataHandler was set!");
  dataHandler = functionPtr;

  return 0;
}

int default_handler(coap_rw_buffer_t* scratch,
                    const coap_packet_t* inpkt, coap_packet_t* outpkt,
                    uint8_t id_hi, uint8_t id_lo)
{
  (void)scratch;
  (void)outpkt;
  (void)inpkt;
  (void)id_hi;
  (void)id_lo;

  puts("Received Data!");

  return 0;
}

static int handle_data(coap_rw_buffer_t* scratch,
                       const coap_packet_t* inpkt, coap_packet_t* outpkt,
                       uint8_t id_hi, uint8_t id_lo)
{
  return dataHandler(scratch, inpkt, outpkt, id_hi, id_lo);
}