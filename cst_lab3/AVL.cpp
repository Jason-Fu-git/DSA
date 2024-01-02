#include <cstdio>
#include <ctime>
// 下方Node类，BST类，AVL类均改编自《数据结构》讲义

/**
 * 节点模板类
 */
template <class T>
class Node
{
public:
    int height;
    T data;
    Node<T> *lc;
    Node<T> *rc;
    Node<T> *parent;
    Node(T data)
    {
        this->data = data;
        this->lc = nullptr;
        this->rc = nullptr;
        this->parent = nullptr;
        this->height = 0;
    }
};

// shortcut
template <class T>
using nodep = Node<T> *;

/**
 * 二叉搜索树模板类
 */
template <class T>
class BST
{
protected:
    nodep<T> _hot;                    // 命中节点的父节点
    nodep<T> root;                    // 树根
    nodep<T> succ_child(nodep<T>);    // 中序遍历意义上的直接后继（孩子）
    nodep<T> pred_ancestor(nodep<T>); // 中序遍历意义上的直接前驱（祖先）
    nodep<T> connect34(nodep<T>, nodep<T>, nodep<T>,
                       nodep<T>, nodep<T>, nodep<T>, nodep<T>);
    nodep<T> rotateAt(nodep<T>);

    void updateHeight(nodep<T>);
    void updateHeightAbove(nodep<T>);

public:
    BST() : root(nullptr), _hot(nullptr)
    {
    }
    virtual nodep<T> searchLe(const T &); // 找到小于等于该元素的最大元素
    virtual nodep<T> &search(const T &);  // 普通搜索
    virtual nodep<T> insert(const T &);   // 插入
    virtual bool remove(const T &);       // 删除
};

// 直接后继
template <class T>
nodep<T> BST<T>::succ_child(nodep<T> p)
{
    p = p->rc;
    while (p->lc)
        p = p->lc;
    return p;
}

// 直接前驱
template <class T>
nodep<T> BST<T>::pred_ancestor(nodep<T> p)
{
    if (!p)
        return nullptr;
    // 返回最小的右祖先
    while (p->parent && p->parent->lc == p) // 向右上走
    {
        p = p->parent;
    }

    p = p->parent; // 再向左上走一步
    return p;
}

// 更新单个节点的高度
template <class T>
void BST<T>::updateHeight(nodep<T> x)
{
    if (x)
    {
        x->height = 0;
        if (x->lc)
            x->height = x->lc->height + 1;
        if (x->rc)
            x->height = (x->height > x->rc->height + 1) ? x->height : x->rc->height + 1;
    }
}

// 更新所有父节点（包括自己）的高度
template <class T>
void BST<T>::updateHeightAbove(nodep<T> x)
{
    while (x)
    {
        int old_height = x->height;
        updateHeight(x);
        // 树高已经改变，则递归更改
        if (old_height != x->height)
            x = x->parent;
        else // 树高不变，父亲的树高一定不变
            break;
    }
}

// 旋转
template <class T>
nodep<T> BST<T>::rotateAt(nodep<T> v)
{
    Node<T> *p = v->parent;
    Node<T> *g = p->parent;
    if (p == g->lc)
    { // zig
        if (v == p->lc)
        { // zig-zig
            p->parent = g->parent;
            return connect34(v, p, g, v->lc, v->rc, p->rc, g->rc);
        }
        else
        { // zig-zag
            v->parent = g->parent;
            return connect34(p, v, g, p->lc, v->lc, v->rc, g->rc);
        }
    }
    else
    {
        if (v == p->rc)
        { // zag-zag
            p->parent = g->parent;
            return connect34(g, p, v, g->lc, p->lc, v->lc, v->rc);
        }
        else
        { // zag-zig
            v->parent = g->parent;
            return connect34(g, v, p, g->lc, v->lc, v->rc, p->rc);
        }
    }
}

// 3+4重构
template <class T>
nodep<T> BST<T>::connect34(nodep<T> a, nodep<T> b, nodep<T> c,
                           nodep<T> T0, nodep<T> T1, nodep<T> T2, nodep<T> T3)
{
    // connect a T0 T1
    a->lc = T0;
    a->rc = T1;
    if (T0)
        T0->parent = a;
    if (T1)
        T1->parent = a;

    // connect c T2 T3
    c->lc = T2;
    c->rc = T3;
    if (T2)
        T2->parent = c;
    if (T3)
        T3->parent = c;

    // connect b a c
    b->lc = a;
    b->rc = c;
    a->parent = b;
    c->parent = b;

    updateHeight(a);
    updateHeightAbove(c);
    return b;
}

// 普通搜索算法
template <class T>
nodep<T> &BST<T>::search(const T &e) // _hot指向命中节点的父亲，模仿讲义，返回引用
{
    if (!root || root->data == e)
    {
        _hot = nullptr;
        return root;
    }
    for (_hot = root;;)
    {
        nodep<T> &p = (e < _hot->data) ? _hot->lc : _hot->rc;
        if (!p || p->data == e)
            return p;
        _hot = p;
    }
}

// 搜索不大于e的最大关键码
template <class T>
nodep<T> BST<T>::searchLe(const T &e)
{
    if (!root) // 空树
        return nullptr;
    nodep<T> &x = search(e);
    if (x) // 不为空，说明有等于它的元素
        return x;
    else // 为空
    {
        if (e < _hot->data)             // e在_hot和它的直接前驱中间
            return pred_ancestor(_hot); // 返回直接前驱
        else                            // 在_hot和它的直接后继中间
            return _hot;                // 返回_hot
    }
}

