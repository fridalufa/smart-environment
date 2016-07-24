#ifndef SMART_ENV_COAP_SERVERH
#define SMART_ENV_COAP_SERVERH

#include "net/af.h"
#include "net/conn/udp.h"
#include <coap.h>

#ifdef MICROCOAP_DEBUG
#define ENABLE_DEBUG (1)
#else
#define ENABLE_DEBUG (0)
#endif
#include "debug.h"

typedef int (*DataHandler)(coap_rw_buffer_t*,
                           const coap_packet_t*,
                           coap_packet_t*,
                           uint8_t, uint8_t);
typedef int (*ConfigHandler)(coap_rw_buffer_t*,
                             const coap_packet_t*,
                             coap_packet_t*,
                             uint8_t, uint8_t);

void coap_server_loop(DataHandler dh, ConfigHandler ch);

#endif