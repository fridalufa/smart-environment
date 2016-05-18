#include <stdio.h>

#include "shell.h"
#include "msg.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern int _netif_config(int argc, char** argv);

int rpl_init(void){
	// Message Queue for receiving potentially fast incoming networking packets
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    char* args[] = {"6","add","2001:db8::1"};
    _netif_config(3, args);

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