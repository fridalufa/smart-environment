#ifndef SMART_ENV_SETUPH
#define SMART_ENV_SETUPH

#include "net/gnrc/rpl.h"
#include "net/ipv6/addr.h"

kernel_pid_t get_first_interface(void);
bool rpl_init(kernel_pid_t iface_pid);
bool rpl_root_init(char* link_addr, kernel_pid_t iface_pid);

#endif