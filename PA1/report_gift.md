# CST 1-1 Gift Report

付甲申 2022012206



## 解题思路

### 1.题目分析
题目可以转化为一个背包问题，即从n个二元组中各挑出一个物体放入容量为P的背包，求方案数。

### 2.解题思路

#### 2.1 算法选择
分析数据规模发现，n取值范围不大，但P取值范围极大。

若暴力枚举，时间复杂度为$O(2^n)$，题目中n最大取到40，显然不可行。

若采用传统动态规划算法，时间复杂度为$O(nP)$，题目中P最大取到$10^9$，也不可行。

但如果将n个二元组分成两堆，分别枚举两堆的状态，特别地，枚举第二堆时可以使用二分查找剪枝，经计算，时间复杂度为 $O(2n·2^{n/2})$，满足限时要求。

故本题采用分治+二分查找的算法。

#### 2.2 解题步骤

- **输入**。在输入过程中，顺便计算每个二元对中最小数的加和`sum_min`，以及两个数差的绝对值，存储到`p`数组中。

- **`P=P-sum_min`**，问题转化为从`p`中选取元素，使和不超过`P`的**01背包问题**。

- **`std::sort(p, p + n, isLarger)`**。`p`降序排列，便于剪枝。

- **枚举前半堆**。把枚举代价存到`p1`数组里，同时剪枝去除溢出的情况。

- **`std::sort(p1, p1 + p1_head)`**。`p1`升序排列，便于后续二分查找。

- **枚举后半堆**。对于代价`temp`，二分查找`p1`中小于等于`P-temp`的数，将数据累加到`method`方案数中。

- **输出`method`**。

#### 2.3 时间复杂度

输入: $O(n)$

礼物花费排序: $O(nlogn)$

前半堆枚举: $O(\frac{n}{2}2^{n/2})$

前半堆排序: $O(\frac{n}{2}2^{n/2})$

后半堆枚举+二分查找: $O(n2^{n/2})$

总时间复杂度为：$O(2n·2^{n/2})$

据理论计算，n=40时，可以满足时间要求。

#### 2.4 空间复杂度

空间复杂度为$O(2^{n/2})$，据计算，n=40时，占用空间为4MB。



## 源代码

```c++
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
```

