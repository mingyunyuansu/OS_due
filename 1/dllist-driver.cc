#include <stdio.h>
#include "dllist.h"
#include <stdlib.h>


void generateN(int N, DLList * list) {
    for (int i = 0; i < N; ++i) {
        int num = rand();
        list->SortedInsert(&num, num);//item就是随机生成的key
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

void removeN(int N, DLList * list) {
    int t;
    for (int i = 0; i < N; ++i) {
        if (list->IsEmpty()){
            break;
        }
        list->Remove(&t);
        printf("item is int: %d, key: %d\n",t, t);
    }
}
