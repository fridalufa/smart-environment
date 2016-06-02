#ifndef SMART_ENV_SETUPH
#define SMART_ENV_SETUPH

#include "net/gnrc/rpl.h"
#include "net/ipv6/addr.h"

kernel_pid_t get_first_interface(void);
bool rpl_init(kernel_pid_t iface_pid);
bool rpl_root_init(char* link_addr, kernel_pid_t iface_pid);

bool add_address_to_interface(char* link_addr, kernel_pid_t iface_pid, uint8_t add_flags);
bool add_multicast_address(char* link_addr, kernel_pid_t iface_pid);

#endif