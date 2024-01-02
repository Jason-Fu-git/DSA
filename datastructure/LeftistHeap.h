#pragma once

template <class T>
class Node
{
public:
    T data;
    int npl; // 空节点路径长度
    Node *lc;
    Node *rc;
    Node *parent;
    Node(T _data, int _npl = 1, Node *_lc = nullptr, Node *_rc = nullptr, Node *_parent = nullptr) : data(_data), npl(_npl), lc(_lc), rc(_rc), parent(_parent) {}
};

// 小顶堆
template <class T>
class LeftistHeap
{
private:
    Node<T> *root;                              // 树根
    int size;                                   // 树规模
    inline void swap(Node<T> *&a, Node<T> *&b); // 交换两个节点
    Node<T> *merge(Node<T> *a, Node<T> *b);     // 合并两个小顶堆
public:
    LeftistHeap() : root(nullptr), size(0) {} // 构造函数
    void insert(T data);                      // 插入元素
    void attach(LeftistHeap<T> *x);           // 把堆x合并到该堆上
    T delMin();                               // 删除最小元素
    inline T getMin();                        // 获取最小元素
};
template <class T>
inline void LeftistHeap<T>::swap(Node<T> *&a, Node<T> *&b)
{ // 节点交换函数
    Node<T> *temp = a;
    a = b;
    b = temp;
};

template <class T>
Node<T> *LeftistHeap<T>::merge(Node<T> *a, Node<T> *b)
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

template <class T>
void LeftistHeap<T>::insert(T e)
{
    root = merge(new Node<T>(e), root); // 插入新元素
    ++size;                             // 规模加一
}

template <class T>
void LeftistHeap<T>::attach(LeftistHeap<T> *x)
{
    root = merge(root, x->root); // 合并两个左式堆
    size += x->size;             // 增加规模
    x->root = nullptr;           // 释放另一个左式堆
    x->size = 0;
}

template <class T>
T LeftistHeap<T>::delMin()
{
    Node<T> *lheap = root->lc;
    Node<T> *rheap = root->rc;
    if (lheap)
        lheap->parent = nullptr;
    if (rheap)
        rheap->parent = nullptr;
    T e = root->data;
    delete root;                // 移除根节点
    root = merge(lheap, rheap); // 合并左孩子和右孩子
    --size;                     // 规模减一
    return e;                   // 返回被删除的根节点的值
}

template <class T>
T LeftistHeap<T>::getMin()
{
    return root->data;
}