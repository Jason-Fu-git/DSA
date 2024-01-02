# CST 4-6 Sort

付甲申 2022012206

## 1. 排序算法选择

考虑到快速排序性能不稳定，排序算法选用 **归并排序**。

同时，根据归并排序排序次数$cnlog_kn$的公式，在$c$取最小值1的情况下，$k$需满足$k\ge4$才能通过所有测试点（在比较次数层面上）。综合三数比较器的特点，发现采用**四路归并**较为合适，可以让$c$近似取1。

### 1.1 四路归并原理

与二路归并的思想相似，每次需要选出四个子向量中最小的元素放到原向量里。对四个元素利用三数比较器全排序，最坏情况下要排序3次，这样有$c=3$，显然太复杂。

事实上，**没有必要对四个数进行全排序**。每次选出最小元素基于以下思想：在a,b,c,d四个元素中若已有两个元素确定了大小关系（不妨设事先已确定了$a<b$），则只需对a,c,d**调用一次三数比较器**就能确定a,b,c,d中的最小元。这是因为b已经大于了某个元素，一定不是最小的。

所以，在每次执行四路归并时，只需在**最开始额外调用一次三数比较器**即可，在之后的每一次比较中，前一次比较的最大元不参与比较，只比较剩余三个向量的当前首元素。

#### 1.1.1 向三路归并的退化

当其中的一个子向量完全就位后，退化为三路归并，每就位一个元素，只需简单地调用一次三数比较器即可（选出最小元）。

#### 1.1.2 向二路归并的退化

另一个子向量完全就位后，退化为二路归并，为调用三数比较器，需将其中的一个数复制一份比较，例如，欲比较a,b，只需比较a,b,b。这样，每就位一个元素，只需调用一次三数比较器（选出最小元）。

最后剩下一个向量，直接复制即可，无需调用比较器。





## 2. 算法实现细节

### 2.1 递归基

递归基选为`len`取1、2、3、4时。`len`为1、2、3的情况与第一部分的分析相似，只需调用一次比较器（`len==1`时无需调用）。对于`len==4`时，本来需要调用0+0+0+1+4=5次比较器，考虑到对4元素执行全排序仅需3次，可以节省2次调用，故额外把4作为递归基。

### 2.2 递归过程

- 划分为四个向量，前三个向量的长度均为$\lfloor len/4 \rfloor$，最后一个向量的长度为$len - 3*\lfloor len/4 \rfloor$。
- 对四个向量分别调用归并排序算法
- 执行四路归并

### 2.3 四路归并

四路归并的基本思想与上一部分的分析一致。在代码的实现细节上，由于一次要维护四个向量，放到数组里按索引维护可以节省代码量。

在全局开辟容量为4的数组，数组元素为以下结构体：

```c++
struct P
{
    int *arr; // 数组指针
    int len;  // 数组长度
    int ptr;  // 数组中待处理的元素索引

    inline int getTop()
    { // 获取当前ptr指向的元素
        return arr[ptr];
    }
};
```

由于题目所给比较器接口只能给出最大元素和最小元素，在解题过程中又单独实现了一个全排序的函数：

```c++
inline void sort3(int e1_idx, int e2_idx, int e3_idx, int *a, int *b, int *c);
```

`e1_idx`，`e2_idx`，`e3_idx`实际上为待排序元素所在数组的索引。`a`，`b`，`c`也是索引，保证其对应元素按升序排列。

四路归并过程中，只需维护已经排好序的序列对应数组的索引`s_idx`，`m_idx`，`l_idx`和上次弹出元素的数组索引`prev`。每次只需输出`s_idx`对应数组的首元素。再进行比较时，比较`prev`，`m_idx`，和上上次的`l_idx`即可。





## 3. 复杂度及比较次数分析

### 3.1 比较次数

