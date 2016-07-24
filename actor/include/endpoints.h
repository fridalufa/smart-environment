#ifndef SMART_ENV_ENDPOINTSH
#define SMART_ENV_ENDPOINTSH

#include <coap.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "coap_server.h"

#define MAX_RESPONSE_LEN 500

int set_data_handler(DataHandler dh);
int set_config_handler(DataHandler dh);
#endif