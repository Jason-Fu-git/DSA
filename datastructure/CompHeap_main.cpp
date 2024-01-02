#include "CompHeap.h"
#include <cstdio>

int main()
{
    CompHeap<int> heap(100);
    heap.insert(5);
    heap.insert(3);
    heap.insert(7);
    heap.insert(1);
    printf("%d\n", heap.getMax());
    heap.insert(10);
    heap.insert(2);
    heap.insert(100);
    printf("%d\n", heap.delMax());
    printf("%d\n", heap.delMax());
    printf("%d\n", heap.delMax());
}