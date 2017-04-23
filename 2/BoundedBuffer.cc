#include"BoundedBuffer.h"
#include "synch.h"
#include "system.h"

extern int is_lock;

BoundedBuffer::BoundedBuffer(int maxsize){
	buffersize = maxsize;
	buffer = new char[maxsize];
	in = out = hav = 0;
	full = new Condition("full condition");
	empty = new Condition("empty condition");
	bufflock = new Lock("buffer lock");
}

BoundedBuffer::~BoundedBuffer(){
	delete buffer;
	delete full;
	delete empty;
	delete bufflock;
}

void BoundedBuffer::Read(void * data, int size){
	if (is_lock) bufflock->Acquire();
	char *r = (char *)data;
	ASSERT(size >= 0);
	while(size--){if (is_lock)
		while (hav == 0){
			empty->Wait(bufflock);
		}
		*(r++)=buffer[out];currentThread->Yield();//mistake
		out = (out+1) % buffersize;
		hav--;
		if (is_lock) full->Broadcast(bufflock);
	}
	if (is_lock)bufflock->Release();
}

void BoundedBuffer::Write(void * data, int size){
	if (is_lock)bufflock->Acquire();
	char *w=(char *)data;
	ASSERT(size >= 0);
	while(size--){if (is_lock)
		while (hav == buffersize){
			full->Wait(bufflock);
		}
		buffer[in]=*(w++);currentThread->Yield();//mistake
		in=(in+1)%buffersize;
		hav++;
		if (is_lock)empty->Broadcast(bufflock);
	}
	if (is_lock) bufflock->Release();
}
