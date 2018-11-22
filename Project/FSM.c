#include <stdio.h>
#include <limits.h>

#include "FreeRTOS.h"

#include "philosophers.h"

#define philoWAITING_DELAY (100UL)

static void notifyLeftPhilosopher(struct Philosopher *philosopher)
{
	struct Philosopher *leftPhilosopher = NULL;

	if (philosopher->id == philoPHILOSOPHERS_NUMBER - 1) {
		leftPhilosopher = philosopher - (philoPHILOSOPHERS_NUMBER - 1);
	} else {
		leftPhilosopher = philosopher + 1;
	}

	/* return valuse is always true for eSetBits */
	xTaskNotify(leftPhilosopher->taskHandle, notifRIGHT_CHOPSTICK,
			eSetBits);
}

static void notifyRightPhilosopher(struct Philosopher *philosopher)
{
	struct Philosopher *rightPhilosopher = NULL;
	if (philosopher->id == 0) {
		rightPhilosopher = philosopher + (philoPHILOSOPHERS_NUMBER - 1);
	} else {
		rightPhilosopher = philosopher - 1;
	}

	/* return valuse is always true for eSetBits */
	xTaskNotify(rightPhilosopher->taskHandle, notifLEFT_CHOPSTICK,
			eSetBits);
}

void FSMInit(struct FSMState states[STATE_MAX],
		 struct Philosopher philosophers[philoPHILOSOPHERS_NUMBER])
{
	states[NOOP].action = FSMNoOp;
	states[THINKING].action = FSMThinking;
	states[EATING].action = FSMEating;
	states[HUNGRY].action = FSMHungry;

	/* create resources, give out chopsticks to philosophers */
	xTaskNotify(philosophers[0].taskHandle,
			notifRIGHT_CHOPSTICK | notifLEFT_CHOPSTICK, eSetBits);

	unsigned int i;
	for (i = 2; i < philoPHILOSOPHERS_NUMBER; i++) {
		xTaskNotify(philosophers[i].taskHandle, notifRIGHT_CHOPSTICK, eSetBits);
	}
}


enum State FSMNoOp(struct Philosopher *p)
{
	(void) p;
	return NOOP;
}


enum State FSMThinking(struct Philosopher *philosopher)
{
	(void) philosopher;

	logMessage("philosopher %u is thinking\n", philosopher->id);
	busy_wait();

	return HUNGRY;
}


enum State FSMHungry(struct Philosopher *philosopher)
{
	if (philosopher->hasLeftChopstick && philosopher->hasRightChopstick) {
		return EATING;
	}

	uint32_t notification;
	BaseType_t isNotified = xTaskNotifyWait(ULONG_MAX, 0x0UL, &notification,
						pdMS_TO_TICKS(philoWAITING_DELAY));
	if (isNotified == pdTRUE) {
		if (notification & notifLEFT_CHOPSTICK) {
			configASSERT(checkLeftChopstick(philosopher));

			getLeftChopstick(philosopher);
		}

		if (notification & notifRIGHT_CHOPSTICK) {
			configASSERT(checkRightChopstick(philosopher));

			getRightChopstick(philosopher);
		}
	}
	return HUNGRY;
}


enum State FSMEating(struct Philosopher *philosopher)
{
	configASSERT(philosopher->hasLeftChopstick
			 && philosopher->hasRightChopstick);

	logMessage("philosopher %u eats\n", philosopher->id);

	busy_wait();
	philosopher->ate++;

	putLeftChopstick(philosopher);
	notifyLeftPhilosopher(philosopher);

	putRightChopstick(philosopher);
	notifyRightPhilosopher(philosopher);

	configASSERT(!(philosopher->hasLeftChopstick
			   && philosopher->hasRightChopstick));

	return THINKING;
}
