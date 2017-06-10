#include "Elevator.h"
#include "EventBarrier.h"
#include <memory.h>
#include <unistd.h>
#include "system.h"

#define UP 1
#define DOWN -1
#define BOTH 0

EventBarrier ** eventbarrierInUp;
EventBarrier ** eventbarrierInDown;
EventBarrier ** eventbarrierOutUp;
EventBarrier ** eventbarrierOutDown;

extern int number_of_floors;
extern int elevator_size;

//int * countInPeople = new int[number_of_floors + 1];
//int * countOutPeople = new int[number_of_floors + 1];

//int * HavePeople = new int[number_of_floors + 1];

int * callDir = new int[number_of_floors + 1];

Elevator::Elevator(char * debugName, int numFloors, int myID){
    name = debugName;
    currentfloor = 1;
    occupancy = 0;
    dir = UP;
    eventbarrierInUp = new EventBarrier * [numFloors + 1];
    eventbarrierOutUp = new EventBarrier * [numFloors + 1];
    eventbarrierInDown = new EventBarrier * [numFloors + 1];
    eventbarrierOutDown = new EventBarrier * [numFloors + 1];

    for (int i = 1; i <= numFloors; ++i) {
        eventbarrierInUp[i] = new EventBarrier();
        eventbarrierInDown[i] = new EventBarrier();
        eventbarrierOutUp[i] = new EventBarrier();
        eventbarrierOutDown[i] = new EventBarrier();
    }
    //memset(countInPeople, 0, sizeof(countInPeople));
    //memset(countOutPeople, 0, sizeof(countOutPeople));
  //  memset(HavePeople, 0, sizeof(HavePeople));

    memset(callDir, 0, sizeof(callDir));
    lock = new Lock("elevator lock");
}

Elevator::~Elevator() {delete eventbarrierInUp; delete lock; delete eventbarrierInDown; delete eventbarrierOutDown; delete eventbarrierOutUp;}


//control interface
void Elevator::OpenDoors() {
    //lock->Acquire();
    //printf("hhhhh\n");
    if (dir == UP) {
        eventbarrierOutUp[currentfloor]->Signal();
        eventbarrierInUp[currentfloor]->Signal();
    }
    else {
        eventbarrierOutDown[currentfloor]->Signal();
        eventbarrierInDown[currentfloor]->Signal();
    }
    //lock->Release();
}

void Elevator::CloseDoors() {
    //printf("HavePeople[currentfloor] == %d currentfloor == %d\n", current_floor_have(), currentfloor);
    //while(HavePeople[currentfloor] != 0){
      //  printf("HavePeople[currentfloor] == %d\n", HavePeople[currentfloor]);
       // OpenDoors();
    //}
    callDir[currentfloor] = 0;
}

bool Elevator::havePeople() {
    int i;
    for (i = 1; i <= number_of_floors; ++i) {
        if ((eventbarrierInUp[i]->return_waiter_num() + eventbarrierOutUp[i]->return_waiter_num() + eventbarrierInDown[i]->return_waiter_num() + eventbarrierOutDown[i]->return_waiter_num()) != 0)
            return true;
    }
    return false;
}

void Elevator::VisitFloor(int floor) {
    /*
     * while (!havePeople()){
        continue;
    }
    if (current_floor_have() != 0 && (callDir[currentfloor] == dir || callDir[currentfloor] == BOTH)) {
        printf("Elevator open door at %d\n", currentfloor);
        OpenDoors();
        CloseDoors();
    }
    */
    currentfloor = (floor == 1 ? number_of_floors : 1);
    dir = (floor == 1 ? DOWN:UP);
    //sleep(1);
    //printf("e at %d\n", currentfloor);
    //currentfloor += dir;
    //printf("HavePeople[currentfloor] == %d, currentfloor == %d\n", current_floor_have(), currentfloor);
    while (currentfloor >= 1 && currentfloor <= number_of_floors) {
        while (!havePeople()){
            printf("--------NO RIDER NOW, ELEVATOR QUIT--------\n");
            currentThread->Finish();
        }
        //printf("HavePeople[currentfloor] == %d, currentfloor == %d\n", current_floor_have(), currentfloor);
        printf("\tElevator at floor %d, direction : %s\n", currentfloor, dir == UP?"up\0":"down\0");
        if (current_floor_have() != 0 && (callDir[currentfloor] == dir || callDir[currentfloor] == BOTH)) {
            printf("Elevator opens door at floor %d\n", currentfloor);
            OpenDoors();
            CloseDoors();
        }
        //sleep(1);
        currentfloor += dir;
    }
}

