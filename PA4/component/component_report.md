# CST 4-4 Component Report

付甲申 2022012206



## 1. 题目分析

题目要求对无向图不断进行连通操作，以及进行topK查询，其中k是固定的。考虑到保存权重的数据结构需要进行大量合并操作，同时需保持偏序性，可采用**容量为k的小顶左式堆**。对连通分量的维护可以采用**并查集**。

### 数据结构

#### 左式堆

实现思路与《数据结构》讲义相似。

左式堆节点类：

```c++
// 小顶堆节点
class Node
{
public:
    int data;
    int npl; // 空节点路径长度
    Node *lc;
    Node *rc;
    Node *parent;
    Node(int _data, int _npl = 1, Node *_lc = nullptr, Node *_rc = nullptr, Node *_parent = nullptr) : data(_data), npl(_npl), lc(_lc), rc(_rc), parent(_parent) {}
};
```

左式堆类：

```c++
// 小顶堆
class LeftistHeap
{
private:
    Node *root;                           // 树根
    int size;                             // 树规模
    int k;                                // topK限制
    inline void swap(Node *&a, Node *&b); // 交换两个节点
    inline void cutToK();                 // 把树的规模减少至k
    Node *merge(Node *a, Node *b);        // 合并两个小顶堆
public:
    LeftistHeap(int _k) : k(_k), root(nullptr), size(0) {} // 构造函数
    void insert(int data);                                 // 插入元素
    void attach(LeftistHeap *x);                           // 把堆x合并到该堆上, 该操作会清空x堆
    int delMin();                                          // 删除最小元素
    inline int getMin();                                   // 获取最小元素，规模不到k时返回-1
};
```

函数`cutToK`在两堆合并（`attach`）及插入元素（`insert`）时调用，功能是把堆的规模控制在k以内。当堆的规模大于k时，不断移除堆顶元素，直至堆的规模降到k。此时，堆顶就是该连通分量权重的topK（这是因为小顶堆的堆顶是整个堆最小的元素，而整个连通分量最大的权重也在堆里，故堆顶是整个连通分量第k大的权重）：

```c++
inline void LeftistHeap::cutToK() 
{
    if (size <= k) // 规模不超过k，直接返回
        return;
    while (size > k)
        delMin(); // 删除最小元素
}
```



#### 并查集

并查集采用`Quick-Union`类型，并在`Find`时进行路径压缩，以提高效率。同时，考虑到把小的并查集合并到大的并查集里能够减少树高的增长，故需同时维护size变量（维护高度开销较大）。

并查集维护两个数组：

```c++
UNode **unodes;
int *tag; // 并查集tag
```

其中，`tag`即节点的标签，维护连通分量，`unodes`维护每个连通分量对应的左式堆和规模，其节点`UNode`定义如下：

```c++
// 并查集Node
struct UNode
{
    int size;          // 每个等价类的大小
    LeftistHeap *heap; // 该等价类对应的左式堆
    UNode(int a, int k)
    {
        size = 1;
        heap = new LeftistHeap(k);
        heap->insert(a);
    }
};
```

为避免递归，`Find`接口采用迭代写法。同时，为避免重复扫描，在压缩路径时，把待查节点`a`和根节点`b`之间所有节点的父亲均设为`a`，并把`a`的父亲设为`b`。这样，中间节点的深度为2，相较于常规算法的1有所增加，但却避免了辅助栈的空间开销及重复扫描，同时并查集的总体高度也能保持渐进常数级：

```c++
int Find(int a) // a和等价类标志节点间所有节点都attach到a上
{
    int b = tag[a];
    while (b != tag[b]) // 不形成自环，一定有父亲
    {
        int next = tag[b];
        tag[b] = a; // a成为父亲
        b = next;   // 上溯至祖先
    }
    tag[a] = b; // 压缩路径长度至2
    return b;   // 返回等价类标志节点
}
```





## 2. 算法步骤

### （1）初始化

- 读取权值：依次读取输入的权值$a_i$，令`tag[i] = i`，`unodes[i] = new UNode(a, k)`。同时建立一个根节点为$a_i$，规模为1的左式堆。
- 读取边：依次读取各条边`(u,v)`，调用`Union(u, v)`，如果在同一个并查集内则不执行合并，否则执行并查集以及对应左式堆的合并。

### （2）执行操作

- 加边操作：对于新边`(u,v)`，调用`Union(u,v)`，如果在同一个并查集内则不执行合并，否则执行并查集以及对应左式堆的合并。
- 查询操作：对于点`u`，调用`Find(u)`找到该并查集的根节点`b`，返回其对应左式堆的堆顶元素（若左式堆规模小于k，则返回-1）。