// 查找算法，直接修改引用
template <class T>
nodep<T> BST<T>::insert(const T &e)
{
    nodep<T> &p = search(e);
    if (!p)
    {
        p = new Node<T>(e);
        p->parent = _hot;
    }
    return p;
}

// 删除算法，直接修改引用
template <class T>
bool BST<T>::remove(const T &e)
{
    nodep<T> &x = search(e); // 定位目标节点
    if (!x)
        return false;        // 确认目标存在（此时_hot为x的父亲）
    nodep<T> w = x;          // 实际被摘除的节点，初值同x
    nodep<T> succ = nullptr; // 实际被删除节点的接替者
    if (!x->lc)
        succ = x = x->rc; // 左子树为空
    else if (!x->rc)
        succ = x = x->lc; // 右子树为空
    else
    { // 左右子树都不为空
        w = succ_child(w);

        T tmp = x->data; // 令x与其后继w互换数据
        x->data = w->data;
        w->data = tmp;

        nodep<T> u = w->parent;                  // 原问题即转化为，摘除非二度的节点w
        (u == x ? u->rc : u->lc) = succ = w->rc; // 兼顾特殊情况：u可能就是x
    }

    _hot = w->parent; // 记录实际被删节点的父亲
    if (succ)
        succ->parent = _hot; // 如果后继存在，更新parent
    updateHeightAbove(_hot);
    delete w;
    return true;
}

/**
 * AVL 类
 */
template <class T>
class AVL : public BST<T>
{
private:
    bool isBalanced(nodep<T>);      // 是否平衡
    nodep<T> tallerChild(nodep<T>); // 较高的孩子，如果登高，选择同侧的

public:
    nodep<T> insert(const T &) override;
    bool remove(const T &) override;
};

template <class T>
bool AVL<T>::isBalanced(nodep<T> node)
{
    int lh = (node->lc) ? node->lc->height : -1;
    int rh = (node->rc) ? node->rc->height : -1;
    return (lh - rh) <= 1 && (rh - lh) <= 1;
}

template <class T>
nodep<T> AVL<T>::tallerChild(nodep<T> node)
{
    int lh = (node->lc) ? node->lc->height : -1;
    int rh = (node->rc) ? node->rc->height : -1;
    if (lh > rh)
        return node->lc;
    else if (lh < rh)
        return node->rc;
    else
    { // 与父亲同侧者优先
        if (node == node->parent->lc)
            return node->lc;
        return node->rc;
    }
}

template <class T>
nodep<T> AVL<T>::insert(const T &e)
{
    nodep<T> &p = BST<T>::search(e);
    if (p)
        return p;             // p不为空节点，说明e已经存在
    p = new Node<T>(e);       // 创建新节点
    p->parent = BST<T>::_hot; // 连接父亲
    nodep<T> q = p;

    // 重平衡
    for (nodep<T> g = BST<T>::_hot; g; g = g->parent)
    {
        if (!isBalanced(g))
        {                          // 祖先节点失衡
            if (BST<T>::root == g) // 根节点失衡
                BST<T>::root = BST<T>::rotateAt(tallerChild(tallerChild(g)));
            else
            {
                if (g->parent->lc == g)
                    g->parent->lc = BST<T>::rotateAt(tallerChild(tallerChild(g)));
                else
                    g->parent->rc = BST<T>::rotateAt(tallerChild(tallerChild(g)));
            }
            break; // 全树必定复衡
        }
        else
        { // 祖先节点未必平衡，仅更新高度
            BST<T>::updateHeight(g);
        }
    }
    return q;
}

template <class T>
bool AVL<T>::remove(const T &e) // O(logn) 复杂度较高，可能一次调整多个节点
{
    if (BST<T>::remove(e))
    {
        // 重平衡
        for (nodep<T> g = BST<T>::_hot; g; g = g->parent)
        {
            if (!isBalanced(g))
            {                          // 祖先节点失衡
                if (BST<T>::root == g) // 根节点失衡
                    g = BST<T>::root = BST<T>::rotateAt(tallerChild(tallerChild(g)));
                else
                {
                    if (g->parent->lc == g)
                        g = g->parent->lc = BST<T>::rotateAt(tallerChild(tallerChild(g)));
                    else
                        g = g->parent->rc = BST<T>::rotateAt(tallerChild(tallerChild(g)));
                }
            }
            BST<T>::updateHeight(g);
        }
    }
    return false;
}

int main()
{
#ifndef _OJ_
    freopen("dl.in", "r", stdin);
    freopen("dl_avl.out", "w", stdout);
#endif
    double t0 = clock();
    AVL<int> avl;
    int n; // 操作数目
    scanf("%d", &n);
    // 处理输入
    for (int i = 0; i < n; i++)
    {
        getchar(); // 读取上一行的换行符
        int x;
        char op;
        scanf("%c %d", &op, &x);
        if (op == 'A')
            avl.insert(x);
        else if (op == 'B')
            avl.remove(x);
        else if (op == 'C')
        {
            Node<int> *p = avl.searchLe(x);
            printf("%d\n", (p ? p->data : -1));
        }
    }
    double t1 = clock();
    printf("AVL time : %.2lf ms\n ", t1 - t0);
}