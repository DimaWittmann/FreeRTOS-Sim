
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
#include <stdbool.h>
#include <signal.h>

#include "FreeRTOS.h"
#include "task.h"

#include "philosophers.h"

/* Priorities at which the tasks are created. */
#define mainCHECK_TASK_PRIORITY         ( configMAX_PRIORITIES - 10 )
#define mainQUEUE_POLL_PRIORITY         ( tskIDLE_PRIORITY + 1 )
#define mainSEM_TEST_PRIORITY           ( tskIDLE_PRIORITY + 1 )
#define mainBLOCK_Q_PRIORITY            ( tskIDLE_PRIORITY + 2 )
#define mainCREATOR_TASK_PRIORITY       ( tskIDLE_PRIORITY + 3 )
#define mainINTEGER_TASK_PRIORITY       ( tskIDLE_PRIORITY )
#define mainGEN_QUEUE_TASK_PRIORITY     ( tskIDLE_PRIORITY )
#define mainFLOP_TASK_PRIORITY          ( tskIDLE_PRIORITY )
#define mainQUEUE_OVERWRITE_PRIORITY    ( tskIDLE_PRIORITY )

#define mainTIMER_TEST_PERIOD           ( 50 )
#define mainTIMER_DELAY                 ( 2500UL )
/*
 * Prototypes for the standard FreeRTOS callback/hook functions implemented
 * within this file.
 */
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationTickHook( void );

/* Task function to check demo status. */
static void simpleTask( void *pvParameters );
static void clearScreen( void *pvParameters );


static bool bExiting = false;


static void vExitSignal(int iSig);


/*-----------------------------------------------------------*/

int main ( void )
{
    TaskHandle_t xTask [philoPHILOSOPHERS_NUMBER + 1];
    uint i = 0;
    struct Philosopher philosophers [philoPHILOSOPHERS_NUMBER] = {0};
    struct Chopstick chopsticks [philoPHILOSOPHERS_NUMBER] = {
        {0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}};

    for (i = 0; i < philoPHILOSOPHERS_NUMBER; ++i) {
        philosophers[i].id = i;
        philosophers[i].state = THINKING;
        philosophers[i].chopsticks = chopsticks;
        xTaskCreate(simpleTask, "simpleTask", configMINIMAL_STACK_SIZE,
                    &philosophers[i], mainCHECK_TASK_PRIORITY + i, &xTask[i] );
    }

    xTaskCreate(clearScreen, "clearScreen", configMINIMAL_STACK_SIZE,
                NULL, mainCHECK_TASK_PRIORITY + 8, &xTask[philoPHILOSOPHERS_NUMBER]);

    signal(SIGINT, vExitSignal);

    /* Start the scheduler itself. */
    vTaskStartScheduler();

    for (i = 0; i < philoPHILOSOPHERS_NUMBER + 1; ++i) {
        vTaskDelete(xTask);
    }

    return 0;
}


/*-----------------------------------------------------------*/
static void vExitSignal(int iSig)
{
    if (iSig == SIGINT)
        bExiting = true;
}
/*-----------------------------------------------------------*/

static void clearScreen( void *pvParameters )
{
    const TickType_t xCycleFrequency = pdMS_TO_TICKS( mainTIMER_DELAY ); 
    TickType_t xNextWakeTime = xTaskGetTickCount();

    for (;;) {
        vTaskDelayUntil( &xNextWakeTime, xCycleFrequency);
        system("clear");
    }
}

static void simpleTask( void *pvParameters )
{
    TickType_t xNextWakeTime;
    const TickType_t xCycleFrequency = pdMS_TO_TICKS( mainTIMER_DELAY );
    struct Philosopher *philosopher = (struct Philosopher*) pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();


    for( ;; )
    {
        /* Place this task in the blocked state until it is time to run again. */
        vTaskDelayUntil( &xNextWakeTime, xCycleFrequency );

        /* This is the only task that uses stdout so its ok to call printf()
        directly. */
        printf( ( char * ) "philosopher %u %s - %u\n", philosopher->id, stateToChar(philosopher->state),
            ( unsigned int ) xTaskGetTickCount() );
        fflush( stdout );

        if (bExiting)
            vTaskEndScheduler();

        switch (philosopher->state) {
            case THINKING:
                philosopher->state = HUNGRY;
                break;

            case HUNGRY:
                if (checkLeftChopstick(philosopher) && checkRightChopstick(philosopher)) {
                    getRightChopstick(philosopher);
                    getLeftChopstick(philosopher);
                }
                if (philosopher->hasLeftChopstick && philosopher->hasRightChopstick) {
                    philosopher->state = EATING;
                }
                break;

            case EATING:
                configASSERT(philosopher->hasLeftChopstick && philosopher->hasRightChopstick);

                putLeftChopstick(philosopher);
                putRightChopstick(philosopher);

                configASSERT(!(philosopher->hasLeftChopstick && philosopher->hasRightChopstick));

                philosopher->state = THINKING;
                break;

            default:
                configASSERT(0);
                break;
        }
    }
}

void vApplicationTickHook( void )
{
    /* This function will be called by each tick interrupt if
    configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
    added here, but the tick hook is called from an interrupt context, so
    code must not attempt to block, and only the interrupt safe FreeRTOS API
    functions can be used (those that end in FromISR()). */


}
/*-----------------------------------------------------------*/

void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
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
