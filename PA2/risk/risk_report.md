# CST 2-1-1 Risk

付甲申 2022012206



## 1. 题目分析

题目首先要求读取连续n天的确诊病例数量，在解题过程中仅有针对确诊病例数量的访问，没有插入、删除过程，存储确诊病例数目的数据结构选用**动态数组**。

接着，题目要求针对每一天$i$，计算其前$m_i$天中最大的确诊病例数目。若使用暴力查找算法，时间复杂度为$O(n^2)$，显然超时。考虑到每次迭代只按顺序增加一个元素，且考虑到截止日期的单调非减性，可以采用**优先队列**的数据结构。迭代到第$i$天时，把$x_{i-1}$入队，维护整个优先队列的值，使每个元素的值为该元素到队尾元素中的最大值，并将日期小于截止日期的元素出队，队首即为答案。理想情况下，时间复杂度为$O(n)$，可以满足题目要求。

然后，针对计算出的最大值，需要分别查找位于区间$[0,p)$，$[p,q)$,$[q,+\infin)$的元素个数。考虑到查找次数最大为$10^5$，可以采用**排序+二分查找**的方法节约时间。



## 2. 算法优化

优先队列的使用虽然能够在一般情况下节省时间，但在最坏情况，即输入的确诊病例数量单调递增时，时间复杂度仍然为$O(n^2)$。事实上，在维护优先队列的时候，小于新插入值的元素一定不会成为最后的答案。对此，可以采用反证法：假设某一时刻优先队列为$<...,x_j,...,x_{i-1}]$且有$x_j$<$x_{i-1}$, 假设取前m天时$x_j$为答案，则必有$x_j \geq x_{i-1}$，矛盾。故针对小于新插入值的元素，完全可以**让这些元素从队尾出队**，为实现这一目标，可以采用**双向队列**的数据结构。这样可以一定程度上减少比较操作，比简单的优先队列要节省时间。

排序算法选择**归并排序**。归并排序过程需要频繁地进行`new`与`delete`操作，浪费一定的时间和内存。考虑到排序时，原先存储确诊病例的数组已经没有用了，故可以用这个数组作为缓存数组，以避免归并过程中的`new`与`delete`操作。



## 3. 解题步骤

### （1）初始化：

- 读取天数n，初始化确诊病例动态数组`cases`，存储确诊病例的双向队列`case_q`，存储`case_q`中元素对应索引的双向队列`index_q`，以及存储答案的数组`k`，规模均为`n`。

- 读取每天的确诊病例，存储到数组`cases`中。

### （2）计算最大病例数：

- 把小于`cases[i-1]`的`case_q`中元素从队尾出队，直至遇到第一个大于等于`cases[i-1]`的元素或到队首，将`cases[i-1]`从队尾入队。

- 根据输入的前溯天数`mi`，将早于前溯天数的元素从队首出队。

- 把队首元素`case_q[head]`存入`k[i]`

### （3）排序：

- 利用merge sort排序`k`数组，merge sort算法原型源自讲义。

### （4）二分查找：
- 分别查找`p`与`q`的lowerbound，即`k`中第一个大于等于`p`和`q`的元素索引，分别记为`l`和`r`

- 输出`l`（低风险）和`r-l`（中风险）

### （5）释放空间



## 4. 复杂度分析

### （1）时间复杂度

输入过程为$O(n)$，双向队列维护过程为$O(n)$，归并排序为$O(nlogn)$，二分查找过程为$O(2Tlogn)$。

总体时间复杂度为$O((n+2T)logn)$。

### （2）空间复杂度

动态数组为$O(4n)$，归并排序递归深度为$O(logn)$，总空间复杂度为$O(n)$。



## 5. 源代码

```c++
#include <cstdio>

int n;        // 天数
int T;        // 查找数
int *cases;   // 确诊人数
int *case_q;  // 病例的优先队列
int *index_q; // 病例优先队列对应的index
int *k;       // 每天对应的最大值

int head = 0;
int tail = -1; // case_q及index_q的首尾指针 （均指向真正的元素）

// 向量的归并排序
// 归并过程中为减少new/delete耗时及额外内存占用，使用case_q作为缓存数组
void mergeSort(int arr[], int l, int r);
void merge(int arr[], int l, int mid, int r);

void mergeSort(int arr[], int l, int r)
{ // sort [l, r)
    if (r - l < 2)
        return;
    int mid = (l + r) >> 1;
    mergeSort(arr, l, mid); // [l,mid)
    mergeSort(arr, mid, r); // [mid, r)
    if (arr[mid - 1] > arr[mid])
        merge(arr, l, mid, r); // merge
}

void merge(int arr[], int l, int mid, int r)
{
    int la = mid - l;
    int lb = r - mid;
    int *A = case_q; // A - [l,mid)
    for (int i = l; i < mid; ++i)
        A[i - l] = arr[i];
    int *B = arr + mid; // B - [mid, r)

    int i = l, j = 0, k = 0;
    while (j < la && k < lb)
    {
        if (A[j] <= B[k])
            arr[i++] = A[j++];
        else
            arr[i++] = B[k++];
    }

    while (j < la)
        arr[i++] = A[j++]; // B is done
}

// 下确界的二分查找
int lowerbound(int arr[], long long e, int l, int h)
{
    //[l,h)
    // 返回第一个大于等于给定值的地址
    while (l < h)
    {
        int mid = (l + h) >> 1;
        if ((long long)arr[mid] < e)
        {
            l = mid + 1;
        }
        else
        {
            h = mid;
        }
    }
    return l;
}

int main()
{
#ifndef _OJ_
    freopen("a.in", "r", stdin);
    freopen("a.out", "w", stdout);
#endif
    // 读取确诊人数
    scanf("%d", &n);
    cases = new int[n];
    case_q = new int[n];
    index_q = new int[n];
    k = new int[n];
    for (int i = 0; i < n; ++i)
    {
        scanf("%d", &cases[i]);
    }
    // 初始化k
    k[0] = 0;
    // 读取向前追溯的天数
    long long mi;
    scanf("%lld", &mi);           // 跳过第一天（前面没有日子）
    for (int i = 1; i < n; ++i) // 从第二天开始
    {
        scanf("%lld", &mi);                          // 读取mi
        long long min_index = ((long long)i - mi < 0) ? 0 : i - mi; // 索引下确界
        // 更新优先队列
        // 保证队列从队首到队尾单调非增
        // 保证队列从队首到队尾索引单调非减
        int new_e = cases[i - 1]; // “新”插入的元素
        while (head <= tail)
        {
            if (new_e <= case_q[tail]) // 直到出现第一个大于等于新元素的元素，或到头
                break;
            --tail; // 小于新元素的元素退出队列
        }
        // 入队
        ++tail;
        case_q[tail] = new_e;
        index_q[tail] = i - 1;
        // 根据索引下确界的单调非减性，索引小于下确界的元素出队
        while (index_q[head] < min_index)
        {
            ++head;
        }
        // 最大值一定在队首
        k[i] = case_q[head];
    }

    // 对k进行排序
    mergeSort(k, 0, n);

    // 查找
    scanf("%d", &T);
    for (int i = 0; i < T; ++i)
    {
        long long p;
        long long q;
        scanf("%lld %lld", &p, &q);
        int l = lowerbound(k, p, 0, n); // 大于等于p的最小元素索引
        int r = lowerbound(k, q, 0, n); // 大于等于q的最小元素索引
        printf("%d %d\n", l, r - l);
    }

    // 释放空间
    delete[] cases;
    delete[] case_q;
    delete[] index_q;
    delete[] k;
}
```











