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
#include "dllist.h"
#include "Table.h"
#include "BoundedBuffer.h"

// testnum is set in main.cc
int testnum = 1;
DLList *list;
extern int T, N;
extern int table_size;
extern int buff_size;

//---------------------------------------------------------------------
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

void 
DLListThread(int which) {
	generateN(N, list, which);
	removeN(N, list, which);
}

Table * table = new Table(table_size);
BoundedBuffer* boundedbuffer = new BoundedBuffer(buff_size);

void
TableThread(int which) {
	int cnt = 0;
	int * num = new int[N];
	while (cnt < N){
	int index = rand()%table_size;
	while (index == 0)
		index = rand()%table_size;

	num[cnt] = table->Alloc(&index);
	printf("thread %d alloced %d into table[%d]\n", which, index, num[cnt]);
	currentThread->Yield();
	cnt++;
	}
	while(cnt > 0) {
	int* obj = (int *)(table->Get(num[cnt - 1]));
	table->Release(num[cnt - 1]);
	printf("thread %d released %d from table[%d]\n", which, *obj, num[cnt - 1]);
	currentThread->Yield();
	cnt--;
	}
}

void BufferThread(int which) {
	int cnt;
	char w[2] = {'a', '\0'};
	char r[3] = {0};
	for (cnt = 0; cnt < buff_size; ++cnt){
		boundedbuffer->Write(w, 1);
		printf("thread %d wrote %s to buffer\n", which, w);
		w[0]++;
		printf("hav = %d\n", boundedbuffer->hav);
		//currentThread->Yield();
	} if (boundedbuffer->hav == buff_size) printf("fulled!\n");
	for (; cnt > 0; cnt -= 2){
		boundedbuffer->Read(r, 2);
		printf("thread %d read %s from buffer\n", which, r);
		printf("hav = %d\n", boundedbuffer->hav);
		//currentThread->Yield();
	}
	if (boundedbuffer->hav == 1){
		boundedbuffer->Read(r, 1);
		printf("thread %d read %s from buffer\n", which, r);
		printf("hav = %d\n", boundedbuffer->hav);
	}
	/*for (;cnt > 0; cnt--){
		boundedbuffer->Read(r, 1);
		printf("thread %d read %s from buffer\n", which, r);
		printf("hav = %d\n", boundedbuffer->hav);
	}*/
	if (boundedbuffer->hav == 0) printf("emptyed!\n");
}
void bThread(int x) {
	char buf[2] = "A";
	for (int i = 0; i < 20; i++){
		printf("write: %s\n", buf);
		boundedbuffer->Write(buf, 1);
		++buf[0];
	}
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

void 
ThreadTest2() {
	list = new DLList();
	for (int i = 1; i < T; ++i) {
		Thread *t = new Thread("forked thread");
		t->Fork(DLListThread, i);
	}
	DLListThread(T);
}

void ThreadTest3() {
	for (int i = 1; i < T; ++i) {
		Thread * t = new Thread("forked thread");
		t->Fork(TableThread, i);
	}
	TableThread(T);
}

void ThreadTest4() {
	for (int i = 1; i < T; ++i) {
		Thread * t = new Thread("forked thread");
		t->Fork(BufferThread, i);
	}
	BufferThread(T);
}
/*
void ThreadTest4() {
	Thread * t = new Thread("forked thread");
	t->Fork(bThread, 1);

	for (int i = 0; i < 10; i++){
		char buf[3] = {0};
		boundedbuffer->Read(buf, 2);
		printf("read: %s\n", buf);
	}
}
*/
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
	ThreadTest3();//Table
	break;
	case 4:
	ThreadTest4();//BoundedBuffer
	break;
    default:
	printf("No test specified.\n");
	break;
    }
}

