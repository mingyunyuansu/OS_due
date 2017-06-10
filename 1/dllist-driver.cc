#include <stdio.h>
#include "dllist.h"
#include <stdlib.h>
#include <ctime>
#include "system.h"

void generateN(int N, DLList * list, int which) {
	srand(time(0));
	for (int i = 0; i < N; ++i) {
        int num = rand()%2000;
        list->SortedInsert(&num, num);//简单起见，item指针就是key的指针
		printf("Thread %d insert number %d into list\n", which, num);
		currentThread->Yield();
    }
    /*测试用，无视
    int m = 3, n = 5, q = 4, cnt = 0;
    list->SortedInsert(&m, m);
    list->SortedInsert(&n, n);
    list->SortedInsert(&q, q);
    DLLElement *p = list->first;
    while (p != NULL && cnt < 10) {
        printf("key of p: %d\n", p->key);
        p = p->next;
        cnt++;
    }*/

}

void removeN(int N, DLList * list, int which) {
	int t;
    for (int i = 0; i < N; ++i) {
        if (list->IsEmpty()){
            break;
        }
        list->Remove(&t);
        printf("Thread %d removed item, key: %d\n",which, t);
		currentThread->Yield();
    }
}
