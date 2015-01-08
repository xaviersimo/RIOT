/*
 * Copyright (C) 2014 Triagnosys
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
 * @brief       Latency Riot application
 *
 * @author      Xavier Perez <xaviersimo@triagnosys.com>
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
int flag;

int index=1000;
int latency[1000]={0}; /*define vector for latency*/
int count[1000]={0};



void *second_thread(void *arg)
{
    (void) arg;

    while (1) {
    	vtimer_now(&now_thread);
    	flag = 1;
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
	next.seconds=0;
	next.microseconds=0;

	/*define sleep variables*/
	vtimer_t vtimer;
	timex_t interval=timex_set(1, 0);

	/*inizialize parameters*/
	int i=0;
	int time=10;

	/*print values*/
	int n=0;

	/*Maximum and minimum values*/
	int max_c=count[0];
	int max_l=latency[0];
	int min_c=1;
	int min_l=latency[0];


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
    printf("Samples: 1000\n");
    printf("time process: %i\n", time);
    printf("********************************************** \n");
    printf("\n \n");
    vtimer_usleep(SEC);


while(1){

	if(flag && (i<time))
		{

		/*get time now and program next thread wake up*/
		vtimer_now(&now);
		vtimer_set_wakeup(&vtimer, interval, pid);
		next.microseconds = now.microseconds + interval.microseconds;
		flag=0;
		i = i +1;


		/*Capture and print out  latency values*/
			//diff.seconds = now_thread.seconds - next.seconds; //always is 0
			diff.microseconds = now_thread.microseconds - next.microseconds;
			if (diff.microseconds > 99999)
			diff.microseconds =  0x100000000 - diff.microseconds;

			count[diff.microseconds] += 1 ;
			//x+=1;
			//printf("%i\n", x);
	//		printf("next is microsec: %"PRIu32"\n", next.microseconds);
		   // printf("now_thread is microsec: %"PRIu32"\n", now_thread.microseconds);
			//printf("diff is microsec: %"PRIu32"\n", diff.microseconds);


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
			printf("00%i %"PRIu32"\n",latency[n], count[n]);
			else if(n<100 && n>=10)
			printf("0%i %"PRIu32"\n",latency[n], count[n]);
			else
			printf("%i %"PRIu32"\n",latency[n], count[n]);

			/*Get the maximum values*/
			if (max_c < count[n])
				{
				max_c = count[n];
				max_l = latency[n];

				}

			/*Get the minimum values*/
			if (min_c > count[n])
				{
				min_c = count[n];
				min_l = latency[n];

				}
			}

		printf("MIN: %i microsec in %i times  ;  MAX: %i microsec in %i times \n", min_l, min_c, max_l, max_c);
		}

	}

    return 0;
}
