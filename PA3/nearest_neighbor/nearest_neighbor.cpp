#include <cstdio>
#include <climits>
#define MAX_CHILD 8

// kd-tree node
struct node
{
    node *lc = nullptr;
    node *rc = nullptr;
    int *coordinate;    // 该节点所存储向量的坐标
    int **child;        // 叶节点保存多个
    int d;              // 分割的维度
    int child_num = -1; // 如果该节点的孩子是叶节点，该字段表示child第一维的n
} *_root;

int **_x; // 保存输入
int _d;   // 维度数
int _n;   // 点的个数

node **stack; // 查找栈
int top = -1; // 栈顶指针

// 选取中位数，同时排序 O(n)
// a - 数组
// d - 待排序的维度 (从0开始)
// n - 数组中的数据量
int select_median(int **a, int d, int n)
{
    int k = (n - 1) >> 1; // floor

    for (int lo = 0, hi = n; lo < hi;)
    {
        int i = lo, j = hi;
        int *pivot = a[lo];

        while (i < j)
        {
            do
                j--;
            while ((i < j) && (pivot[d] <= a[j][d]));
            if (i < j)

                a[i] = a[j];

            do
                i++;
            while ((i < j) && (a[i][d] <= pivot[d]));
            if (i < j)
                a[j] = a[i];

        } // assert: quit with i == j or j+1
        a[j] = pivot;
        // A[0,j) <= A[j] <= A(j, n)
        if (k <= j)
            hi = j; // suffix trimmed
        if (i <= k)
            lo = i;
    }
    return k;
}

// 建立kd树 (返回：新建立节点)
node *construct(int offset, int d, int n)
{
    // 开点 (中位数保存至当前节点)
    node *p = new node;
    p->d = d;
    if (n <= MAX_CHILD) // 叶节点储存多个, 直接保存
    {
        p->child_num = n;
        p->child = new int *[n];
        // copy
        for (int i = 0; i < n; i++)
        {
            p->child[i] = new int[_d];
            for (int j = 0; j < _d; j++)
                p->child[i][j] = _x[offset + i][j];
        }
        return p;
    }

    // 划分,取中位数
    int mid = select_median(_x + offset, d, n);
    // copy
    p->coordinate = new int[_d];
    for (int i = 0; i < _d; i++)
        p->coordinate[i] = _x[offset + mid][i];

    // 递归
    p->lc = construct(offset, (d + 1) % _d, mid);
    p->rc = construct(offset + mid + 1, (d + 1) % _d, n - mid - 1);
    return p;
}

inline void search(node *p, int *data)
{
    stack[++top] = p;          // 入栈
    while (p->child_num == -1) // 内部节点
    {
        int d = p->d;
        // 左边
        if (data[d] < p->coordinate[d])
            p = p->lc;
        else
            p = p->rc;
        stack[++top] = p; // 入栈
    }
}

long long get_min_distance(node *root, int *data, int previous_top, long long min_distance)
{
    search(root, data); // 先进行一遍搜索
    while (top > previous_top)
    {
        node *p = stack[top--]; // 退栈
        if (p->child_num > -1)
        { // 到达叶节点，暴力枚举
            for (int i = 0; i < p->child_num; i++)
            {
                long long distance = 0;
                for (int j = 0; j < _d; j++)
                    distance += (long long)(p->child[i][j] - data[j]) * (long long)(p->child[i][j] - data[j]);
                if (distance < min_distance)
                    min_distance = distance;
            }
        }
        else
        { // 内部节点，检查距离
            long long distance = 0;
            for (int j = 0; j < _d; j++)
                distance += (long long)(p->coordinate[j] - data[j]) * (long long)(p->coordinate[j] - data[j]);
            if (distance < min_distance)
                min_distance = distance;
        }

        // 检查是否与p父亲的超平面相交
        if (top > previous_top) // 有父亲
        {
            node *q = stack[top];
            long long dis = q->coordinate[q->d] - data[q->d]; // 球心到超平面的距离
            if (dis * dis <= min_distance)
            { // 相交, 则搜索另一侧的孩子 (p的兄弟)
                node *another_child = q->lc;
                if (p == q->lc)
                    another_child = q->rc;
                if (another_child) // 递归搜索另一侧的孩子 (p的兄弟)
                    min_distance = get_min_distance(another_child, data, top, min_distance);
            }
        }
    }
    return min_distance;
}

int main()
{
    scanf("%d %d", &_d, &_n);
    // 初始化
    _x = new int *[_n];
    stack = new node *[_n];
    // 输入所有节点
    for (int i = 0; i < _n; i++)
    {
        _x[i] = new int[_d];
        for (int j = 0; j < _d; j++)
            scanf("%d", &_x[i][j]);
    }
    // 建立kd树
    _root = construct(0, 0, _n);
    // 搜索
    int q;
    int *data = new int[_d];
    scanf("%d", &q);
    for (int i = 0; i < q; i++)
    {
        for (int j = 0; j < _d; j++)
            scanf("%d", &data[j]);
        printf("%lld\n", get_min_distance(_root, data, -1, LLONG_MAX));
    }
}