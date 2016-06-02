#include <stdio.h>

#include "shell.h"
#include "msg.h"
#include "thread.h"
#include "../shared/setup.h"
#include "time.h"
#include "coap_server.h"
#include "../shared/coap_client.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

#define SERVER_QUEUE_SIZE     (8)
static msg_t _server_msg_queue[SERVER_QUEUE_SIZE];

static char _rcv_stack_buf[THREAD_STACKSIZE_DEFAULT];

char* rpl_root_addr = "2001:db8::1";
kernel_pid_t iface_pid = 6;

#define MULTICAST_ADDR "ff02::fc"

int coap_client(int argc, char** argv);
int greet(int argc, char** argv);
int selected_interface(int argc, char** argv);
int mkroot(int argc, char** argv);

static const shell_command_t shell_commands[] = {
    { "coap", "Send a coap request to the server and display response", coap_client },
    { "greet", "Let the server greet you via a CoAP POST request", greet },
    { "iface", "Show the interface used for network communication", selected_interface },
    { "mkroot", "Make this node root of the rpl", mkroot },
    { NULL, NULL, NULL }
};

static void* _coap_server_thread(void* arg);

int main(void)
{

    puts("Smart Environment Actor Application");

    // initialize RNG
    random_init(time(NULL));

    // Message Queue for receiving potentially fast incoming networking packets
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    iface_pid = get_first_interface();

    if (iface_pid < 0) {
        puts("Unable to find an interface to use for wireless communication");
        return 1;
    }

    if (!rpl_init(iface_pid)) {
        puts("error: unable to initialize RPL");
        return 1;
    }

    puts("Joining multicast group on addr " MULTICAST_ADDR);

    add_multicast_address(MULTICAST_ADDR, iface_pid);

    puts("Launching coap server");
    thread_create(_rcv_stack_buf, THREAD_STACKSIZE_DEFAULT, THREAD_PRIORITY_MAIN - 1, 0, _coap_server_thread, NULL , "_coap_server_thread");

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

    coap_client_send(&target, COAP_METHOD_GET, ".well-known/core");

    coap_client_receive();

    return 0;
}

int greet(int argc, char** argv)
{

    if (argc < 2) {
        printf("Usage: %s <name>\n", argv[0]);
        return 1;
    }

    ipv6_addr_t target;
    ipv6_addr_from_str(&target, "2001:db8::1");

    coap_client_send_payload(&target, COAP_METHOD_POST, "greet", argv[1], COAP_CONTENTTYPE_TEXT_PLAIN);

    coap_client_receive();

    return 0;
}

int selected_interface(int argc, char** argv)
{
    (void)argc;
    (void)argv;
    kernel_pid_t ifs = get_first_interface();

    printf("Using interface: %d\n", ifs);

    return 0;
}

int mkroot(int argc, char** argv)
{
    (void)argc;
    (void)argv;
    if (!rpl_root_init(rpl_root_addr, iface_pid)) {
        puts("Failed to make this node the rpl root");
        return 1;
    }

    puts("Made this node the rpl root");

    return 0;
}