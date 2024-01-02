#include <cstdio>

int calcCount(int n)
{
    if (n == 4)
        return 3;
    else if (n == 1)
        return 0;
    else if (n <= 3)
        return 1;
    return 3 * calcCount(n / 4) + calcCount(n - n / 4 - n / 4 - n / 4) + n + 1;
}

int main()
{
    printf("%d\n", calcCount(100000));
}
