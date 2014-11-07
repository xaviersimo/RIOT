/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Hello World application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * @}
 */

//#include <stdio.h>
#include "board.h"
#include "vtimer.h"
#include "uart.h"

#define MSEC (1000)
#define SEC (1000 * MSEC)

int main(void)
{
	while(1)
	{

    LED_GREEN_ON;
    vtimer_usleep(SEC);
    LED_GREEN_OFF;
    vtimer_usleep(SEC);


    //printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    //printf("This board features a(n) %s MCU.\n", RIOT_MCU);
    //puts("hello worl!\n");
    //printf("Hello World!");

	uart_write(STDIO, 'h');
	uart_write(STDIO, 'h');
	uart_write(STDIO, 'h');
	uart_write(STDIO, 'h');
	uart_write(STDIO, 'h');
	uart_write(STDIO, '\n');
	}
    return 0;
}
