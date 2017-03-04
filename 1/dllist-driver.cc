void generateN(int N, DLList * list) {
    for (int i = 0; i < N; ++i) {
        int num = rand();
        list.SortedInsert(&num, num);
    }
}

void removeN(int N, DLList * list) {
    
}