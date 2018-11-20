#include "FreeRTOS.h"

#include "philosophers.h"


void FSMInit(struct FSMState states[STATE_MAX],
		 struct Philosopher philosophers[philoPHILOSOPHERS_NUMBER])
{
	states[NOOP].action = FSMNoOp;
	states[THINKING].action = FSMThinking;
	states[EATING].action = FSMEating;
	states[HUNGRY].action = FSMHungry;
}


enum State FSMNoOp(struct Philosopher *p)
{
	(void) p;
	return NOOP;
}


enum State FSMThinking(struct Philosopher *p)
{
	(void) p;
	return HUNGRY;
}


enum State FSMEating(struct Philosopher *philosopher)
{
	configASSERT(philosopher->hasLeftChopstick
			 && philosopher->hasRightChopstick);

	philosopher->ate++;

	putLeftChopstick(philosopher);
	putRightChopstick(philosopher);

	configASSERT(!(philosopher->hasLeftChopstick
			   && philosopher->hasRightChopstick));

	return THINKING;
}


enum State FSMHungry(struct Philosopher *philosopher)
{
	if (!philosopher->hasLeftChopstick && checkLeftChopstick(philosopher)) {
		getLeftChopstick(philosopher);
	} else
	if (!philosopher->hasRightChopstick && checkRightChopstick(philosopher)) {
		getRightChopstick(philosopher);
	}

	if (philosopher->hasLeftChopstick && philosopher->hasRightChopstick) {
		return EATING;
	} else {
		return HUNGRY;
	}
}
