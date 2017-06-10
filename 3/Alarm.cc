#include "Alarm.h"
#include "../machine/interrupt.h"
#include "system.h"

extern AlarmClock * alarmclock;
extern Interrupt * interrupt;

static void Expire(int arg) {
    alarmclock->Awaken();
}
void AlarmClock::Pause(int howLong) {
    lock->Acquire();
    interrupt->Schedule(Expire, 0, howLong, TimerInt);
    con->Wait(lock, stats->totalTicks + howLong);
    lock->Release();
}

void AlarmClock::Awaken() {
    lock->Acquire();
    con->Signal(lock);
    lock->Release();
}

AlarmClock::AlarmClock() {
    lock = new Lock("AlarmCLock lock");
    con = new ConditionPriority("AlarmClock condition");
}

AlarmClock::~AlarmClock() {
    delete lock;
    delete con;
}
