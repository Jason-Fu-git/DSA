#include "queue.h"
#pragma once

template <class T>
class Node;

template <class T>
using nodep = Node<T> *;

template <class T>
class Node
{
public:
    int height;
    T data;
    nodep<T> lc;
    nodep<T> rc;
    nodep<T> parent;
    Node(T data)
    {
        this->data = data;
        this->lc = nullptr;
        this->rc = nullptr;
        this->parent = nullptr;
        this->height = 0;
    }
    void insertAsLc(nodep<T> p)
    {
        lc = p;
        if(p)
            p->parent = this;
    }
    void insertAsRc(nodep<T> p)
    {
        rc = p;
        if (p)
            p->parent = this;
    }
};

template <class T>
class BT
{
protected:
    nodep<T> root;
    int size;

    // 维护树高
    void updateHeightAbove(nodep<T> );
    void updateHeight(nodep<T> );

public:
    BT()
    {
        root = nullptr;
        size = 0;
    }
    nodep<T> getRoot();
    int getSize();

    // 修改操作
    nodep<T> insert(const T &);
    nodep<T> insertAsLc(nodep<T> , const T &);
    nodep<T> insertAsRc(nodep<T> , const T &);
    int remove(nodep<T> );

    // 中序遍历意义下的直接后继
    nodep<T> succ(nodep<T> );
    // 中序遍历意义下的直接前驱
    nodep<T> pred(nodep<T> );    

    template <class VST>
    void inorder(VST &visit);
    template <class VST>
    void preorder(VST &visit);
    template <class VST>
    void postorder(VST &visit);
    template <class VST>
    void levelorder(VST &visit);
};

template <class T>
nodep<T> BT<T>::getRoot()
{
    return this->root;
}

template <class T>
nodep<T> BT<T>::insert(const T &data)
{
    ++size;
    return this->root = new Node<T>(data);
}

template <class T>
nodep<T> BT<T>::insertAsLc(nodep<T> node, const T &data)
{
    ++size;
    node->insertAsLc(new Node<T>(data));
    updateHeightAbove(node);
    return node->lc;
}

template <class T>
nodep<T> BT<T>::insertAsRc(nodep<T> node, const T &data)
{
    ++size;
    node->insertAsRc(new Node<T>(data));
    updateHeightAbove(node);
    return node->rc;
}

template <class T>
nodep<T> BT<T>::succ(nodep<T> p)
{
    nodep<T> q = p;
    if (q->rc) // 有右孩子, 返回最靠左的右孩子
    {
        q = q->rc;
        while (q->lc)
        {
            q = q->lc;
        }
    }
    else
    {                                           // 返回最小的左祖先
        while (q->parent && q->parent->rc == q) // 向左上走
            q = q->parent;
        q = q->parent; // 再向右上走一步
    }
    return q;
}

template <class T>
nodep<T> BT<T>::pred(nodep<T> p)
{
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

template <class T>
int BT<T>::remove(nodep<T> node)
{
    if (node == nullptr)
        return 0;
    updateHeightAbove(node->parent);
    int rm_size = remove(node->lc) + remove(node->rc);
    rm_size++;
    delete node;
    size -= rm_size;
    return rm_size;
}

template <class T>
template <class VST>
void BT<T>::inorder(VST &visit)
{
    nodep<T> stack[1000];
    int stack_top = -1;
    nodep<T> p = root;
    while (1)
    {
        while (p)
        {
            stack[++stack_top] = p; // 自己进栈
            p = p->lc;              // 转向左孩子
        }
        if (stack_top == -1)
            break;              // 栈中没有元素
        p = stack[stack_top--]; // 弹出栈顶
        visit(p);               // 访问自己
        p = p->rc;              // 转向右孩子
    }
}

template <class T>
template <class VST>
void BT<T>::preorder(VST &visit)
{
    // 先序遍历 data -> l -> r
    nodep<T> stack[1000];
    int stack_top = -1;
    nodep<T> p = root;
    while (1)
    {
        while (p)
        {
            visit(p);                   // 访问自己
            stack[++stack_top] = p->rc; // 右孩子进栈
            p = p->lc;                  // 转向左孩子
        }
        if (stack_top == -1)
            break; // 栈中没有元素
        p = stack[stack_top--];
    }
}

template <class T>
template <class VST>
void BT<T>::postorder(VST &visit)
{
    nodep<T> stack[1000];
    int stack_top = -1;
    nodep<T> x = root;
    if (root)
        stack[++stack_top] = root;
    while (stack_top != -1) // 栈不为空
    {
        if (stack[stack_top] != x->parent)        // 不是x的父亲，必定是x的右兄弟（因为左孩子后于右孩子进栈）
        {                                         // 更新栈，直至找到最靠左的孩子
            while (nodep<T> y = stack[stack_top]) // 栈顶不为nullptr
            {
                if (y->lc)
                { // 有左孩子
                    if (y->rc)
                    { // 有右孩子，右孩子先进栈
                        stack[++stack_top] = y->rc;
                    }
                    stack[++stack_top] = y->lc; // 左孩子进栈
                }
                else
                { // 没有左孩子，向右走一步
                    stack[++stack_top] = y->rc;
                }
            }
            stack_top--; // 弹出栈顶空节点
        }
        x = stack[stack_top--]; // 获取栈顶节点
        visit(x);               // 并访问
    }
}

template <class T>
template <class VST>
void BT<T>::levelorder(VST &visit)
{
    // 需要用到队列
    Queue<nodep<T> > q;
    if (root)
    {
        q.enqueue(root);
    }
    while (q.getSize())
    {
        nodep<T> x = q.dequeue();
        visit(x);
        if (x->lc)
            q.enqueue(x->lc);
        if (x->rc)
            q.enqueue(x->rc);
    }
}

template <class T>
void BT<T>::updateHeight(Node<T>* x){
    x->height = 0;
    if (x->lc)
        x->height = x->lc->height + 1;
    if (x->rc)
        x->height = (x->height > x->rc->height + 1) ? x->height : x->rc->height + 1;
}

template <class T>
void BT<T>::updateHeightAbove(nodep<T> x)
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

template<class T>
int BT<T>::getSize(){
    return size;
}
