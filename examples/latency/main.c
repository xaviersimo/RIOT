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
#include "msg.h"

#define MSEC (1000)
#define SEC (1000 * MSEC)
char second_thread_stack[KERNEL_CONF_STACKSIZE_MAIN];
char latency_vector_stack[KERNEL_CONF_STACKSIZE_MAIN];


/*Global variables*/
timex_t now_thread;
int flag_th;

int index=1000;
int latency[1000]={0}; /*define vector for latency*/
int count[1000]={0};

void *second_thread(void *arg)
{
    (void) arg;

    while (1) {
    	vtimer_now(&now_thread);
    	flag_th = 1;
		thread_sleep();
    }

    return NULL;
}

void *vector_latency(void *arg)
{
  (void) arg;

  /*Init latency vector*/
  int j=0;
  int c=0;

  for(j=0 ; j < index; j++)
  	{
  	latency[j] = c;
  	c= c + 1;
  	}
  printf("vector init\n");

    return NULL;
}

int main(void)
{


    /*define time variables*/
	timex_t now;
	timex_t next;
	timex_t diff;
	now_thread.seconds=0;
	now_thread.microseconds=0;
	int flag_time;

	/*define sleep variables*/
	vtimer_t vtimer;
	timex_t interval=timex_set(2, 0);

	/*inizialize parameters*/
	int i=0;
	int time=100;

	/*print values*/
	int n=0;


	/*define sleeping thread*/
    kernel_pid_t pid = thread_create(second_thread_stack,
    		                          KERNEL_CONF_STACKSIZE_MAIN,
                                      PRIORITY_MAIN - 1,
                                      CREATE_WOUT_YIELD | CREATE_STACKTEST,
                                      second_thread,
                                      NULL,
                                      "sleep_thread");

	/*define latency vector thread*/
     thread_create(latency_vector_stack,
                   KERNEL_CONF_STACKSIZE_MAIN,
                   PRIORITY_MAIN - 2,
                   CREATE_WOUT_YIELD | CREATE_STACKTEST,
                   vector_latency,
                   NULL,
                   "vector_latency");



    /*Init program*/
    printf("********************************************* \n");
    printf("************* Latency RIOT test ************* \n");
    printf("********************************************* \n");
    printf("config parameters:\n\n");
    printf("Interval sleep: sec:%i\n", interval.seconds);
    printf("Latency resolution (in microsenconds): 1000\n");
    printf("time process: %i\n", time);
    printf("********************************************** \n");
    printf("\n \n");
    vtimer_usleep(SEC);


while(1){

	if(flag_time && (i<time))
		{

		flag_time = 0;

		/*Capture the time values*/
		vtimer_now(&now);
		next.seconds = now.seconds + interval.seconds;
		next.microseconds = now.microseconds + interval.microseconds;
		}


	if(flag_th && (i<time))
		{

		/*Program next thread wake up*/
		flag_th=0;
		flag_time=1;
		i = i +1;
	    vtimer_set_wakeup(&vtimer, interval, pid);

		/*Capture and print out  latency values*/
		if(i>1)
			{
			//diff.seconds = now_thread.seconds - next.seconds; //always is 0
			diff.microseconds = now_thread.microseconds - next.microseconds;

			count[diff.microseconds/10] += 1 ;

			//printf("diff is microsec: %i\n", diff.microseconds);
			}
		}

	if(time == i)
		{
		time=0;
		printf("Test finish\n");
		printf("print histogram\n");
		vtimer_usleep(SEC);
		/*print out values*/
		for(n=0; n < index ; n++)
			{
			if(n<10)
			printf("%i000 %i\n",latency[n], count[n]);
			else if(n<100 && n>=10)
			printf("%i00 %i\n",latency[n], count[n]);
			else
			printf("%i0 %i\n",latency[n], count[n]);
			}

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
