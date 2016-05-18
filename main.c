#include <stdio.h>

#include "shell.h"
#include "msg.h"
#include "net/gnrc/rpl.h"
#include "net/ipv6/addr.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

char* link_addr = "2001:db8::1";
kernel_pid_t iface_pid = 6;

ipv6_addr_t addr;

int create_interface(void)
{

    // Create global interface 6
    uint8_t prefix_len, flags = 0;
    ipv6_addr_t* ifaddr;

    prefix_len = ipv6_addr_split(link_addr, '/', 64);
    flags = GNRC_IPV6_NETIF_ADDR_FLAGS_NDP_AUTO | GNRC_IPV6_NETIF_ADDR_FLAGS_UNICAST;

    if ((ifaddr = gnrc_ipv6_netif_add_addr(iface_pid, &addr, prefix_len, flags)) == NULL) {
        printf("error: unable to add IPv6 address\n");
        return 1;
    }

    // Address shall be valid infinitely
    gnrc_ipv6_netif_addr_get(ifaddr)->valid = UINT32_MAX;
    // Address shall be preferred infinitely
    gnrc_ipv6_netif_addr_get(ifaddr)->preferred = UINT32_MAX;

    return 0;
}

int rpl_init(void)
{

    if (ipv6_addr_from_str(&addr, link_addr) == NULL) {
        puts("error: unable to parse IPv6 address.");
        return 1;
    };

    // Message Queue for receiving potentially fast incoming networking packets
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    create_interface();

    // Check if interface exists
    gnrc_ipv6_netif_t* entry = NULL;

    entry = gnrc_ipv6_netif_get(iface_pid);

    if (entry == NULL) {
        puts("unknown interface specified");
        return 1;
    }

    // RPL init
    gnrc_rpl_init(6);

    // Initialize root node
    ipv6_addr_from_str(&addr, link_addr);
    gnrc_rpl_root_init(0, &addr, true, false);

    return 0;
}

int main(void)
{
    printf("Hello Smart Environment!\n");

    // Initialize RPL
    if (rpl_init() == 1) {
        // Initialization failed!
        return 1;
    }

    // Taken from the hello world example!
    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}