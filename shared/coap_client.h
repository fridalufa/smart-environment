#ifndef SMART_ENV_COAP_CLIENTH
#define SMART_ENV_COAP_CLIENTH

#include "net/af.h"
#include "net/conn/udp.h"
#include <string.h>
#include <stdio.h>
#include <random.h>
#include <coap.h>

#ifdef MICROCOAP_DEBUG
#define ENABLE_DEBUG (1)
#else
#define ENABLE_DEBUG (0)
#endif
#include "debug.h"

void coap_client_send_simple(ipv6_addr_t* target, coap_method_t method, char* endpoint);

void coap_client_send(ipv6_addr_t* target, coap_method_t method, coap_msgtype_t type,
                      char* endpoint, char* payload, coap_content_type_t ct);

void coap_client_receive(void);

#endif