对于长度为n的向量的归并排序，比较次数$T(n)$递推公式如下：
$$
T(n) = 4T(\frac{n}{4})+n
$$
归并过程中需额外比较一次，之后每就位一个元素比较一次，最后至少一个元素可以不经比较就位，故归并过程至多n次比较。

根据该递推公式，可得比较次数：
$$
T(n) = 1.0*nlog_4 n
$$

### 3.2 时间复杂度

- 复制次数 - $P(n) = 2.0*nlog_4 n$（每次归并时完全复制整个向量）

- 调用三数比较器的实际复杂度与调用次数成正比，故总体时间复杂度：
  $$
  O(2.0*nlog_4n)+O(c*nlog_4n)=O(nlogn)
  $$
  

### 3.3 空间复杂度

除去库中开辟的长度为n的向量，归并过程开辟了长度为n的全局缓存向量，空间复杂度为$\Omega(2n)$。

递归深度为$O(log_4 n)=O(logn)$，在题中所给数据规模下，最大为10层左右。



## 4. 源代码

```c++
#include "sort.h"

struct P
{
    int *arr; // 数组指针
    int len;  // 数组长度
    int ptr;  // 数组中待处理的元素索引

    inline int getTop()
    { // 获取当前ptr指向的元素
        return arr[ptr];
    }
};

P arr_split[4];
int *arr_tmp; // 归并过程中的缓存

// 给e1,e2,e3对应的元素排序，按升序关系存到a,b,c三个指针里
// 注：e1,e2,e3都是arr_split的index
inline void sort3(int e1_idx, int e2_idx, int e3_idx, int *a, int *b, int *c)
{
    int e1 = arr_split[e1_idx].getTop();
    int e2 = arr_split[e2_idx].getTop();
    int e3 = arr_split[e3_idx].getTop();
    int max, min;
    compare(e1, e2, e3, &max, &min);
    // 找到对应的元素
    if (max == e1)
    {
        *c = e1_idx;
        if (min == e2)
        {
            *a = e2_idx;
            *b = e3_idx;
        }
        else
        {
            *a = e3_idx;
            *b = e2_idx;
        }
    }
    else if (max == e2)
    {
        *c = e2_idx;
        if (min == e1)
        {
            *a = e1_idx;
            *b = e3_idx;
        }
        else
        {
            *a = e3_idx;
            *b = e1_idx;
        }
    }
    else
    {
        *c = e3_idx;
        if (min == e1)
        {
            *a = e1_idx;
            *b = e2_idx;
        }
        else
        {
            *a = e2_idx;
            *b = e1_idx;
        }
    }
}

// 四路归并
void merge_sort(int *arr, int len)
{
    if (len == 4)
    {
        int max, min, middle;
        // 两次三排序
        compare(arr[0], arr[1], arr[2], &max, &min);
        compare(min, max, arr[3], &max, &min);
        // 最小元素就位
        for (int i = 1; i < 4; ++i)
            if (arr[i] == min)
            {
                // 交换
                int tmp = arr[i];
                arr[i] = arr[0];
                arr[0] = tmp;
                break;
            }
        // 退化成三排序
        merge_sort(arr + 1, 3);
        return;
    }
    else if (len == 3)
    {
        int max, min, middle;
        // 一次三排序
        compare(arr[0], arr[1], arr[2], &max, &min);
        // 判断中间的元素
        for (int i = 0; i < 3; i++)
            if (arr[i] != max && arr[i] != min)
            {
                middle = arr[i];
                break;
            }
        // 就位
        arr[0] = min;
        arr[1] = middle;
        arr[2] = max;
        return;
    }
    else if (len == 2)
    {
        int max, min;
        // 二排序
        compare(arr[0], arr[1], arr[1], &max, &min);
        // 就位
        arr[0] = min;
        arr[1] = max;
        return;
    }
    else if (len <= 1)
    {           // len == 1
        return; // do nothing
    }
    // len > 4
    // split into 4 vectors
    int quad = len / 4;
    // sort them respectively
    merge_sort(arr, quad);
    merge_sort(arr + quad, quad);
    merge_sort(arr + quad * 2, quad);
    merge_sort(arr + quad * 3, len - quad * 3);
    // duplicate
    for (int i = 0; i < len; ++i)
        arr_tmp[i] = arr[i];
    // 更新指针，指向缓存区
    arr_split[0] = {arr_tmp, quad, 0};                      // a
    arr_split[1] = {arr_tmp + quad, quad, 0};               // b
    arr_split[2] = {arr_tmp + quad * 2, quad, 0};           // c
    arr_split[3] = {arr_tmp + quad * 3, len - quad * 3, 0}; // d
    // merge them
    int s_idx, m_idx, l_idx; // l_idx 是在某一偏序关系中的最大元 (在arr_split中的索引)
    int prev = 3;            // 下次等待遍历的向量的编号(在arr_split中的索引)
    int arr_count = 4;       // 当前尚未完全归并的数组数
    // 首先对前三个向量的首元素进行排序
    sort3(0, 1, 2, &s_idx, &m_idx, &l_idx);
    // 执行归并
    for (int i = 0; i < len; ++i)
    {
        if (arr_count == 4)
        {                                                      // 四路归并
            int tmp = l_idx;                                   // 该元素不参与排序
            sort3(prev, s_idx, m_idx, &s_idx, &m_idx, &l_idx); // 排序
            // 把最小元存入
            arr[i] = arr_split[s_idx].getTop();
            // 更新该最小元所在数组的指针
            if (++arr_split[s_idx].ptr == arr_split[s_idx].len) // 该数组所有元素都已经就位
            {
                arr_count = 3; // 更新剩余数组数目
                // 复制
                for (int j = s_idx + 1; j < 4; ++j)
                {
                    arr_split[j - 1].arr = arr_split[j].arr;
                    arr_split[j - 1].ptr = arr_split[j].ptr;
                    arr_split[j - 1].len = arr_split[j].len;
                }
                continue;
            }
            prev = s_idx; // 更新下次排序的向量编号
            s_idx = tmp;
        }
        else if (arr_count == 3) // 3路归并
        {
            // 直接排序
            sort3(0, 1, 2, &s_idx, &m_idx, &l_idx);
            // 把最小元存入
            arr[i] = arr_split[s_idx].getTop();
            // 更新该最小元所在数组的指针
            if (++arr_split[s_idx].ptr == arr_split[s_idx].len) // 该数组所有元素都已经就位
            {
                arr_count = 2; // 更新剩余数组数目
                // 复制
                for (int j = s_idx + 1; j < 3; ++j)
                {
                    arr_split[j - 1].arr = arr_split[j].arr;
                    arr_split[j - 1].ptr = arr_split[j].ptr;
                    arr_split[j - 1].len = arr_split[j].len;
                }
                continue;
            }
        }
        else if (arr_count == 2)
        {                                           // 二路归并
            sort3(0, 1, 1, &s_idx, &m_idx, &l_idx); // 3排序退化为2排序
            // 把最小元存入
            arr[i] = arr_split[s_idx].getTop();
            // 更新该最小元所在数组的指针
            if (++arr_split[s_idx].ptr == arr_split[s_idx].len) // 该数组所有元素都已经就位
            {
                arr_count = 1; // 更新剩余数组数目
                // 复制
                for (int j = s_idx + 1; j < 2; ++j)
                {
                    arr_split[j - 1].arr = arr_split[j].arr;
                    arr_split[j - 1].ptr = arr_split[j].ptr;
                    arr_split[j - 1].len = arr_split[j].len;
                }
                continue;
            }
        }
        else
        { // 单路，直接复制
            arr[i] = arr_split[0].getTop();
            ++arr_split[0].ptr;
        }
    }
}

void sort(int n, int limit, int *a)
{
    arr_tmp = new int[n];
    merge_sort(a, n);
    delete[] arr_tmp;
}
```

