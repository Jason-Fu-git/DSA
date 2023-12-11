#include "temperature.h"
#define SIZE 300000
#define INT_MAX 0x7fffffff

struct node
{
    int lc = -1; // 左孩子的index
    int rc = -1; // 右孩子的index
                 // bound box
    int l;       // left
    int r;       // right
    int u;       // upper
    int b;       // below

    int x;    // 分割点的 x 坐标
    int y;    // 分割点的 y 坐标
    int temp; // 分割点的 温度

    int min_temp; // 最小温度
    int max_temp; // 最大温度
};
// 对于叶节点,最小温度=最大温度

node kdtree[SIZE]; // 用数组存储node，以提高内存连续性
int top = 0;       // 栈顶指针

int *x_arr;    // x数组的副本
int *y_arr;    // y数组的副本
int *temp_arr; // temp数组的副本

int _min = INT_MAX;
int _max = -1;
int _x1, _x2, _y1, _y2;         // 全局变量，减少开销
int min_x, max_x, min_y, max_y; // 所有观测站的bound
// 改编自《数据结构》讲义14章quickSelect()函数
/**
 * @param a - 主排序数组
 * @param b - 随主排序数组同步变化
 * @param temp - 温度数组
 * @param n - 待排序数组区域的size
 * @return 中位数相对于传入指针的index
 */
int select_median(int *a, int *b, int *temp, int n);

// 选取中位数，同时排序 O(n)
int select_median(int *a, int *b, int *temp, int n)
{
    int k = (n - 1) >> 1; // floor

    for (int lo = 0, hi = n; lo < hi;)
    {
        int i = lo, j = hi;
        int a_pivot = a[lo], b_pivot = b[lo], temp_pivot = temp[lo]; // 各数组的pivot
        while (i < j)
        {
            do
            {
                j--;
            } while ((i < j) && (a_pivot <= a[j]));
            if (i < j)
            {
                a[i] = a[j];
                b[i] = b[j];
                temp[i] = temp[j];
            }
            do
            {
                i++;
            } while ((i < j) && (a[i] <= a_pivot));
            if (i < j)
            {
                a[j] = a[i];
                b[j] = b[i];
                temp[j] = temp[i];
            }

        } // assert: quit with i == j or j+1
        a[j] = a_pivot;
        b[j] = b_pivot;
        temp[j] = temp_pivot; // A[0,j) <= A[j] <= A(j, n)
        if (k <= j)
            hi = j; // suffix trimmed
        if (i <= k)
            lo = i;
    }
    return k;
}

// 建立kd树 (返回：新建立节点的index)
int construct(int offset, int n, bool isY)
{
    if (n == 0) // 空节点
        return -1;
    int mid = -1;
    int index = top++;
    // 更新最大值/最小值 和 bounding box
    _min = temp_arr[offset];
    _max = temp_arr[offset];
    _x1 = x_arr[offset];
    _x2 = x_arr[offset];
    _y1 = y_arr[offset];
    _y2 = y_arr[offset];
    for (int i = offset + 1; i < n; i++)
    {
        if (temp_arr[i] < _min)
            _min = temp_arr[i];
        if (temp_arr[i] > _max)
            _max = temp_arr[i];
        if (x_arr[i] < _x1)
            _x1 = x_arr[i];
        if (x_arr[i] > _x2)
            _x2 = x_arr[i];
        if (y_arr[i] < _y1)
            _y1 = y_arr[i];
        if (y_arr[i] > _y2)
            _y2 = y_arr[i];
    }
    // update bounding box and max min
    kdtree[index].l = _x1;
    kdtree[index].r = _x2;
    kdtree[index].b = _y1;
    kdtree[index].u = _y2;
    kdtree[index].max_temp = _max;
    kdtree[index].min_temp = _min;

    if (n == 1)
    { // 终止条件
        kdtree[index].x = x_arr[offset];
        kdtree[index].y = y_arr[offset];
        kdtree[index].temp = temp_arr[offset];
        return index;
    }

    if (isY) // 按Y划分
        mid = select_median(y_arr + offset, x_arr + offset, temp_arr + offset, n);
    else // 按X划分
        mid = select_median(x_arr + offset, y_arr + offset, temp_arr + offset, n);

    // 开点
    kdtree[index].x = x_arr[offset + mid];
    kdtree[index].y = y_arr[offset + mid];
    kdtree[index].temp = temp_arr[offset + mid];

    // 递归 (中位数保存至当前节点)
    kdtree[index].lc = construct(offset, mid, !isY);
    kdtree[index].rc = construct(offset + mid + 1, n - mid - 1, !isY);
    return index;
}

