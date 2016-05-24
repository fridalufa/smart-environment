#include <stdio.h>

#include "shell.h"
#include "msg.h"
#include "thread.h"
#include "setup.h"
#include "time.h"
#include "coap_server.h"
#include "coap_client.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

#define SERVER_QUEUE_SIZE     (8)
static msg_t _server_msg_queue[SERVER_QUEUE_SIZE];

static char _rcv_stack_buf[THREAD_STACKSIZE_DEFAULT];

char* link_addr = "2001:db8::1";
kernel_pid_t iface_pid = 6;

int coap_client(int argc, char** argv);

static const shell_command_t shell_commands[] = {
    { "coap", "Send a coap request to the server and display response", coap_client },
    { NULL, NULL, NULL }
};

static void* _coap_server_thread(void* arg);

int main(void)
{

    // initialize RNG
    random_init(time(NULL));

    // Message Queue for receiving potentially fast incoming networking packets
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    bool isRootNode = (getenv("MODE") != NULL) && (strcmp(getenv("MODE"), "root") == 0);

    bool initialized = false;

    if (isRootNode) {
        initialized = rpl_root_init(link_addr, iface_pid);
    } else {
        initialized = rpl_init(iface_pid);
    }

    if (!initialized) {
        puts("error: unable to initialize RPL");
        return 1;
    }

    if (isRootNode) {
        puts("Launching coap server");
        thread_create(_rcv_stack_buf, THREAD_STACKSIZE_DEFAULT, THREAD_PRIORITY_MAIN - 1, 0, _coap_server_thread, NULL , "_coap_server_thread");
    }

    // Taken from the hello world example!
    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}

static void* _coap_server_thread(void* arg)
{
    (void)arg;
    msg_init_queue(_server_msg_queue, SERVER_QUEUE_SIZE);
    puts("Launching server loop");

    coap_server_loop();

    return NULL;
}

int coap_client(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    ipv6_addr_t target;
    ipv6_addr_from_str(&target, "2001:db8::1");

    coap_client_send(&target, COAP_METHOD_GET, ".well-known/core", NULL);

    coap_client_receive();

    return 0;
}