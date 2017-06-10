#include "EventBarrier.h"
#include "Elevator.h"
#include "system.h"
//extern Building * building;

EventBarrier::EventBarrier() {
    lock = new Lock("EventBarrier Lock");
    con_signal = new Condition("signal condition");
    con_wait = new Condition("wait condition");
    con_complete = new Condition("complete condition");
    status = UNSIGNALED;
    waiter_num = 0;
}

EventBarrier::~EventBarrier() {
    delete lock;
    delete con_signal;
    delete con_wait;
    delete con_complete;
}

int EventBarrier::return_waiter_num() {
    return waiter_num;
}

void EventBarrier::Wait() {
    lock->Acquire();
    waiter_num++;
    if (status == SIGNALED) {
        lock->Release();
        return;
    }
    con_wait->Wait(lock);
    lock->Release();
}

void EventBarrier::Signal() {
    lock->Acquire();
    status = SIGNALED;
    con_wait->Broadcast(lock);
    while(waiter_num != 0) {
        con_signal->Wait(lock);//还有线程没有完成，挂起等待
    }/*
    while (building->returnElev()->current_floor_have()) {
        printf("Yield");
        currentThread->Yield();
    }*/
    con_complete->Broadcast(lock);//所有线程已完成，通知所有等待响应的线程
    status = UNSIGNALED;//Signal()调用者通过栅栏
    lock->Release();
}

void EventBarrier::Complete() {
    lock->Acquire();
    waiter_num--;//Wait结束
    if (waiter_num == 0) {
        con_signal->Signal(lock);//如果是最后一个到达的人，则整个事件完成，告诉Siganl()调用者
    }
    else {
    con_complete->Wait(lock);//否则自己表示已完成，但要等还没完成的人
    }
    lock->Release();
}

int EventBarrier::Waiters() {
    return waiter_num;
}

