// 计算 a^n ， 时间复杂度 O(logn)
#include <cstdio>

int power(int a, int n)
{
    int result = 1;
    int base = a;
    while (n > 0)
    {
        if (n & 1)
        {
            result *= base;
        }
        n >>= 1;
        base *= base;
    }
    return result;
}

int main()
{
    int a, n;
    scanf("%d %d", &a, &n);
    printf("%d\n", power(a, n));
}