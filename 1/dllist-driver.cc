#include <stdio.h>
#include "dllist.h"
#include <stdlib.h>


void generateN(int N, DLList * list) {
    /*for (int i = 0; i < N; ++i) {
        int num = rand();
        list->SortedInsert(&num, num);
    }*/
    int m = 3, n = 4, q = 5, cnt = 0;
    list->SortedInsert(&m, m);
    list->SortedInsert(&n, n);
    list->SortedInsert(&q, q);
    DLLElement *p = list->first;
    while (p != NULL && cnt < 10) {
        printf("key of p: %d\n", p->key);
        p = p->next;
        cnt++;
    }
}

void removeN(int N, DLList * list) {
    int t;
    for (int i = 0; i < N; ++i) {
        if (list->IsEmpty()){
            printf("!!!!");
            break;
        }
        list->Remove(&t);
        printf("key: %d\n", t);
    }
}
