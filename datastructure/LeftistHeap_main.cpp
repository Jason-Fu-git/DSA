#include "LeftistHeap.h"
#include <cstdio>

int main()
{
    LeftistHeap<int> h1;
    LeftistHeap<int> h2;
    h1.insert(10);
    h1.insert(20);
    h1.insert(2);
    h1.insert(30);
    h1.insert(40);
    h1.insert(0);
    h1.insert(7);
    printf("%d\n", h1.delMin());
    printf("%d\n", h1.delMin());
    printf("%d\n", h1.delMin());
    for (int i = 0; i < 100; i++)
    {
        h2.insert(i);
    }
    h1.attach(&h2);
    printf("%d\n", h1.getMin());
}