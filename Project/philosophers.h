#include <stdlib.h>
#include <stdbool.h>

#define philoPHILOSOPHERS_NUMBER        ( 5 )


struct Chopstick
{
    uint id;
    bool available;
};

enum State
{
    EATING = 1,
    HUNGRY,
    THINKING,
};

struct Philosopher
{
    uint id;
    enum State state;
    struct Chopstick *chopsticks;
    bool hasLeftChopstick;
    bool hasRightChopstick;
    uint ate;
};


bool checkLeftChopstick(struct Philosopher *p);
bool checkRightChopstick(struct Philosopher *p);

void getLeftChopstick(struct Philosopher *p);
void getRightChopstick(struct Philosopher *p);

void putLeftChopstick(struct Philosopher *p);
void putRightChopstick(struct Philosopher *p);

const char* stateToChar(enum State);
