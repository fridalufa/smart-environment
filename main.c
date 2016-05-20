#include <stdio.h>

#include "shell.h"
#include "msg.h"
#include "setup.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

char* link_addr = "2001:db8::1";
kernel_pid_t iface_pid = 6;

int main(void)
{
    printf("Hello Smart Environment!\n");

    // Message Queue for receiving potentially fast incoming networking packets
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    bool isRootNode = (getenv("MODE") != NULL) && (strcmp(getenv("MODE"), "root") == 0);

    bool initialized = true;

    if (isRootNode) {
        initialized = rpl_root_init(link_addr, iface_pid);
    } else {
        initialized = rpl_init(iface_pid);
    }

    if (!initialized) {
        puts("error: unable to initialize RPL");
        return 1;
    }

    // Taken from the hello world example!
    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}