inline int _abs(int a)
{
    return a < 0 ? -a : a;
}

// 是否相交 (判断矩形中心和边长的关系)
inline bool isIntersect(long long l, long long r, long long b, long long u)
{
    return _abs(l + r - _x1 - _x2) <= (r - l + _x2 - _x1) && _abs(u + b - _y1 - _y2) <= (u - b + _y2 - _y1);
}

// 第一个矩形是否被包含
inline bool contains(int l, int r, int b, int u)
{
    return _x1 <= l && r <= _x2 && _y1 <= b && u <= _y2;
}

// 查找给定矩形域中气温的最大值与最小值
/**
 * @param l - left
 * @param r - right
 * @param b - below
 * @param u - upper
 * _x1 < _x2
 * _y1 < _y2
 *
 * 该函数将直接更改全局最大值/最小值，每次search前初始化为-1
 *
 */
void search(int kd_index, bool isY)
{

    node *p = &kdtree[kd_index];
    if (_min != -1 && _max >= p->max_temp && _min <= p->min_temp) // 剪枝
        return;

    if (contains(p->x, p->x, p->y, p->y)) // 单个观察站
    {                                     // 在选区内
        if (_max < p->temp)
            _max = p->temp;
        if (_min > p->temp || _min == -1)
            _min = p->temp;
    }
    // 左子树
    if (p->lc != -1)
    {
        node *lc = &kdtree[p->lc];
        if (contains(lc->l, lc->r, lc->b, lc->u)) // 被包含，直接report
        {
            _max = (lc->max_temp > _max) ? lc->max_temp : _max;
            _min = (lc->min_temp < _min || _min == -1) ? lc->min_temp : _min;
        }
        else if (isIntersect(lc->l, lc->r, lc->b, lc->u)) // 相交，必须递归
            search(p->lc, !isY);
    }

    // 右子树
    if (p->rc != -1)
    {
        node *rc = &kdtree[p->rc];
        if (contains(rc->l, rc->r, rc->b, rc->u)) // 被包含，直接report
        {
            _max = (rc->max_temp > _max) ? rc->max_temp : _max;
            _min = (rc->min_temp < _min || _min == -1) ? rc->min_temp : _min;
        }
        else if (isIntersect(rc->l, rc->r, rc->b, rc->u)) // 相交，必须递归
            search(p->rc, !isY);
    }
}

void init(int n, const int *x, const int *y, const int *t)
{
    min_x = x[0];
    max_x = x[0];
    min_y = y[0];
    max_y = y[0];

    // copy data
    x_arr = new int[n];
    y_arr = new int[n];
    temp_arr = new int[n];
    for (int i = 0; i < n; ++i)
    {
        x_arr[i] = x[i];
        if (min_x > x[i])
            min_x = x[i];
        if (max_x < x[i])
            max_x = x[i];

        y_arr[i] = y[i];
        if (min_y > y[i])
            min_y = y[i];
        if (max_y < y[i])
            max_y = y[i];

        temp_arr[i] = t[i];
    }

    // construct
    construct(0, n, true);
}

void query(int x1, int x2, int y1, int y2, int *tmin, int *tmax)
{
    // copy
    _x1 = x1;
    _x2 = x2;
    _y1 = y1;
    _y2 = y2;

    // init
    _min = -1;
    _max = -1;
    // search
    search(0, true);
    *tmin = _min;
    *tmax = _max;
}