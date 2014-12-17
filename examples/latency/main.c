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
#include "gpio.h"
#include "thread.h"

#define MSEC (1000)
#define SEC (1000 * MSEC)
char second_thread_stack[KERNEL_CONF_STACKSIZE_MAIN];


/*Global variables*/
timex_t now_thread;

void *second_thread(void *arg)
{
    (void) arg;

    while (1) {
		//thread_print_all();
    	vtimer_now(&now_thread);
		thread_sleep();
    }

    return NULL;
}

int main(void)
{


	/*define vector for latency*/
    int latency[100]={0};
    int count[100]={0};

    /*define time variables*/
	timex_t now;
	timex_t next;
	timex_t diff;
	now_thread.seconds=0;
	now_thread.microseconds=0;

	/*define sleep variables*/
	vtimer_t vtimer;
	timex_t interval=timex_set(5, 1000);

	/*inizialize parameters*/
	int i=0;
	int time=1;
	//latency=100;


	/*define second thread*/
    kernel_pid_t pid = thread_create(second_thread_stack,
    		                          KERNEL_CONF_STACKSIZE_MAIN,
                                      PRIORITY_MAIN - 1,
                                      CREATE_WOUT_YIELD | CREATE_STACKTEST | CREATE_SLEEPING,
                                      second_thread,
                                      NULL,
                                      "sleep_thread");

    /*Init program*/
    printf("***************************** \n");
    printf("***** Latency RIOT test ***** \n");
    printf("***************************** \n");
    printf("config parameters:\n\n");
    printf("Interval sleep\n");
    printf("Latency resolution (in microsenconds):\n");
    printf("time process:\n");
    printf("***************************** \n");
    printf("\n \n");


while(1){


	while(i<time)
	{
	i = i +1;
    vtimer_now(&now);
    vtimer_set_wakeup(&vtimer, interval, pid);
    printf("now is second: %i\n", now.seconds);
    next.seconds = now.seconds + interval.seconds;
    next.microseconds = now.microseconds + interval.microseconds;


	}

	printf("I am in main\n");
	//thread_print_all();
	vtimer_usleep(SEC);
	if(now_thread.microseconds != 0)
		{
		diff.seconds = next.seconds - now_thread.seconds;
		diff.microseconds = next.microseconds - now_thread.microseconds;
		printf("diff is sec: %i", diff.seconds);
		printf("diff is microsec: %i", diff.microseconds);

	 	}

}


//	while(i<100)
	//	{
	//	i=i+1;
	//	latency[i]=i;
	//	printf("latency: %i  ", latency[i]);
	//	printf("count is:%i\n", count[i]);
		//vtimer_usleep(SEC);
	//	}

	//	printf("finish\n");
	//	vtimer_usleep(SEC);




    //vtimer_now(&now);

 // printf("now is second: %i\n", now.seconds);
  //printf("now is second: %i\n", now.microseconds);

    return 0;
}
