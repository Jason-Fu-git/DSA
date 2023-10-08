// 《九章算术》最大公约数计算 O(log(a+b))
// 该算法较欧几里得算法更efficient

#include <cstdio>
#include <cmath>

int gcdCN(int a, int b)
{
    int p = 1;
    while (!((a & 1) || (b & 1)))
    {
        a >>= 1;
        b >>= 1;
        p <<= 1;
    }

    int t = abs(a - b);
    while (t > 0)
    {
        while (!t & 1)
        {
            t >>= 1;
        }
        (a >= b) ? a = t : b = t;
        t = abs(a - b);
    }

    return a * p;
}


int main(){
    int a,b;
    scanf("%d %d",&a,&b);
    printf("%d\n",gcdCN(a,b));
    return 0;
}