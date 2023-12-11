#include "BT.h"

template <class T>
class BST : public BT<T>
{
protected:
    nodep<T> _hot; // 命中节点的父节点
    nodep<T> connect34(nodep<T>, nodep<T>, nodep<T>,
                       nodep<T>, nodep<T>, nodep<T>, nodep<T>); // 3 + 4 重构
    nodep<T> rotateAt(nodep<T>);                                // v必有祖父
public:
    virtual nodep<T> &search(const T &);
    virtual nodep<T> insert(const T &);
    virtual bool remove(const T &);
};

template <class T>
nodep<T> &BST<T>::search(const T &e) // _hot指向命中节点的父亲
{
    if (!BT<T>::root || BT<T>::root->data == e)
    {
        _hot = nullptr;
        return BT<T>::root;
    }
    for (_hot = BT<T>::root;;)
    {
        nodep<T> &p = (e < _hot->data) ? _hot->lc : _hot->rc;
        if (!p || p->data == e)
            return p;
        _hot = p;
    }
}

template <class T>
nodep<T> BST<T>::insert(const T &e)
{
    nodep<T> &p = search(e);
    if (!p)
    {
        p = new Node<T>(e);
        p->parent = _hot;
        ++BT<T>::size;
    }
    return p;
}

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
        w = BT<T>::succ(w);

        T tmp = x->data; // 令x与其后继w互换数据
        x->data = w->data;
        w->data = tmp;

        nodep<T> u = w->parent;                  // 原问题即转化为，摘除非二度的节点w
        (u == x ? u->rc : u->lc) = succ = w->rc; // 兼顾特殊情况：u可能就是x
    }

    _hot = w->parent; // 记录实际被删节点的父亲
    if (succ)
        succ->parent = _hot; // 如果后继存在，更新parent
    BT<T>::updateHeightAbove(_hot);
    delete w;
    --BT<T>::size;
    return true;
}

/*
-----b-----
--a-----c--
T0-T1--T2-T3
*/
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

    BT<T>::updateHeight(a);
    BT<T>::updateHeightAbove(c);
    return b;
}

template <class T>
nodep<T> BST<T>::rotateAt(nodep<T> v)
{
    nodep<T> p = v->parent;
    nodep<T> g = p->parent;
    if (p == g->lc)
    { // zig
        /*
            ------g
            ----p--
            --v----
        */

        if (v == p->lc)
        { // zig-zig
            p->parent = g->parent;
            return connect34(v, p, g, v->lc, v->rc, p->rc, g->rc);
        }
        /*
            ------g
            --p----
            -----v-
        */
        else
        { // zig-zag
            v->parent = g->parent;
            return connect34(p, v, g, p->lc, v->lc, v->rc, g->rc);
        }
    }
    else
    {
        /*
            -g-----
            ----p--
            ------v
        */
        if (v == p->rc)
        { // zag-zag
            p->parent = g->parent;
            return connect34(g, p, v, g->lc, p->lc, v->lc, v->rc);
        }
        /*
            -g-----
            -----p-
            ---v---
        */

        else
        { // zag-zig
            v->parent = g->parent;
            return connect34(g, v, p, g->lc, v->lc, v->rc, p->rc);
        }
    }
}