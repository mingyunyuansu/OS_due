#include "dllist.h"
#include "synch.h"
#include <stdlib.h>
#include "system.h"

extern int err_type;
extern int is_lock;

DLLElement:: DLLElement(void *itemPtr, int sortKey){
	key = sortKey;
	item = itemPtr;
	next = NULL;
	prev = NULL;
}

DLList:: DLList(){
	first = last = NULL;
	lock = new Lock("dllist lock");
	listEmpty = new Condition("list empty cond");
}

DLList:: ~DLList() {
	int t;
	while(!IsEmpty()) {
		Remove(&t);
	}
	delete lock;
	delete listEmpty;
}

void
DLList:: Prepend(void *item) {//在表头加元素，key值是当前最小值-1
	if (!IsEmpty()){//不为空的情况
		DLLElement * element = new DLLElement(item, (first->key) - 1);
		element->next = first;
		first->prev = element;
		first = element;
	}
	else{//当前链表内为空
		DLLElement * element = new DLLElement(item, 0);//先随便设为0
		first = element;
		last = element;
	}
}

void
DLList:: Append(void *item) {
	//在末尾加
	if (IsEmpty()){//为空，随便加进去
		DLLElement * element = new DLLElement(item, 0);
		last = first = element;
	}
	else {//不为空，插入到最后
		DLLElement * element = new DLLElement(item, (last->key) + 1);
		element->prev = last;
		last->next = element;
		last = element;
	}
}

bool
DLList:: IsEmpty() {
	//判断是否空链表，依据是first是否为NULL	
	return (first == NULL);
}

void*
DLList:: Remove(int *keyPtr) {
	if (is_lock)
	lock->Acquire();//获得锁
	if (IsEmpty())
		return NULL;

	void *first_item;
	if (is_lock)
	while (IsEmpty())
		listEmpty->Wait(lock);//等待条件变量

	DLLElement * element = first;//先把当前first保存

	*keyPtr = first->key;//当前first的key值

	if (err_type == 1)//会造成多个thread对同一个first element做删除
		currentThread->Yield();

	first_item = first->item;//要返回的当前first的item

	if (first == last){//链表已被清空
		first = last = NULL;
	}
	else {//普通的删除，这时如果退出让其他线程进行操作，同样有可能导致删除操作发生错位，像刻舟求剑一样，最后破坏链表结构
		if (err_type == 5)
			currentThread->Yield();
		first->next->prev = NULL;
		first = first->next;
	}
	delete element;
	if (is_lock)
	lock->Release();
	return first_item;
}

void
DLList:: SortedInsert(void *item, int sortKey) {
	if (is_lock)
	lock->Acquire();//获得锁
	DLLElement *element = new DLLElement(item, sortKey);
	

	if (IsEmpty()) {

		if (err_type == 2)//会造成多个thread以为自己在对空链表做插入
			currentThread->Yield();

		first = last = element;
	}
	else if (element->key <= first->key){//插入的元素的key比当前first的还小
		element->next = first;
		if (err_type == 3)//可能导致多个线程试图把多个当前“最小”数放到first前，导致两个元素同时指向一个元素，他们好像“并列”一样，但是其中一个元素将被埋没
			currentThread->Yield();
		first->prev = element;
		//err_type == 3 其实也可以放在这里，是一样的效果，本质是first指针的定位不准确
		first = element;
	}
	
	else {//否则设个指针p，不停地将element的key与指针后一个项的key比，有空隙则插入
		DLLElement *p = first;
		while (p->next != NULL) {
			if (p->next->key >= element->key) {
				if (err_type == 4)//显然，在将要往链表的中间插入时，这时退出的话，就很有可能被其他线程的插入操作破坏表的顺序，根据其他线程的操作不同，表的结构的破坏程度也会不同
					currentThread->Yield();
				element->next = p->next;
				element->prev = p;
				p->next = element;
				element->next->prev = element;
				if(is_lock){ 
					listEmpty->Signal(lock);
					lock->Release();
				}
				return;
			}
			p = p->next;
		}
		//如果能运行到这里，说明新的element键值最大，应插入到最后
		p->next = element;
		element->prev = p;
		last = element;
	}
	if (is_lock){
		listEmpty->Signal(lock);
		lock->Release();
	}
}

void *DLList:: SortedRemove(int sortKey){
	if (IsEmpty())
		return NULL;
	DLLElement * p = first;
	while (p) {
		if (p->key == sortKey) {
			void *eq_item = p->item;
			if (first == last) {
				first = last = NULL;
				delete p;
				return eq_item;
			}
			else if (p == first) {
				p->next->prev = NULL;
				first = first->next;
				delete p;
				return eq_item;
			}
			else if (p == last) {
				last = last->prev;
				last->next = NULL;
				delete p;
				return eq_item;
			}
			else {//待删除的项即非头也非尾
				p->prev->next = p->next;
				p->next->prev = p->prev;
				delete p;
				return eq_item;
			}
		}
		p = p->next;
	}
	//能运行到这里，说明没有这样的项
	return NULL;
}
