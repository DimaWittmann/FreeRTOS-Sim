#include <stdlib.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"

#define philoPHILOSOPHERS_NUMBER (5)
#define philoCHECK_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define notifLEFT_CHOPSTICK (0x1UL)
#define notifRIGHT_CHOPSTICK (0x2UL)


struct Chopstick {
	uint id;
	bool available;
};

enum State {
	NOOP,
	EATING = 1,
	HUNGRY,
	THINKING,
	STATE_MAX,
};

struct Philosopher {
	uint id;
	enum State state;
	struct Chopstick *chopsticks;
	bool hasLeftChopstick;
	bool hasRightChopstick;
	uint ate;
	uint cycles;
	TaskHandle_t taskHandle;
};


struct FSMState {
	enum State (*action)(struct Philosopher *p);
};


bool checkLeftChopstick(struct Philosopher *p);
bool checkRightChopstick(struct Philosopher *p);

void getLeftChopstick(struct Philosopher *p);
void getRightChopstick(struct Philosopher *p);

void putLeftChopstick(struct Philosopher *p);
void putRightChopstick(struct Philosopher *p);

const char *stateToChar(enum State);

void logMessage(const char *fmt, ...);
#if (DEBUG == 1)
# define logDebugMessage(fmt, ...) logMessage(fmt, ##__VA_ARGS__)
#else
# define logDebugMessage(fmt, ...)
#endif
void clearScreen(void);

void initPhilosophers(void);
void deinitPhilosophers(void);
void FSMInit(struct FSMState states[STATE_MAX],
	     struct Philosopher philosophers[philoPHILOSOPHERS_NUMBER]);
void FSMNextStep(struct Philosopher *p, struct FSMState states[STATE_MAX]);

enum State FSMNoOp(struct Philosopher *p);
enum State FSMThinking(struct Philosopher *p);
enum State FSMEating(struct Philosopher *p);
enum State FSMHungry(struct Philosopher *p);
