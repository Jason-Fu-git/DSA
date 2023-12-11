#include <cstdio>

// 下方Node类，BST类，Splay类均改编自《数据结构》讲义

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
    void insertAsLc(Node<T> *p)
    {
        lc = p;
        if (p)
            p->parent = this;
    }
    void insertAsRc(Node<T> *p)
    {
        rc = p;
        if (p)
            p->parent = this;
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
    nodep<T> _hot; // 命中节点的父节点
    nodep<T> root; // 树根

    nodep<T> succ_child(nodep<T>); // 中序遍历意义上的直接后继（孩子）
    nodep<T> pred(nodep<T>);       // 中序遍历意义上的直接前驱（祖先）

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
nodep<T> BST<T>::pred(nodep<T> p)
{
    if (!p)
        return nullptr;
    nodep<T> q = p;
    if (q->lc) // 有左孩子, 返回最靠右的左孩子
    {
        q = q->lc;
        while (q->rc)
        {
            q = q->rc;
        }
    }
    else
    {                                           // 返回最小的右祖先
        while (q->parent && q->parent->lc == q) // 向右上走
            q = q->parent;
        q = q->parent; // 再向左上走一步
    }
    return q;
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
        if (e < _hot->data)    // e在_hot和它的直接前驱中间
            return pred(_hot); // 返回直接前驱
        else                   // 在_hot和它的直接后继中间
            return _hot;       // 返回_hot
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
    delete w;
    return true;
}

/**
 * 伸展树模板类
 */
template <class T>
class Splay : public BST<T>
{
protected:
    void splay(nodep<T> v); // 把v伸展至根
public:
    // BST的所有增删查函数需重载
    nodep<T> searchLe(const T &) override; // 找到小于等于该元素的最大元素
    nodep<T> &search(const T &e) override;
    nodep<T> insert(const T &e) override;
    bool remove(const T &e) override;
};

template <class T>
void Splay<T>::splay(nodep<T> v)
{
    if (!v)
        return;
    nodep<T> p, g;
    while ((p = v->parent) && (g = p->parent)) // 双层伸展，有父亲和祖父
    {
        nodep<T> gp = g->parent;
        if (p == g->lc)
        {
            if (v == p->lc)
            {
                g->insertAsLc(p->rc);
                p->insertAsLc(v->rc);
                p->insertAsRc(g);
                v->insertAsRc(p);
            }
            else
            {
                p->insertAsRc(v->lc);
                g->insertAsLc(v->rc);
                v->insertAsLc(p);
                v->insertAsRc(g);
            }
        }
        else
        {
            if (v == p->rc)
            {
                g->insertAsRc(p->lc);
                p->insertAsRc(v->lc);
                p->insertAsLc(g);
                v->insertAsLc(p);
            }
            else
            {
                g->insertAsRc(v->lc);
                p->insertAsLc(v->rc);
                v->insertAsLc(g);
                v->insertAsRc(p);
            }
        }

        if (!gp)
        { // v是树根
            BST<T>::root = v;
            v->parent = nullptr;
            return;
        }
        else
        {
            v->parent = gp;
            if (g == gp->lc)
                gp->lc = v;
            else
                gp->rc = v;
        }
    }

    if (p = v->parent)
    { // p是根
        if (v == p->lc)
        {
            p->insertAsLc(v->rc);
            v->insertAsRc(p);
        }
        else
        {
            p->insertAsRc(v->lc);
            v->insertAsLc(p);
        }
    }

    BST<T>::root = v;
    v->parent = nullptr;
    return;
}

template <class T>
nodep<T> &Splay<T>::search(const T &e)
{
    nodep<T> &v = BST<T>::search(e);
    splay(v ? v : BST<T>::_hot);
    return BST<T>::root; // return root
}

template <class T>
nodep<T> Splay<T>::insert(const T &e)
{
    if (!BST<T>::root)
    { // empty
        BST<T>::root = new Node<T>(e);
        return BST<T>::root;
    }

    nodep<T> v = search(e);
    if (v->data == e)
        return v; // already exists
    // if not exist, insert it around the root

    if (e < v->data)
    { // lc
        v->parent = BST<T>::root = new Node<T>(e);
        BST<T>::root->insertAsRc(v);
        BST<T>::root->insertAsLc(v->lc);
        v->lc = nullptr;
    }
    else
    { // rc
        v->parent = BST<T>::root = new Node<T>(e);
        BST<T>::root->insertAsLc(v);
        BST<T>::root->insertAsRc(v->rc);
        v->rc = nullptr;
    }
    return BST<T>::root;
}

template <class T>
bool Splay<T>::remove(const T &e)
{
    nodep<T> v = search(e);
    if (v && v->data == e)
    { // 命中，此时应移除根节点
        if (!v->rc)
        {                         // v右子树为空
            BST<T>::root = v->lc; // 左孩子为root
            if (v->lc)
                v->lc->parent = nullptr;
        }
        else
        {
            BST<T>::root = v->rc;
            BST<T>::root->parent = nullptr;
            search(e); // v右树的最小节点成为根
            BST<T>::root->lc = v->lc;
            if (v->lc)
                v->lc->parent = BST<T>::root;
        }
        delete v;
        return true;
    }
    return false;
}

template <class T>
nodep<T> Splay<T>::searchLe(const T &e)
{
    if (!BST<T>::root) // 空树
        return nullptr;
    nodep<T> x = search(e);
    if (e >= x->data) // 命中树根或是树根的直接后继
        return x;
    else                                   // e在root和它的直接前驱中间
        return BST<T>::pred(BST<T>::root); // 返回直接前驱
}

int main()
{
    // #ifndef _OJ_
    //     freopen("a.in", "r", stdin);
    //     freopen("a.out", "w", stdout);
    // #endif
    Splay<int> splay;
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
            splay.insert(x);
        else if (op == 'B')
            splay.remove(x);
        else if (op == 'C')
        {
            Node<int> *p = splay.searchLe(x);
            printf("%d\n", (p ? p->data : -1));
        }
    }
}