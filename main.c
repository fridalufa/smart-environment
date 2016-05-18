#include <stdio.h>

#include "shell.h"
#include "msg.h"
#include "net/gnrc/rpl.h"
#include "net/ipv6/addr.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern int _netif_config(int argc, char** argv);

int rpl_init(void){
	ipv6_addr_t addr;

	// Message Queue for receiving potentially fast incoming networking packets
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    // Create global interface 6
    char* args[] = {"ifconfig","6","add","2001:db8::1"};
    _netif_config(4, args);

    // Check if interface exists
    gnrc_ipv6_netif_t *entry = NULL;
    kernel_pid_t iface_pid = 6;
    entry = gnrc_ipv6_netif_get(iface_pid);

    if (entry == NULL) {
        puts("unknown interface specified");
        return 1;
    }

    // RPL init
	gnrc_rpl_init(6);

	// Initialize root node
	ipv6_addr_from_str(&addr, "2001:db8::1");
	gnrc_rpl_root_init(0, &addr, true, false);

    return 0;
}

int main(void)
{
	printf("Hello Smart Environment!\n");

	rpl_init();

	// Taken from the hello world example!
	printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

	return 0;
}