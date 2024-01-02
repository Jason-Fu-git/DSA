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