//rider interface
bool Elevator::Enter(int id) {
    //lock->Acquire();
    ASSERT(current_floor_have() != 0);
    if (dir == UP){
        eventbarrierInUp[currentfloor]->Complete();//Entered
    }
    else {
        eventbarrierInDown[currentfloor]->Complete();
    }
    occupancy++;
    printf("Rider %d entered\n", id);
    /*
    if (occupancy < elevator_size) {
        occupancy++;
        return true;
    }
    */
    //lock->Release();
    //HavePeople[currentfloor]--;
    return true;
}

void Elevator::Exit() {
    //lock->Acquire();
    ASSERT(current_floor_have() != 0)
    if (dir == UP){
        eventbarrierOutUp[currentfloor]->Complete();
    }
    else {
        eventbarrierOutDown[currentfloor]->Complete();
    }
    occupancy--;
    //HavePeople[currentfloor]--;
    //lock->Release();
}

void Elevator::RequestFloor(int floor) {
    //lock->Acquire();
    //HavePeople[floor]++;
    if (dir == UP){
        if (callDir[floor] == DOWN)
            callDir[floor] = BOTH;
        else
            callDir[floor] = dir;
        eventbarrierOutUp[floor]->Wait();
    }
    else {
        if (callDir[floor] == UP)
            callDir[floor] = BOTH;
        else
            callDir[floor] = dir;
        eventbarrierOutDown[floor]->Wait();
    }
    //lock->Release();
}

void Elevator::Run() {
    while (true) {
        //printf("Run start\n");
        VisitFloor(number_of_floors);
        //printf("Run from top to 1\n");
        VisitFloor(1);
    }
}

Building::Building(char * debugName, int numFloors, int numElevators){
    name = debugName;
    elevator = new Elevator(name, numFloors, 1);
}

Building::~Building(){}
//rider interface

void Building::CallUp(int fromFloor) {
//    lock->Acquire();
    //HavePeople[fromFloor]++;
    //printf("press up");
    if (callDir[fromFloor] == DOWN)
        callDir[fromFloor] = BOTH;
    else
        callDir[fromFloor] = UP;
//    eventbarrierInUp[fromFloor]->Wait(); 
//    lock->Release();
}

void  Building::CallDown(int fromFloor) {
//    lock->Acquire();
    //HavePeople[fromFloor]++;
    if (callDir[fromFloor] == UP)
        callDir[fromFloor] = BOTH;
    else
        callDir[fromFloor] = DOWN;
//    eventbarrierInDown[fromFloor]->Wait();
//    lock->Release();
}

Elevator *  Building::AwaitUp(int fromFloor){
     eventbarrierInUp[fromFloor]->Wait();
     return elevator;
}

Elevator *  Building::AwaitDown(int fromFloor) {
    eventbarrierInDown[fromFloor]->Wait();
    return elevator;
}

int Elevator::current_floor_have(){
    return eventbarrierInUp[currentfloor]->return_waiter_num() + eventbarrierOutUp[currentfloor]->return_waiter_num() + eventbarrierInDown[currentfloor]->return_waiter_num() + eventbarrierOutDown[currentfloor]->return_waiter_num();
}
/*
int Elevator::query_floor_have(int query){
    return eventbarrierInUp[query]->return_waiter_num() + eventbarrierOutUp[query]->retrun_waiter_num() + eventbarrierInDown[query]->return_waitr_num() + eventbarrierOutDown[query]->return_waiter_num;
}
*/
