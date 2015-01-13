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

int latency[1000] = {0}; /* define vector for latency */
int count[1000] = {0};

int main(void)
{


	/*vector variable*/
	int index = 1000;
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
	int inc_min = 0;
	int min_c[100] = {0};
	int min_l[100] = {0};
	int print_min = 0;;




	/*Init program*/
	printf("********************************************* \n");
	printf("************* Latency RIOT test ************* \n");
	printf("********************************************* \n");
	printf("config parameters:\n\n");
	printf("Interval sleep: %"PRIu32" sec and %"PRIu32" micro\n", interval.seconds, interval.microseconds);
	printf("Samples: %i\n", index);
	printf("time process: %i\n", test_repeats);
	printf("********************************************* \n");
	printf("\n \n");
	vtimer_usleep(SEC);

	/*Init latency vector*/

	for(j = 0 ; j < index; j++) {
		latency[j] = c;
		c += 1;
	}

	printf("vector init\n");


	vtimer_now(&now);
	next = timex_add(now, interval);
	while(1) {
		if(iteration < test_repeats) {
			vtimer_usleep(interval.microseconds); // sleep
			vtimer_now(&now); // get actual time after sleep (:=now)
			diff = timex_sub(now, next); // compute difference between theoretical time after sleep (:=next)
                                         // and actual time after sleep (:=now)
			next = timex_add(interval, now); // update theoretical time for next iteration

			iteration++;

			if (diff.microseconds > 99999) // guard for overflow
				diff.microseconds = 0x100000000 - diff.microseconds;
			count[diff.microseconds] += 1; // store diff result for statistics

			//printf("the now.microseconds is: %"PRIu32"\n", now.microseconds);
			//printf("the next.microseconds is: %"PRIu32"\n", next.microseconds);
			//printf("the diff.microseconds is: %"PRIu32"\n", diff.microseconds);
			//printf("the i is: %i \n",i);
		}

		if(test_repeats == iteration) {
			test_repeats = 0;
			printf("Test finish\n");
			printf("print histogram\n");
			vtimer_usleep(SEC);

			/*print out values*/
			for(n = 0; n < index ; n++) {
				if(n < 10)
					printf("00%i %i\n", latency[n], count[n]);
				else if(n<100 && n>=10)
					printf("0%i %i\n", latency[n], count[n]);
				else
					printf("%i %i\n", latency[n], count[n]);

				/*Get the maximum values*/
				if (max_c < count[n]) {
					max_c = count[n];
					max_l = latency[n];
				}

				/*Get the minimum values*/
				if ( count[n] == 1) {
					inc_min ++;
					min_c[inc_min] = count[n];
					min_l[inc_min] = latency[n];
				}
			}
			printf("MAX: %i microsec in %i repetitions \n\n", max_l, max_c);
			for(print_min = 1; print_min <= inc_min; print_min++){
				printf("MIN: %i microsec in %i repetitions\n", min_l[print_min], min_c[print_min]);

			}
		}
	}

	return 0;
}
