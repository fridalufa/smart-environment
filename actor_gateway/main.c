#include <stdio.h>

#include "shell.h"
#include "msg.h"
#include "thread.h"

#include "time.h"
#include "coap_server.h"

#include "setup.h"
#include "coap_client.h"

#include "gateway_handler.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

#define SERVER_QUEUE_SIZE     (8)
static msg_t _server_msg_queue[SERVER_QUEUE_SIZE];

static char _rcv_stack_buf[THREAD_STACKSIZE_DEFAULT];

char* rpl_root_addr = "2001:db8::1";
kernel_pid_t iface_pid = 6;

#define MULTICAST_ADDR "ff02::13"

int coap_client(int argc, char** argv);
int registerGateway(int argc, char** argv);
int selected_interface(int argc, char** argv);
int mkroot(int argc, char** argv);

static const shell_command_t shell_commands[] = {
    { "coap", "Send a coap request to the server and display response", coap_client },
    { "register", "Let the server register you via a CoAP POST request", registerGateway },
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

    if (argc < 2) {
        printf("Usage: %s <server or multicast address>\n", argv[0]);
        return 1;
    }

    ipv6_addr_t target;
    ipv6_addr_from_str(&target, argv[1]);

    coap_client_send(&target, COAP_METHOD_POST, COAP_TYPE_NONCON, "register", NULL, 0);

    // coap_client_receive();

    return 0;
}

int registerGateway(int argc, char** argv)
{

    if (argc < 3) {
        printf("Usage: %s <server or multicast address> <name>\n", argv[0]);
        return 1;
    }

    ipv6_addr_t target;
    ipv6_addr_from_str(&target, argv[1]);

    coap_client_send(&target, COAP_METHOD_POST, COAP_TYPE_CON, "register", argv[2], COAP_CONTENTTYPE_TEXT_PLAIN);

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