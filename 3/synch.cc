// synch.cc 
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks 
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------
//extern bool isLockBusy;
//extern Thread * lockOwner;
//extern List * queue;

Semaphore::Semaphore(char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    
    while (value == 0) { 			// semaphore not available
	queue->Append((void *)currentThread);	// so go to sleep
	currentThread->Sleep();
    } 
    value--; 					// semaphore available, 
						// consume its value
    
    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread *)queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
	scheduler->ReadyToRun(thread);
    value++;
    (void) interrupt->SetLevel(oldLevel);
}
//
// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!
Lock::Lock(char* debugName) {
	name = debugName;
	isLockBusy = false;
	queue = new List();
}

Lock::~Lock() {delete queue;}

void Lock::Acquire() {
	IntStatus oldLevel = interrupt->SetLevel(IntOff);//关中断
	while (isLockBusy) {
		queue->Append((void*)currentThread);
		currentThread->Sleep();
	}
	isLockBusy = true;
	lockOwner = currentThread;
	(void)interrupt->SetLevel(oldLevel);//恢复中断
}

void Lock::Release() {
	Thread* thread;
	ASSERT(isHeldByCurrentThread());//必须是当前thread持有锁，才能释放锁，否则肯定有错误
	IntStatus oldLevel = interrupt->SetLevel(IntOff);//interruption off
	thread = (Thread*)queue->Remove();
	if (thread)
		scheduler->ReadyToRun(thread);
	isLockBusy = false;
	lockOwner = NULL;
	(void)interrupt->SetLevel(oldLevel);//恢复中断
}

bool Lock::isHeldByCurrentThread(){
	return (lockOwner == currentThread && isLockBusy == true);
}

Condition::Condition(char* debugName) { 
	name = debugName;
	queue = new List();
}
Condition::~Condition() {
	delete queue;
}
void Condition::Wait(Lock* conditionLock) { 
	ASSERT(conditionLock->isHeldByCurrentThread());
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	conditionLock->Release();
	queue->Append((void *)currentThread);
	currentThread->Sleep();
	conditionLock->Acquire();
	(void)interrupt->SetLevel(oldLevel);
}

void Condition::Signal(Lock* conditionLock) {
	ASSERT(conditionLock->isHeldByCurrentThread());
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	Thread * thread = (Thread*)queue->Remove();
	if (thread)
		scheduler->ReadyToRun(thread);
	(void)interrupt->SetLevel(oldLevel);
}

void Condition::Broadcast(Lock* conditionLock) {
	ASSERT(conditionLock->isHeldByCurrentThread());
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	Thread * thread = (Thread*)queue->Remove();
	while(thread) {
		scheduler->ReadyToRun(thread);
		thread = (Thread*)queue->Remove();
	}
	(void)interrupt->SetLevel(oldLevel);
}
//下面的Condition给AlarmClock用
ConditionPriority::ConditionPriority(char* debugName) { 
	name = debugName;
	queue = new List();
}
ConditionPriority::~ConditionPriority() {
	delete queue;
}
void ConditionPriority::Wait(Lock* conditionPriorityLock, int p) { 
	ASSERT(conditionPriorityLock->isHeldByCurrentThread());
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	conditionPriorityLock->Release();
	queue->SortedInsert((void *)currentThread, p);
	currentThread->Sleep();
	conditionPriorityLock->Acquire();
	(void)interrupt->SetLevel(oldLevel);
}

void ConditionPriority::Signal(Lock* conditionPriorityLock) {
	ASSERT(conditionPriorityLock->isHeldByCurrentThread());
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	Thread * thread = (Thread*)queue->Remove();
	if (thread)
		scheduler->ReadyToRun(thread);
	(void)interrupt->SetLevel(oldLevel);
}

void ConditionPriority::Broadcast(Lock* conditionPriorityLock) {
	ASSERT(conditionPriorityLock->isHeldByCurrentThread());
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	Thread * thread = (Thread*)queue->Remove();
	while(thread) {
		scheduler->ReadyToRun(thread);
		thread = (Thread*)queue->Remove();
	}
	(void)interrupt->SetLevel(oldLevel);
}
