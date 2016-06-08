#include <stdio.h>

#include "shell.h"
#include "msg.h"
#include "thread.h"
#include "time.h"

#include "../shared/setup.h"
#include "../shared/coap_client.h"

#include "cbor.h"
#include "tmp006.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

char* link_addr = "2001:db8::1";
kernel_pid_t iface_pid = 6;

static unsigned char stream_data[100];
static cbor_stream_t stream = {stream_data, sizeof(stream_data), 0};

int coap_client(int argc, char** argv);
int greet(int argc, char** argv);
int selected_interface(int argc, char** argv);
int mkroot(int argc, char** argv);
int cbor_test(int argc, char** argv);
int temp(int argc, char** argv);
int send_temperature(int argc, char** argv);

const coap_endpoint_t endpoints[] = {
    /* marks the end of the endpoints array: */
    { (coap_method_t)0, NULL, NULL, NULL }
};

static const shell_command_t shell_commands[] = {
    { "coap", "Send a coap request to the server and display response", coap_client },
    { "greet", "Let the server greet you via a CoAP POST request", greet },
    { "iface", "Show the interface used for network communication", selected_interface },
    { "mkroot", "Make this node root of the rpl", mkroot },
    { "cbor", "Create a test cbor payload", cbor_test},
    { "temp", "Sense temperature", temp},
    { "temperature", "Send temperature to server", send_temperature },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("Smart Environment Sensor Application");

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
        puts("error: Unable to initialize RPL");
        return 1;
    }

    // Taken from the hello world example!
    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}

int coap_client(int argc, char** argv)
{

    if (argc < 2) {
        printf("Usage: %s <server or multicast address>\n", argv[0]);
        return 1;
    }

    ipv6_addr_t target;
    ipv6_addr_from_str(&target, argv[1]);

    coap_client_send(&target, COAP_METHOD_GET, COAP_TYPE_NONCON, ".well-known/core", NULL, 0);

    coap_client_receive();

    return 0;
}

int greet(int argc, char** argv)
{

    if (argc < 3) {
        printf("Usage: %s <server or multicast address> <name>\n", argv[0]);
        return 1;
    }

    ipv6_addr_t target;
    ipv6_addr_from_str(&target, argv[1]);

    coap_client_send(&target, COAP_METHOD_POST, COAP_TYPE_CON, "greet", argv[2], COAP_CONTENTTYPE_TEXT_PLAIN);

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
    if (!rpl_root_init(link_addr, iface_pid)) {
        puts("Failed to make this node the rpl root");
        return 1;
    }

    puts("Made this node the rpl root");

    return 0;
}

int cbor_test(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    int result = 0;

    cbor_clear(&stream);
    cbor_init(&stream, stream_data, sizeof(stream_data));

    cbor_serialize_int(&stream, 10);
    cbor_deserialize_int(&stream, 0, &result);

    printf("Fetched number: %d\n", result);

    return 0;
}

int temp(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    tmp006_t sensor;
    int16_t rawtemp = 0, rawvolt = 0;
    float tamb = 0.0, tobj = 0.0;
    uint8_t drdy;
    int result = tmp006_init(&sensor, I2C_0, 0x40, TMP006_CONFIG_CR_AS2);

    printf("Init result: %d\n", result);

    /*if (tmp006_test(&sensor) == 0) {
        puts("YEAH");
    } else {
        puts("NOPE");
        return 1;
    }*/

    if (tmp006_set_active(&sensor)) {
        puts("Measurement start failed.");
        return 1;
    }

    while (1) {
        if (tmp006_read(&sensor, &rawvolt, &rawtemp, &drdy) == -1) {
            puts("Failed to read temperature");
            //return 1;
        }

        if (drdy) {
            printf("Raw data T: %5d   V: %5d\n", rawtemp, rawvolt);
            tmp006_convert(rawvolt, rawtemp,  &tamb, &tobj);
            printf("Data Tabm: %d   Tobj: %d\n", (int)(tamb * 100), (int)(tobj * 100));
        } else {
            printf("conversion in progress\n");
        }

        xtimer_usleep(TMP006_CONVERSION_TIME);
    }
    return 0;
}

int send_temperature(int argc, char** argv)
{

    if (argc < 3) {
        printf("Usage: %s <server or multicast address> <temperature>\n", argv[0]);
        return 1;
    }

    ipv6_addr_t target;
    ipv6_addr_from_str(&target, argv[1]);

    coap_client_send(&target, COAP_METHOD_POST, COAP_TYPE_NONCON, "temperature", argv[2], COAP_CONTENTTYPE_TEXT_PLAIN);

    return 0;
}