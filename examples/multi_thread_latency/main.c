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
char stack[15][KERNEL_CONF_STACKSIZE_MAIN];
//char  stack_1[KERNEL_CONF_STACKSIZE_MAIN];
char latency_vector_stack[KERNEL_CONF_STACKSIZE_MAIN];


/*Global variables*/
int index=100;
int latency[1000]={0}; /*define vector for latency*/
int count[1000]={0};

/*Define multiple threads*/
int threads=5;
int th=1;


//for(th=1 ; th < threads +1; th++)
  //	{
    //char sprintf(buffer, "t%d_stack", th)[KERNEL_CONF_STACKSIZE_MAIN];
  //	}


timex_t now_thread;
int flag[5]={1};



void *second_thread(void *arg)
{
    (void) arg;

    while (1) {
    	vtimer_now(&now_thread);
    	flag[1] = 1;
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

	flag[1]=1;
    /*define time variables*/
	timex_t now;
	timex_t next;
	timex_t next_old;
	timex_t diff;
	now_thread.seconds=0;
	now_thread.microseconds=0;
	next.seconds=0;
	next.microseconds=0;
	next_old.seconds=0;
	next_old.microseconds=0;


	/*define sleep variables*/
	vtimer_t vtimer;
	//timex_t interval=timex_set(1, 0);
	timex_t interval;
	interval.seconds=1;
	interval.microseconds=0;

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




	/*define multi sleeping thread*/
	kernel_pid_t pid[threads];
	const char buffer[9];

for(th=1 ; th < threads +1; th++)
	  	{
	    sprintf(buffer, "thread_%d", th);
	    printf("buffer is:%s\n", buffer);
		printf("threads:%i\n", th);
      pid[th] = thread_create(stack[th],
    		                    KERNEL_CONF_STACKSIZE_MAIN,
                                PRIORITY_MAIN - th,
                                CREATE_WOUT_YIELD | CREATE_STACKTEST | CREATE_SLEEPING,
                                second_thread,
                                NULL,
                               buffer);
        printf("pid thread is:%i\n", pid[th]);
	  	}

    printf("threads init\n");

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
    printf("Interval sleep: %i sec\n", interval.seconds);
    printf("Samples:%i\n", index);
    printf("time process: %i\n", time);
    printf("********************************************** \n");
    printf("\n \n");
    thread_print_all();
    vtimer_usleep(SEC);


while(1){

	if(flag[1] && (i<time))
		{

		/*get time now and program next thread wake up*/
		vtimer_now(&now);
		vtimer_set_wakeup(&vtimer, interval, pid[1]);
		next_old.microseconds = next.microseconds;
		next.microseconds = now.microseconds + interval.microseconds;
		flag[1]=0;
		i = i +1;

		/*Capture and print out  latency values*/
			//diff.seconds = now_thread.seconds - next.seconds; //always is 0
			diff.microseconds = now_thread.microseconds - next_old.microseconds;
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
	    //thread_print_all();
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
