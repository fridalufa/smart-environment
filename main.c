#include <stdio.h>

int main(void)
{
	printf("Hello Smart Environment!\n");

	// Taken from the hello world example!
	printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

	return 0;
}