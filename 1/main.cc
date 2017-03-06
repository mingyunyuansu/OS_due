#include "dllist.h"
#include <stdio.h>
int main() {
    DLList * list = new DLList();
    generateN(10, list);
    removeN(10, list);
    return 0;
}
