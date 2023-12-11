# CST 3-3-2 Nearest Neighbor Report

付甲申 2022012206



## 1. 题目分析

题目是典型的N维空间最近邻问题，数据结构采用**kd树**，节点的定义如下：

```c++
struct node
{
    node *lc = nullptr;
    node *rc = nullptr;
    int *coordinate;    // 该节点所存储向量的坐标
    int **child;        // 叶节点保存多个
    int d;              // 分割的维度
    int child_num = -1; // 如果该节点的孩子是叶节点，该字段表示child第一维的n
};
```

为提高局部性，减少递归层数，kd树的每个内部节点仅在`coordinate`数组中储存中位数节点的坐标。对于叶节点，在`child`数组中储存多个节点，`child_num`变量保存叶节点储存节点的数目。经测试，叶节点储存节点数目小于等于**8**时效率达到最优水平。若大于这个数字，暴力枚举叶节点的开销将大于减少递归深度的开销。

kd树的建造过程采用逐维度中位数划分方法。若使建造kd树的时间复杂度达到$O(nlogn)$，中位数查找算法的时间复杂度必须为$O(n)$，且在查找中位数的同时要对数组进行划分。综合以上因素，中位数查找算法采用类似`stl`库`nth_element`的算法（讲义14-B3），其中的`n`取待划分数组规模的一半即可。当待划分数组规模小于等于阈值**8**时，不再划分，直接存到叶节点里。

考虑到数据的均匀性不错，最近邻查找时可以采取“查找+回溯”的方法。设所给向量为`v`。首先，采用类似BST中`search`算法的方法，从根节点查找到叶节点，形成查找序列$\{v_i\}$，使`v`落在叶节点对应的局部空间内，`v`的最近邻大概率就在叶节点里。接着，从叶节点里找到`v`的最近邻，并沿着搜索路径回溯。每回溯一步，弹出节点$v_i$，更新`v`的最近邻，并判断以`v`为球心，当前距离最小值为半径的球是否与$v_{i-1}$的划分超平面$S$相交。若相交，$S$另一侧的空间内可能存在`v`的最近邻，需要重复上述“搜索+回溯”过程，直至回到根节点。这样查找，最坏情况为$O(n)$，但考虑到数据较均匀，“搜索+回溯”的递归次数不会太多，故复杂度可以接近$O(logn)$。



## 2. 解题步骤

### （1）输入及预处理

逐个读取输入的向量，储存到二维数组`_x`中。

### （2） 建立kd树

采用如下的递归过程：

- 若当前待划分数组规模`n`小于等于`MAX_CHILD`宏（=8），结束递归，把数组中所有点存到新开辟节点的`child`数组中。
- 若`n`大于`MAX_CHILD`，执行以下过程：
  - 调用中位数划分函数`select_median`，使特定维度$d$的中位数$x_{d,mid}$就位，中位数左边元素坐标分量满足$x_d \leq x_{d,mid}$，右边元素坐标分量满足$x_d \geq x_{d,mid}$。
  - 把中位元素$x_{mid}$的坐标存到新开辟节点`p`的`coordinate`数组中。
  - 递归，继续划分$x_{mid}$左边的元素和右边的元素，返回的节点分别作为`p`的左孩子和右孩子。

### （3）查找最近邻

对于每一个待查找向量`v`，进行如下操作：

#### ① 搜索

从维度0开始，每层搜索的维度循环加一。在进行第i层搜索时，设目标维度为$d$，当前节点为$x_i$。若$v_d \lt x_{i,d} $，说明`v`落在其左空间内，则继续搜索其左孩子；反之，继续搜索其右孩子。并将$x_i$入栈。直到搜索到叶节点

#### ② 回溯

- 弹出栈顶节点$x_{top}$
- 若$x_{top}$为叶节点（对应`child_num `>0），暴力枚举其中储存的节点，更新最近欧氏距离平方$dis_m$；若其为叶节点，只需按照其坐标更新$dis_m$即可。
- 若栈中仍有节点，必为$x_{top}$的父亲$x_p$。设其划分维度为$d$，检查`v`距其划分超平面的距离平方$(v_d - x_{p,d})^2$是否小于等于$dis_m$，若为真，说明$x_p$的另一个划分子空间内可能存在`v`的最近邻，继续进行 “搜索+回溯”操作；若为假，继续检查栈顶结点，直至栈为空。



## 3. 复杂度分析

设数据规模为`n`，查找次数为`q`，维度为`d`。

### (1) 时间复杂度

- 数据预处理 $O(dn)$
- 中位数划分 $O(n)$ + 叶节点及内部节点拷贝 $O(d)$ + 建树过程二分 => kd树建树过程 $O(dnlogn)$

- 查找过程
  - 最坏情况下，需要遍历所有节点，时间复杂度$O(qdn)$
  - 数据均匀情况下，大概率在一次“搜索+回溯”过程中找到最近邻，考虑到树高为$O(logn)$，时间复杂度为$O(2qdlogn+8qd) = O(qdlogn)$ 

综上，在数据均匀分布的情况下，理想的时间复杂度为$O((n+q)dlogn)$。考虑到`d`极小，可以视为常数，故为$O((n+q)logn)$。

### (2) 空间复杂度

- 数据预处理 $O(dn)$
- kd树内部节点储存一个向量，叶节点储存小于等于8个向量，空间复杂度为$O(dn)$

综上，总体空间复杂度为$O(dn)$。考虑到`d`极小，可以视为常数，故为$O(n)$



## 4. 源码

```c++
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
```



