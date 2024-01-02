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