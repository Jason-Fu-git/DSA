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
// 归并过程中为减少new/delete耗时，使用case_q作为缓存数组！
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
int lowerbound(int arr[], int e, int l, int h)
{
    //[l,h)
    // 返回第一个大于等于给定值的地址
    while (l < h)
    {
        int mid = (l + h) >> 1;
        if (arr[mid] < e)
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
    int mi;
    scanf("%d", &mi);           // 跳过第一天（前面没有日子）
    for (int i = 1; i < n; ++i) // 从第二天开始
    {
        scanf("%d", &mi);                          // 读取mi
        int min_index = (i - mi < 0) ? 0 : i - mi; // 索引下确界
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
        int p, q;
        scanf("%d %d", &p, &q);
        int l = lowerbound(k, p, 0, n); // 大于等于p的最小元素索引
        int r = lowerbound(k, q, 0, n); // 大于等于q的最小元素索引
        printf("%d %d\n", l, r - l);
    }
}