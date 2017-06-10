// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.
#include "copyright.h"
#include "system.h"
#include "EventBarrier.h"
#include "Alarm.h"
#include <stdlib.h>
#include "Elevator.h"

// testnum is set in main.cc
int testnum = 1;
int T, W;
EventBarrier * eventbarrier;
AlarmClock * alarmclock;
Building * building;

int countAlarm = 0;
int number_of_floors = 20;

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
	printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

void BarrierThread(int which) {
    if (which == W) {
        printf("thread %d Signaled\n", which);
        printf("Before Signal(), %d Threads are waiting\n", eventbarrier->Waiters());
        eventbarrier->Signal();
        //eventbarrier->Complete();
    }
    else {
        printf("thread %d Wait\n", which);
        eventbarrier->Wait();
        printf("thread %d Complete\n", which);
        eventbarrier->Complete();
    }
    printf("thread %d through\n", which);
}
void ProhibitExit(int arg) {
    while (countAlarm != 0)
        currentThread->Yield();
}

void AlarmThread(int which) {
    int sleep_time = Random()%300;
    printf("thread %d sleep at %d, wake up at %d\n", which, stats->totalTicks, (stats->totalTicks) + sleep_time);
    countAlarm++;
    alarmclock->Pause(sleep_time);
    printf("thread %d awakend at %d\n", which, stats->totalTicks);
    countAlarm--;
}

void create_elev(int arg) {
    building->returnElev()->Run();    
}
void rider(int id) {
    Elevator * e;
    int srcFloor, dstFloor;
    srcFloor = (random() % (number_of_floors-1)) + 1;

    dstFloor = (random() % (number_of_floors-1)) + 1;
    printf("Rider %d from %d to %d\n", id, srcFloor, dstFloor);

    if (srcFloor == dstFloor) return;
    
    do {
        if (srcFloor < dstFloor) {
            printf("Rider %d callUp(%d)\n", id, srcFloor);
            building->CallUp(srcFloor);
            //printf("fffff\n");
            e = building->AwaitUp(srcFloor);
        }
        else {
            printf("Rider %d callDown(%d)\n", id, srcFloor);
            building->CallDown(srcFloor);
            //printf("rider %d AwaitDown\n",id);
            e = building->AwaitDown(srcFloor);
        }
    } while(!e->Enter(id));
    //printf("rider %d Entered\n", id);
    printf("Rider %d requested %d\n", id, dstFloor);
    e->RequestFloor(dstFloor);
    printf("Rider %d Exited\n", id);
    e->Exit();
}
//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}

void ThreadTest2() {
    eventbarrier = new EventBarrier();
    DEBUG('t', "Entering ThreadTest2");
    for (int i = 1; i < T; ++i) {
        Thread * t = new Thread("forked Thread");
        t->Fork(BarrierThread, i);
    }
    BarrierThread(T);
}

void ThreadTest3() {
    alarmclock = new AlarmClock();
        if (countAlarm == 0) {
            Thread * alarm_t = new Thread("alarm prohibit exit");
            alarm_t->Fork(ProhibitExit, 0);
        }
    DEBUG('t', "Entering ThreadTest3");
    for (int i = 1; i < T; ++i) {
        Thread * t = new Thread("forked Thread");
        t->Fork(AlarmThread, i);
    }
    AlarmThread(T);
}

void ThreadTest4() {
    building = new Building("newbuilding", number_of_floors, 1);
    Thread * t_elev = new Thread("elevator");
    t_elev->Fork(create_elev, 0);
    for (int i = 1; i < T; ++i) {
        Thread * t = new Thread("Forked Thread");
        t->Fork(rider, i);
    }
    rider(T);
    //    currentThread->Finish();
}
//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest()
{
    switch (testnum) {
    case 1:
	ThreadTest1();
	break;
    case 2:
    ThreadTest2();
    break;
    case 3:
    ThreadTest3();
    break;
    case 4:
    ThreadTest4();
    break;
    default:
    printf("No test specified.\n");
	break;
    }
}

