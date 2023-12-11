#include "BST.h"
template <class T>
class AVL : public BST<T>
{
private:
    bool isBalanced(nodep<T>);
    nodep<T> tallerChild(nodep<T>);

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
    ++BT<T>::size;

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
