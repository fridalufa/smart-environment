#include "setup.h"

/**
 * Add a global IPv6 address for the root node
 * @param  link_addr The desired IPv6 address
 * @param  iface_pid ID of the interface
 * @return           true on success, false on failure
 */
bool _configure_global_ipv6_address(char* link_addr, kernel_pid_t iface_pid)
{
    return add_address_to_interface(link_addr, iface_pid, GNRC_IPV6_NETIF_ADDR_FLAGS_UNICAST);
}

bool add_multicast_address(char* link_addr, kernel_pid_t iface_pid)
{
    return add_address_to_interface(link_addr, iface_pid, GNRC_IPV6_NETIF_ADDR_FLAGS_NON_UNICAST);
}

bool add_address_to_interface(char* link_addr, kernel_pid_t iface_pid, uint8_t add_flags)
{

    // add a global IPv6 address for the root node
    uint8_t prefix_len, flags = 0;
    ipv6_addr_t* ifaddr;
    ipv6_addr_t addr_dec;

    if (ipv6_addr_from_str(&addr_dec, link_addr) == NULL) {
        puts("error: unable to parse IPv6 address.");
        return false;
    };

    prefix_len = ipv6_addr_split(link_addr, '/', 64);
    flags = GNRC_IPV6_NETIF_ADDR_FLAGS_NDP_AUTO | add_flags;

    if ((ifaddr = gnrc_ipv6_netif_add_addr(iface_pid, &addr_dec, prefix_len, flags)) == NULL) {
        printf("error: unable to add IPv6 address\n");
        return false;
    }

    // Address shall be valid infinitely
    gnrc_ipv6_netif_addr_get(ifaddr)->valid = UINT32_MAX;
    // Address shall be preferred infinitely
    gnrc_ipv6_netif_addr_get(ifaddr)->preferred = UINT32_MAX;

    return true;
}

/**
 * Initialize this node to join a RPL routing network.
 * @param  iface_pid  The ID for the RPL interface
 * @return            true on successful init, false otherwise
 */
bool rpl_init(kernel_pid_t iface_pid)
{

    // Check if interface exists
    gnrc_ipv6_netif_t* entry = NULL;

    entry = gnrc_ipv6_netif_get(iface_pid);

    if (entry == NULL) {
        puts("unknown interface specified");
        return false;
    }

    // RPL init
    gnrc_rpl_init(iface_pid);

    return true;
}

/**
 * Initialize this node as a RPL root
 * @param  link_addr The address of the global interface, that'll be created
 * @param  iface_pid The id of the interface for the RPL network
 * @return           true on success, false otherwise
 */
bool rpl_root_init(char* link_addr, kernel_pid_t iface_pid)
{
    // TODO: this code is in fact duplicated
    ipv6_addr_t addr;
    if (ipv6_addr_from_str(&addr, link_addr) == NULL) {
        puts("error: unable to parse IPv6 address.");
        return false;
    };

    _configure_global_ipv6_address(link_addr, iface_pid);

    // Initialize root node
    if (gnrc_rpl_root_init(0, &addr, true, false) == NULL) {
        puts("failed to initialize node as root node");
        return false;
    }

    return true;
}

kernel_pid_t get_first_interface(void)
{
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];

    size_t numof = gnrc_netif_get(ifs);

    if (numof > 0) {
        return ifs[0];
    }

    return -1;
}