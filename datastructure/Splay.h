#include "BST.h"
template <class T>
class Splay : public BST<T>
{
protected:
    void splay(Node<T> *v); // 把v伸展至根
public:
    Node<T> *search(const T &e) override;
    Node<T> *insert(const T &e) override;
    bool remove(const T &e) override;
};

template <class T>
void Splay<T>::splay(Node<T> *v)
{
    if (!v)
        return;
    Node<T> *p, *g;
    while ((p = v->parent) && (g = p->parent)) // 双层伸展，有父亲和祖父
    {
        Node<T> *gp = g->parent;
        if (p == g->lc)
        {

            /*
            ------g--
            ---p-----
            -v-------
            */
            if (v == p->lc)
            {
                g->insertAsLc(p->rc);
                p->insertAsLc(v->rc);
                p->insertAsRc(g);
                v->insertAsRc(p);
            }
            /*
            ------g--
            --p------
            -----v---
            */
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

            /*
            --g-----
            ----p---
            -------v
            */
            if (v == p->rc)
            {
                g->insertAsRc(p->lc);
                p->insertAsRc(v->lc);
                p->insertAsLc(g);
                v->insertAsLc(p);
            }
            /*
            ----g----
            --------p
            -----v---
            */
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
            BT<T>::root = v;
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
        BT<T>::updateHeight(g);
        BT<T>::updateHeight(p);
        BT<T>::updateHeight(v);
    }

    if (p = v->parent)
    { // p是根
        /*
        ---root---
        --v---q---
        */
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
        BT<T>::updateHeight(p);
        BT<T>::updateHeight(v);
    }

    BT<T>::root = v;
    v->parent = nullptr;
    return;
}

template <class T>
Node<T> *Splay<T>::search(const T &e)
{
    Node<T> *v = BST<T>::search(e);
    splay(v ? v : BST<T>::_hot);
    return BT<T>::root;
}

template <class T>
Node<T> *Splay<T>::insert(const T &e)
{
    if (BT<T>::size == 0)
    { // empty
        BT<T>::root = new Node<T>(e);
        ++BT<T>::size;
        return BT<T>::root;
    }

    Node<T> *v = search(e);
    if (v->data == e)
        return v; // already exists
    // if not exist, insert it around the root

    if (e < v->data)
    { // lc
        v->parent = BT<T>::root = new Node<T>(e);
        BT<T>::root->insertAsRc(v);
        BT<T>::root->insertAsLc(v->lc);
        v->lc = nullptr;
    }
    else
    { // rc
        v->parent = BT<T>::root = new Node<T>(e);
        BT<T>::root->insertAsLc(v);
        BT<T>::root->insertAsRc(v->rc);
        v->rc = nullptr;
    }

    ++BT<T>::size;
    BT<T>::updateHeightAbove(v);
    return BT<T>::root;
}

template <class T>
bool Splay<T>::remove(const T &e)
{
    Node<T> *v = search(e);
    if (v && v->data == e)
    { // 命中，此时应移除根节点
        if (!v->rc)
        {                        // v右子树为空
            BT<T>::root = v->lc; // 左孩子为root
            if (v->lc)
                v->lc->parent = nullptr;
        }
        else
        {
            BT<T>::root = v->rc;
            BT<T>::root->parent = nullptr;
            search(e); // v右树的最小节点成为根
            BT<T>::root->lc = v->lc;
            if (v->lc)
                v->lc->parent = BT<T>::root;
        }
        delete v;
        --BT<T>::size;
        BT<T>::updateHeight(BT<T>::root);
        return true;
    }
    return false;
}