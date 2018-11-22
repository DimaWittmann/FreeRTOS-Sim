
/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
 * main() creates all the demo application tasks, then starts the scheduler.
 * The web documentation provides more details of the standard demo application
 * tasks, which provide no particular functionality but do provide a good
 * example of how to use the FreeRTOS API.
 *
 * In addition to the standard demo tasks, the following tasks and tests are
 * defined and/or created within this file:
 *
 * "Check" task - This only executes every five seconds but has a high priority
 * to ensure it gets processor time.  Its main function is to check that all the
 * standard demo tasks are still operational.  While no errors have been
 * discovered the check task will print out "OK" and the current simulated tick
 * time.  If an error is discovered in the execution of a task then the check
 * task will print out an appropriate error message.
 *
 */

/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

#include "philosophers.h"

#define mainTIMER_DELAY (100UL)

static void simpleTask(void *pvParameters);
static void clearScreen(void *pvParameters);

static struct Philosopher philosophers[philoPHILOSOPHERS_NUMBER];
static struct FSMState states[STATE_MAX];
static struct Chopstick chopsticks[philoPHILOSOPHERS_NUMBER] = {{0, 1},
								{1, 1},
								{2, 1},
								{3, 1},
								{4, 1}};

/*-----------------------------------------------------------*/

int main(void)
{
	TaskHandle_t clearTaskHandle;

	xTaskCreate(clearScreen, "clearScreen", configMINIMAL_STACK_SIZE,
		    philosophers, philoCHECK_TASK_PRIORITY + 5,
		    &clearTaskHandle);

	initPhilosophers();

	vTaskStartScheduler();

	deinitPhilosophers();
	vTaskDelete(clearTaskHandle);

	return 0;
}

static void clearScreen(void *pvParameters)
{
	const TickType_t xCycleFrequency = pdMS_TO_TICKS(mainTIMER_DELAY);
	TickType_t xNextWakeTime = xTaskGetTickCount();
	struct Philosopher *xPhilosophers = (struct Philosopher *)pvParameters;

	for (;;) {
		uint i;

		printf("\n");

		for (i = 0; i < philoPHILOSOPHERS_NUMBER; ++i) {
			printf("philosopher %u ate %u \n",
			       xPhilosophers[i].id, xPhilosophers[i].ate);
		}
		vTaskDelayUntil(&xNextWakeTime, xCycleFrequency);
	}
}

static void simpleTask(void *pvParameters)
{
	struct Philosopher *xPhilosopher = (struct Philosopher *)pvParameters;
	for (;;) {
		FSMNextStep(xPhilosopher, states);
	}
	vTaskSuspend(NULL);
}

void initPhilosophers(void)
{
	uint i = 0;

	for (i = 0; i < philoPHILOSOPHERS_NUMBER; ++i) {
		philosophers[i].id = i;
		philosophers[i].state = THINKING;
		philosophers[i].chopsticks = chopsticks;
		xTaskCreate(simpleTask, "simpleTask", configMINIMAL_STACK_SIZE,
			    &philosophers[i], philoCHECK_TASK_PRIORITY,
			    &(philosophers[i].taskHandle));
	}

	FSMInit(states, philosophers);
}

void deinitPhilosophers(void)
{
	uint i = 0;

	for (i = 0; i < philoPHILOSOPHERS_NUMBER; ++i) {
		vTaskDelete(philosophers[i].taskHandle);
	}
}

/*-----------------------------------------------------------*/

void vAssertCalled(unsigned long ulLine, const char *const pcFileName)
{
	taskENTER_CRITICAL();
	{
		printf("[ASSERT] %s:%lu\n", pcFileName, ulLine);
		fflush(stdout);
	}
	taskEXIT_CRITICAL();
	exit(-1);
}
/*-----------------------------------------------------------*/
