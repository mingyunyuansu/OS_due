#include "Table.h"
#include "synch.h"
#include "system.h"

extern int is_lock;

Table::Table(int size) {
	table_size = size;
	table = new void*[size+1];
	memset(table, 0, sizeof(void*) * (size+1));
	lock = new Lock("Table lock");
}

Table::~Table() {
	delete lock;
	delete table;
}

int Table::Alloc(void * object) {
	if (is_lock)
		lock->Acquire();
	int i;
	for (i = 1; i <= table_size; ++i) {
		if (table[i] == NULL) {
			currentThread->Yield();//强制退出
			table[i] = object;
			if (is_lock) lock->Release();
			return i;
		}
	}
	if (is_lock) lock->Release();//分配失败
	return -1;
}

void Table::Release(int index) {
	ASSERT(index > 0 && index <= table_size);
	if (is_lock) lock->Acquire();
	currentThread->Yield();
	table[index] = NULL;
	if (is_lock) lock->Release();
}

void * Table::Get(int index) {
	ASSERT(index > 0 && index <= table_size);
	return table[index];
}

