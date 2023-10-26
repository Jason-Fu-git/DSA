#include <cstdio>
#include <algorithm>
using namespace std;

int n; // 几岁了
int P; // 预算

// 礼物花费（每一组大减小）,
int *p;

// 前半堆
int *p1;
int p1_head = 0; // p1的头

// 方案数
long long method = 0; 

bool isLarger(int a, int b)
{
    return a > b;
}

// 返回代价小于等于threshold的方案数
int find_leq_method(int threshold)
{
    int l = 0;
    int r = p1_head - 1;
    while (l <= r)
    {
        int mid = (l + r) >> 1;
        if (p1[mid] <= threshold)
        {
            l = mid + 1;
        }
        else
        {
            r = mid - 1;
        }
    }
    return r + 1;
}

int main()
{
    // 读取，并存入
    scanf("%d %d", &n, &P);
    p = new int[n];  // 初始化
    int sum_min = 0; // 花费的最小值
    for (int i = 0; i < n; i++)
    {
        int a, b;
        scanf("%d %d", &a, &b);

        // 判断大小
        if (a > b)
        {
            p[i] = a - b; // 存入p
            a = b;        // 把a置成最小的
        }
        else
        {
            p[i] = b - a; // 存入p
        }

        // 累加，判断是否越界
        sum_min += a;
        if (sum_min < a)
        { // 超出int上界
            printf("0");
            return 0;
        }
    }

    P -= sum_min;
    if (P < 0) // 超出p
    {
        printf("0");
        return 0;
    }

    // 礼物花费排序 (降序)
    sort(p, p + n, isLarger);

    // 划分
    int first_len = n / 2;    // 前半堆
    int mid = 1 << first_len; // 前半堆二进制上界
    p1 = new int[mid];

    // 遍历前半堆
    for (int i = 0; i < mid; ++i)
    {
        int temp = 0;
        for (int j = 0; j < first_len; ++j)
        {
            if ((1 << j) & i)
            { // 选取这个位
                temp += p[j];
                if (temp < p[j])
                {
                    temp = -1;
                    break; // 越界
                }
            }
        }
        if (temp >= 0 && temp <= P) // 未越界
        {
            p1[p1_head] = temp;
            ++p1_head;
        }
    }

    // 前半堆排序(升序)
    sort(p1, p1 + p1_head);

    // 遍历后半堆
    int second_len = n - first_len; // 后半堆
    mid = 1 << second_len;          // 后半堆二进制上界
    for (int i = 0; i < mid; ++i)
    {
        int temp = 0;
        for (int j = 0; j < second_len; ++j)
        {
            if ((1 << j) & i)
            { // 选取这个位
                int pos = first_len + j;
                temp += p[pos];
                if (temp < p[pos])
                {
                    temp = -1;
                    break; // 越界
                }
            }
        }
        if (temp >= 0 && temp <= P)
        {
            // 折半查找
            method += find_leq_method(P - temp);
        }
    }

    printf("%lld", method);
    delete[] p;
    delete[] p1;
    return 0;
}