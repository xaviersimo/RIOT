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

#define MULTI_THREAD	(0)
#define THREADS			(5)
#define TH_TEST			(2)

#if MULTI_THREAD
char stack[THREADS][KERNEL_CONF_STACKSIZE_MAIN];
#endif

#define MSEC (1000)
#define SEC (1000 * MSEC)
#define MAX_LATENCY 1000

int latency[MAX_LATENCY] = {0}; /* define vector for latency */
int count[MAX_LATENCY] = {0};

/*Global variables*/


#if MULTI_THREAD
void *second_thread(void *arg)
{
(void) arg;

while (1) {
	if(TH_TEST > THREADS)
		printf("error: thread test latency is bigger of number of threads");

	//define thread variables
	int pid;
	//printf("I am in thread");

	pid = thread_getpid(); //get pid for the running thread
//	if (pid == TH_TEST){
		//calculate the test latency for the specific thread
	//}



}

	return NULL;
}
#endif

int main(void)
{


	/*vector variable*/
	int overflow = 0;
	int j = 0;
	int c = 0;

	/*define time variables*/
	timex_t now;
	timex_t next = timex_set(0, 0);
	timex_t diff;


	/*define sleep variables*/
	timex_t interval = timex_set(0, 1000); // set sleep interval to 1000 us = 1 ms

	/*inizialize parameters*/
	int iteration = 0;
	int test_repeats = 1000;


	/*print values*/
	int n = 0;

	/*Maximum and minimum values*/
	int max_c = count[0];
	int max_l = latency[0];
	int max_time_c = count[0];
	int max_time_l = latency[0];
	int min_time_c = 0;
	int min_time_l = 0;
	int min_time_flag = 1;


#if MULTI_THREAD
	/*Define multiple threads*/
	int th=1;
	/*define multi sleeping thread*/
	kernel_pid_t pid[THREADS];
	const char buffer[THREADS][10];

for(th=1 ; th < THREADS +1; th++)
	{
	sprintf(buffer[th], "th_back_%d", th);
	//printf("buffer is:%s\n", buffer[th]);
	pid[th] = thread_create(stack[th],
			KERNEL_CONF_STACKSIZE_MAIN,
			PRIORITY_MAIN - th,
			CREATE_WOUT_YIELD | CREATE_STACKTEST | CREATE_SLEEPING,
			second_thread,
			NULL,
			buffer[th]);
	}

#endif


	/*Init program*/
	printf("# ********************************************* \n");
	printf("# ************* Latency RIOT test ************* \n");
	printf("# ********************************************* \n");
	printf("# config parameters:\n");
	printf("# Interval sleep: %"PRIu32" sec and %"PRIu32" micro\n", interval.seconds, interval.microseconds);
	printf("# Samples: %i\n", MAX_LATENCY);
	printf("# Repetitions: %i\n", test_repeats);
	printf("# ********************************************* \n");

	/*Init latency vector*/

	for(j = 0 ; j < MAX_LATENCY; j++) {
		latency[j] = c;
		c += 1;
	}

	printf("# vector init\n");

	vtimer_usleep(SEC);


	vtimer_now(&now);
	next = timex_add(now, interval);
	while(1) {
		if(iteration < test_repeats) {
			vtimer_usleep(interval.microseconds); // sleep
			//vtimer_set_msg(interval.microseconds);
			//for i = 1:100 i++
			//message_receive(...);
			vtimer_now(&now); // get actual time after sleep (:=now)
			diff = timex_sub(now, next); // compute difference between theoretical time after sleep (:=next)
                                         // and actual time after sleep (:=now)

			iteration++;

			if (diff.microseconds > MAX_LATENCY - 1) // guard for overflow
				overflow++;
			else
				count[diff.microseconds] += 1; // store diff result for statistics

			vtimer_now(&now); // get actual time for next iteration (:=now)
			next = timex_add(interval, now); // update theoretical time for next iteration

			//printf("the now.microseconds is: %"PRIu32"\n", now.microseconds);
			//printf("the next.microseconds is: %"PRIu32"\n", next.microseconds);
			//printf("the diff.microseconds is: %"PRIu32"\n", diff.microseconds);
			//printf("the i is: %i \n",i);
		}

		if(test_repeats == iteration) {
			test_repeats = 0;
			printf("# Test finish\n");
			printf("# print histogram\n");
			vtimer_usleep(SEC);

			/*print out values*/
			for(n = 0; n < MAX_LATENCY ; n++) {
				if(n < 10)
					printf("00%i %i\n", latency[n], count[n]);
				else if(n<100 && n>=10)
					printf("0%i %i\n", latency[n], count[n]);
				else
					printf("%i %i\n", latency[n], count[n]);

				/*Get the maximum repetitions*/
				if (max_c < count[n]) {
					max_c = count[n];
					max_l = latency[n];
				}

				/*Get time values*/
				if (count[n] >= 1){
					/*Get maximum time value*/
					if(max_time_l < latency[n]) {
						max_time_l = latency[n];
						max_time_c = count[n];
					}
					/*Get minimum time value*/
					if(min_time_flag){
						min_time_flag = 0;
						min_time_l = latency[n];
						min_time_c = count[n];
					}
				}
			}
			printf("# MIN time is: %i microseconds in %i repetitions\n", min_time_l, min_time_c);
			printf("# MAX time is: %i microseconds in %i repetitions\n", max_time_l, max_time_c);
			printf("# overflow is: %i\n", overflow);
			printf("# MAX repetitions are: %i repetitions in %i microseconds\n", max_c, max_l);

#if MULTI_THREAD
			printf("\n\n");
			thread_print_all();
#endif
			LED_GREEN_OFF; //indicate test finish
		}
	}

	return 0;
}
