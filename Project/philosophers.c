#include <stdio.h>

#include "FreeRTOS.h"

#include "philosophers.h"

static uint getRightChopstickId(struct Philosopher *p)
{
    // simplify
    const uint id = ( ((int)p->id - 1) % philoPHILOSOPHERS_NUMBER + philoPHILOSOPHERS_NUMBER )
                    % philoPHILOSOPHERS_NUMBER;

    return id;
}


bool checkLeftChopstick(struct Philosopher *p)
{
    return p->chopsticks[p->id].available;
}

bool checkRightChopstick(struct Philosopher *p)
{
    return p->chopsticks[getRightChopstickId(p)].available;
}

void getLeftChopstick(struct Philosopher *p)
{
    p->chopsticks[p->id].available = 0;
    p->hasLeftChopstick = true;

    printf("philosopher %u got %u chopstick \n", p->id, p->id);
}

void getRightChopstick(struct Philosopher *p)
{
    const uint chopstickId = getRightChopstickId(p);

    p->chopsticks[chopstickId].available = 0;
    p->hasRightChopstick = true;

    printf("philosopher %u got %u chopstick \n", p->id, chopstickId);
}

void putLeftChopstick(struct Philosopher *p)
{
    p->chopsticks[p->id].available = 1;
    p->hasLeftChopstick = false;

    printf("philosopher %u put %u chopstick \n", p->id, p->id);
}


void putRightChopstick(struct Philosopher *p)
{
    const uint chopstickId = getRightChopstickId(p);
    p->chopsticks[chopstickId].available = 1;
    p->hasRightChopstick = false;

    printf("philosopher %u put %u chopstick \n", p->id, chopstickId);
}


const char* stateToChar(enum State state)
{
    const char* str = "";

    switch (state) {
    case EATING:
        str = "EATING";
        break;
    case THINKING:
        str = "THINKING";
        break;
    case HUNGRY:
        str = "HUNGRY";
        break;
    default:
        configASSERT(0);
    }

    return str;
}
