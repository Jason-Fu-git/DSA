#include "bitmap.h"
#include <iostream>
#include <cmath>

// 从 2~n 中寻找素数
void findPrime(int n)
{
    bitmap b(n + 1);
    int m = std::sqrt(n);
    for (int i = 2; i <= m; i++)
    {
        for (int j = i * i; j <= n; j += i)
        {
            b.set(j);
        }
    }
    std::cout << b.to_string(n + 1) << std::endl;
}

int main()
{
    findPrime(100);
}