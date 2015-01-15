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
#include "ps.h"

#define THREADS			(15)
#define THREAD_LATENCY  (3)  //Define the pid number to measure latency.

char stack[THREADS][KERNEL_CONF_STACKSIZE_MAIN];


#define MSEC (1000)
#define SEC (1000 * MSEC)
#define MAX_LATENCY 1000

/*Define vectors to print histogram*/
int latency[MAX_LATENCY] = {0}; /* define vector for latency */
int count[MAX_LATENCY] = {0};
int overflow = 0;

int thread[THREADS] = {0}; //save the repetitions to execute each thread

/*inizialize parameters*/
int iteration = 0;
int test_repeats = 1000;

void *second_thread(void *arg)
{
(void) arg;
/*Define variables control threads*/
int pid;


/*define time variables*/
timex_t now;
timex_t next = timex_set(0, 0);
timex_t diff;

/*define sleep variables*/
timex_t interval = timex_set(0, 1000); // set sleep interval to 1000 us = 1 ms

vtimer_now(&now);
next = timex_add(now, interval);

while(1){

	pid = thread_getpid();
	thread[pid]++;
	//printf("pid thread is: %i\n", pid);


	if (pid == THREAD_LATENCY){
		//printf("thread latency is: %i\n", pid);
		if(iteration < test_repeats) {
		vtimer_usleep(interval.microseconds); // sleep
		vtimer_now(&now); // get actual time after sleep (:=now)
		diff = timex_sub(now, next); // compute difference between theoretical time after sleep (:=next)
                                     // and actual time after sleep (:=now)
		iteration++;

		if (diff.microseconds > MAX_LATENCY - 1) // guard for overflow
			overflow++;
		else
			count[diff.microseconds] += 1; // store diff result for statistics
			//printf("the latency result is: %"PRIu32" \n", diff.microseconds);

		vtimer_now(&now); // get actual time for next iteration (:=now)
		next = timex_add(interval, now); // update theoretical time for next iteration
		}
	}
	thread_yield();
	if(test_repeats == iteration) { //if the test finish send to sleep all threads
		thread_sleep();
	}
}
	return NULL;
}

int main(void)
{


	/*vector variable*/
	int j = 0;
	int c = 0;



	/*print values*/
	int n = 0;
	int d = 0;

	/*Maximum and minimum values*/
	int max_c = count[0];
	int max_l = latency[0];
	int max_time_c = count[0];
	int max_time_l = latency[0];
	int min_time_c = 0;
	int min_time_l = 0;
	int min_time_flag = 1;



	/*Init program*/
	printf("# ********************************************* \n");
	printf("# ************* Latency RIOT test ************* \n");
	printf("# ********************************************* \n");
	printf("# config parameters:\n");
	//printf("# Interval sleep: %"PRIu32" sec and %"PRIu32" micro\n", interval.seconds, interval.microseconds);
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

	/*Define multiple threads*/
	int th=1;
	/*define multi sleeping thread*/
	kernel_pid_t pid[THREADS];
	char buffer[THREADS][11];

	for(th=1 ; th < THREADS; th++)
	{
	sprintf(buffer[th], "th_back_%d", th);
//	printf("buffer is:%s\n", buffer[th]);
	pid[th] = thread_create(stack[th],
			KERNEL_CONF_STACKSIZE_MAIN,
			PRIORITY_MAIN -1,
			CREATE_WOUT_YIELD | CREATE_STACKTEST,
			second_thread,
			NULL,
			buffer[th]);
	}
	thread_yield();

	while(1) {
		if(test_repeats == iteration) {
			thread_yield(); //force to execute the rest of threads in order to send all threads in sleep before print results
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


			printf("\n\n");
			thread_print_all();

			for (d=3; d < THREADS +2; d++){
				printf("#Thread pid %d is executed: %i times\n", d, thread[d]);
			}
			LED_GREEN_OFF; //indicate test finish
		}
	thread_yield();
	}

	return 0;
}

