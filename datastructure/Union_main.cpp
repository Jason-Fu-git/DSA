#include "Union.h"
#include <cstdio>

int main()
{
    UnionFindSet u(100);
    for (int i = 0; i < 10; i++)
    {
        for (int j = i * 10; j < i * 10 + 10; j++)
        {
            u.Union(i * 10, j);
            printf("%d, %d, size : %d\n", j, u.Find(j), u.size[u.Find(j)]);
        }
    }
    printf("--------------------\n");
    for (int i = 0; i < 100; i++)
    {
        u.Union(0, i);
        printf("%d, %d, size : %d\n", i, u.Find(i), u.size[u.Find(i)]);
    }
}