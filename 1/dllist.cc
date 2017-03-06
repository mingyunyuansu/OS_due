#include "dllist.h"
const int NULL = 0;

DLLElement:: DLLElement(void *itemPtr, int sortKey){
	key = sortKey;
	item = itemPtr;
	next = NULL;
	prev = NULL;
}

DLList:: DLList(){
	first = last = NULL;
}

DLList:: ~DLList() {
	int t;
	while(!IsEmpty()) {
		Remove(&t);
	}
}

void
DLList:: Prepend(void *item) {
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
	return (first == NULL);
}

void*
DLList:: Remove(int *keyPtr) {
	if (IsEmpty())
		return NULL;

	void *first_item;
	DLLElement * element = first;//先把当前first保存

	*keyPtr = first->key;//当前first的key值
	first_item = first->item;//要返回的当前first的item

	if (first == last)
		first = last = NULL;
	else {
		first->next->prev = NULL;
		first = first->next;
	}
	delete element;
	return first_item;
}

void
DLList:: SortedInsert(void *item, int sortKey) {
	DLLElement *element = new DLLElement(item, sortKey);
	if (IsEmpty()) {
		first = last = element;
	}
	else if (element->key <= first->key){//插入的元素的key比当前first的还小
		element->next = first;
		first->prev = element;
		first = element;
	}
	else if (element->key >= last->key) {
		//插入的元素极大
		last->next = element;
		element->prev = last;
		last = element;
		return;
	}
	else {//否则设个指针p，不停地将element的key与指针后一个项的key比，有空隙则插入
		DLLElement *p = first;
		while (p->next != NULL) {
			if (p->next->key >= element->key) {
				element->next = p->next;
				element->prev = p;
				p->next = element;
				element->next->prev = element;
				return;
			}
			//如果能运行到这里，说明新的element键值最大，应插入到最后
			p->next = element;
			element->prev = p;
			last = element;
		}
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