## 3. 复杂度分析

### （1）时间复杂度 - $O((m+q+n-k)logk)$

#### 左式堆

设堆的规模为n

- 堆高 ： $O(logn)$
- 合并操作：$O(logn)$
- 最小值查询：$O(1)$
- 移除堆顶（即合并左右子堆）：$O(logn)$

#### 并查集

对于并查集高度的证明比较复杂，可参考[并查集复杂度 - OI Wiki (oi-wiki.org)](https://oi-wiki.org/ds/dsu-complexity/) 给出的证明，这里仅给出结论：

- 高度：$O(\alpha(n))$，其中$\alpha$是[Ackermann 函数](https://en.wikipedia.org/wiki/Ackermann_function) 的反函数，由于增长极其缓慢，在题目所给数据规模下，可认为是$O(1)$。
- 合并、查找操作：与高度成正比-$O(1)$。

#### 总体复杂度分析

##### 初始化权重

依次建立左式堆 - $O(n)$。

##### 初始化各边（不包含修剪多余元素）

并查集的合并操作为$O(1)$，总体$O(m)$。

考虑到每个左式堆最大规模为$k$，单次合并为$O(logk)$，总体用时为$O(mlogk+m)=O(mlogk)$。

##### 增加边（不包含修剪多余元素）

同初始化各边的情况，若进行$q$次添加，总体用时为$O(qlogk)$。

##### 查询操作

单次$O(1)$，若查询$q$次，总体用时为$O(q)$。

##### 修剪多余元素

合并之前，两个堆的最大规模为$k$，最坏情况下，合并之后需要进行$k$次修剪，故单次合并后修剪操作复杂度为$O(klogk)$。

但是，考虑到每个权重最多只能出现在一个左式堆里，并且被修剪之后就不会再次出现在任何堆里，故 **整体上最多执行$n-k$次删除操作**,  整体修剪操作的复杂度为$O((n-k)logk)$。



最坏情况下，总体复杂度为：
$$
O(n)+O(mlogk)+O(qlogk)+O(q)+O((n-k)logk) = O((m+q+n-k)logk)
$$
在题中数据规模下，可以通过。



### （2）空间复杂度 - $O(n)$

#### 左式堆

每个权重最多出现在一个堆中，故为$O(n)$。

#### 并查集

显然为$O(n)$。



考虑到程序执行过程中没有任何递归过程和辅助栈，总体空间复杂度为$O(n)$。





## 4. 源代码

```c++
#include <cstdio>

// ------------------- 以下左式堆接口模仿自《数据结构》讲义 ------------------------ //
// 小顶堆节点
class Node
{
public:
    int data;
    int npl; // 空节点路径长度
    Node *lc;
    Node *rc;
    Node *parent;
    Node(int _data, int _npl = 1, Node *_lc = nullptr, Node *_rc = nullptr, Node *_parent = nullptr) : data(_data), npl(_npl), lc(_lc), rc(_rc), parent(_parent) {}
};

// 小顶堆
class LeftistHeap
{
private:
    Node *root;                           // 树根
    int size;                             // 树规模
    int k;                                // topK限制
    inline void swap(Node *&a, Node *&b); // 交换两个节点
    inline void cutToK();                 // 把树的规模减少至k
    Node *merge(Node *a, Node *b);        // 合并两个小顶堆
public:
    LeftistHeap(int _k) : k(_k), root(nullptr), size(0) {} // 构造函数
    void insert(int data);                                 // 插入元素
    void attach(LeftistHeap *x);                           // 把堆x合并到该堆上, 该操作会清空x堆
    int delMin();                                          // 删除最小元素
    inline int getMin();                                   // 获取最小元素，规模不到k时返回-1
};

inline void LeftistHeap::swap(Node *&a, Node *&b)
{ // 节点交换函数
    Node *temp = a;
    a = b;
    b = temp;
};

inline void LeftistHeap::cutToK() // 最坏O(klogn)
{
    if (size <= k) // 规模不超过k，直接返回
        return;
    while (size > k)
        delMin(); // 删除最小元素
}

Node *LeftistHeap::merge(Node *a, Node *b)
{
    if (!a)
        return b; // 退化情况
    if (!b)
        return a; // 退化情况
    if (a->data > b->data)
        swap(a, b);                // 确保a<=b
    for (; a->rc; a = a->rc)       // 沿右侧链做二路归并，直至堆a->rc先于b变空
        if (a->rc->data > b->data) // 只有在a->rc > b时, 执行交换操作
        {
            b->parent = a;
            swap(a->rc, b);
        }
    (a->rc = b)->parent = a; // 直接接入b的残余部分（必然非空）
    for (; a; b = a, a = a->parent)
    {                                            // 从a出发沿右侧链逐层回溯（b == a->rc）
        if (!a->lc || (a->lc->npl < a->rc->npl)) // 若有必要
            swap(a->lc, a->rc);                  // 通过交换确保右子堆的npl不大
        a->npl = a->rc ? a->rc->npl + 1 : 1;     // 更新npl
    }
    return b; // 返回合并后的堆顶
} // 本算法只实现结构上的合并，堆的规模须由上层调用者负责更新

void LeftistHeap::insert(int e)
{
    root = merge(new Node(e), root); // 插入新元素
    ++size;                          // 规模加一
    cutToK();                        // 规模可能超过k
}

void LeftistHeap::attach(LeftistHeap *x)
{
    root = merge(root, x->root); // 合并两个左式堆
    size += x->size;             // 增加规模
    x->root = nullptr;           // 释放另一个左式堆
    x->size = 0;
    cutToK(); // 规模可能超过k
}

int LeftistHeap::delMin()
{
    Node *lheap = root->lc;
    Node *rheap = root->rc;
    if (lheap)
        lheap->parent = nullptr;
    if (rheap)
        rheap->parent = nullptr;
    int e = root->data;
    delete root;                // 移除根节点
    root = merge(lheap, rheap); // 合并左孩子和右孩子
    --size;                     // 规模减一
    return e;                   // 返回被删除的根节点的值
}

inline int LeftistHeap::getMin()
{
    if (size < k)
        return -1;
    return root->data;
}

// ------------------- 并查集 ------------------------ //

// 并查集Node
struct UNode
{
    int size;          // 每个等价类的大小
    LeftistHeap *heap; // 该等价类对应的左式堆
    /**
     * a - 权重
     * k - topK
     */
    UNode(int a, int k)
    {
        size = 1;
        heap = new LeftistHeap(k);
        heap->insert(a);
    }
};

UNode **unodes;
int *tag; // 并查集tag

/**
 * 此函数同时会压缩查找路径
 * a - 待查找等价类标签的节点
 * return - 等价类标签
 */
int Find(int a) // a和等价类标志节点间所有节点都attach到a上
{
    int b = tag[a];
    while (b != tag[b]) // 不形成自环，一定有父亲
    {
        int next = tag[b];
        tag[b] = a; // a成为父亲
        b = next;   // 上溯至祖先
    }
    tag[a] = b; // 压缩路径长度至2
    return b;   // 返回等价类标志节点
}

/**
 * 合并 a 和 b所在的等价类
 */
int Union(int a, int b)
{
    // 找到各自的等价类
    int fa = Find(a);
    int fb = Find(b);
    if (fa == fb) // 同一个等价类
        return fa;
    UNode *ua = unodes[fa];
    UNode *ub = unodes[fb];
    // 规模小的等价类合并到规模大的等价类上，以减小高度
    if (ua->size < ub->size)
    {
        tag[fa] = fb; // 合并到b的等价类上
        ub->size += ua->size;
        // 同时把a的左式堆合并到b中
        ub->heap->attach(ua->heap);
        return fb;
    }

    tag[fb] = fa; // 合并到a的等价类上
    ua->size += ub->size;
    // 同时把b的左式堆合并到a中
    ua->heap->attach(ub->heap);
    return fa;
}

// ----------------- 主函数 ----------------------//

int main()
{
    int n, m, k, q;
    scanf("%d %d %d %d", &n, &m, &k, &q);
    // initialize
    unodes = new UNode *[n + 1];
    tag = new int[n + 1];
    // read the weights
    for (int i = 1; i <= n; i++)
    {
        int a;
        scanf("%d", &a);
        tag[i] = i;                  // 初始化等价类标志节点
        unodes[i] = new UNode(a, k); // initialize unode
    }
    // read the edges
    for (int i = 0; i < m; i++)
    {
        int u, v;
        scanf("%d %d", &u, &v);
        Union(u, v); // union
    }
    // read the ops
    for (int i = 0; i < q; i++)
    {
        int op;
        scanf("%d", &op);
        if (op == 1)
        {
            int u, v;
            scanf("%d %d", &u, &v);
            Union(u, v); // union
        }
        else
        {
            int u;
            scanf("%d", &u);
            int root = Find(u);                           // find the root
            printf("%d\n", unodes[root]->heap->getMin()); // print the weight
        }
    }
}
```

