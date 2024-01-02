# CST LAB3 BBST Report

付甲申 2022012206



## 数据结构实现过程

本次实验中实现了`AVL`树和`Splay`树，实现思路与《数据结构》讲义基本一致。

### `Node` 类

`AVL`树和`Splay`树的节点都基于模板类`Node`：

```c++
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
```

为去除冗余性，节点类中仅维护了树高`height`，储存的数据`data`和`lc`，`rc`，`parent`三个指针。

### `BST`类

`AVL`和`Splay`均派生自`BST`类：

```c++
template <class T>
using nodep = Node<T> *;
template <class T>
class BST
{
protected:
    nodep<T> _hot;                    // 命中节点的父节点
    nodep<T> root;                    // 树根
    nodep<T> succ_child(nodep<T>); // 中序遍历意义上的直接后继（孩子）
    nodep<T> pred(nodep<T>);       // 中序遍历意义上的直接前驱
    
    nodep<T> connect34(nodep<T>, nodep<T>, nodep<T>,
                       nodep<T>, nodep<T>, nodep<T>, nodep<T>);
    nodep<T> rotateAt(nodep<T>);
    void updateHeight(nodep<T>);
    void updateHeightAbove(nodep<T>);

public:
    BST() : root(nullptr), _hot(nullptr){}
    virtual nodep<T> searchLe(const T &); // 找到小于等于该元素的最大元素
    virtual nodep<T> &search(const T &);  // 普通搜索
    virtual nodep<T> insert(const T &);   // 插入
    virtual bool remove(const T &);       // 删除
};
```

`search`，`insert`等接口与讲义的实现思路基本一致。为提高效率，中序遍历的直接后继简化为`succ_child`，只返回最靠左的右孩子。

其中`searchLe`接口是为本题新增的接口，返回小于等于`e`的最大节点，实现如下：

```c++
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
    {   if (e < _hot->data)    // e在_hot和它的直接前驱中间
            return pred(_hot); // 返回直接前驱
        else                   // 在_hot和它的直接后继中间
            return _hot;       // 返回_hot
    }
}
```

欲验证其正确性，可分为以下三种情况

1. `e`小于所有关键码：此时该树可能为空树，直接返回空指针；若不为空树，`search`不会命中，`_hot`必为全树最靠左的节点，`pred`返回空指针。
2. 没有等于`e`的关键码，但存在小于`e`的关键码：`search`不会命中，考虑`_hot->data`，若大于`e`，说明`e`夹在`_hot`和`pred(_hot)`中间，返回`pred(_hot)`；若小于`e`，说明`e`夹在`_hot`和`succ(_hot)`中间，返回`_hot`。
3. 存在等于`e`的关键码：直接返回搜索结果。



### `AVL`类

```c++
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
```

派生自`BST`类，其中`insert`函数与`remove`函数的实现与讲义一致，不再赘述。`isBalanced`和`tallerChild`是讲义对应宏的函数版本。



### `Splay`类

```c++
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
```

派生自`BST`类，其中`splay`，`search`，`insert`，`remove`函数的实现与讲义一致，不再赘述。考虑到每次查找后相应节点需伸展到树根，需重写`searchLe`函数：

```c++
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
```

由于`search`返回的节点必定是树根，未命中时，树根是`_hot`，故`e`大于等于`x->data`的情况可以直接返回树根。小于`x->data`情况的处理同上，返回其直接前驱。



## 数据结构复杂度分析

### `AVL`

树高：`AVL`的渐进平衡性保证其树高为$O(logn)$

- `search` ：搜索链长度为树高，$O(logn)$
- `insert`：搜索+最多一次重平衡，$O(logn)$
- `remove`：搜索+最多$\Omega(logn)$次重平衡，$O(logn)$
- `searchLe`：搜索+最多一次长度为$O(logn)$的回溯，$O(logn)$

### `Splay`

利用势能函数分析，其增、删、查操作分摊复杂度仍为$O(logn)$。

但考虑到每次访问都要将目标节点伸展到树根，伸展过程为$O(logn)$。故总体而言，其复杂度要高于`AVL`，尤其是在数据较为随机的情况下。

但在数据具有较高局部性的情况下，`Splay`可在较小的局部范围内找到对应节点，若数据的局部性$k$满足$k <<n$，访问过程可以达到$O(logk)$，此时性能优于`AVL`。



## 测例设计

### 1. 随机数据

模拟实际情况下对输入是否具有局部性把握不大的情况，对效率的稳定性要求较高。

对于插入和搜索操作，在$[0, MAX\_DATA]$范围内生成随机数。对于删除操作，在该时刻树中的元素中随机挑选。

### 2. 局部性数据

模拟大多数实际情况下数据具有局部性。

对于插入和搜索操作，分多个批次进行，每一批次在$[randbase, randbase+randrange]$范围内生成$randrange*10$个数据，$randrange$一般设计得比较小，在程序中取$1000$。生成的数据可以保证良好的空间局部性（每次插入的数据和上次的大小差别比较小），以及时间局部性（每个批次的数据有可能重复，即刚访问过的数据有可能很快就被访问）。

对于删除操作，每一批次在该时刻树中的元素中（已排序）选择较小范围的元素进行移除。

### 3. 单调数据

模拟对地址空间等的顺序访问。

对于插入操作，生成范围为$[0,n)$的单调序列。

对于删除操作，按大小顺序从头开始逐个删除$n$个元素。

对于查找操作，查找范围为$[0,n)$的单调序列。



## 测例生成

### 1. 随机数据

按 “插入 1,000,000 个元素 - 删除 10,000 个元素 - 查找 1,000,000 个元素” 的顺序生成，重复5组。共 10,050,000 次操作。

插入和搜索操作直接生成随机数。

对于删除操作，在数据生成器中维护向量`v`，模拟每次操作后树的状态。每次删除操作在`v`的`index`大小范围内生成随机数，挑选对应索引的元素执行删除。

### 2. 局部性数据

按 “插入 100,000 个元素 - 删除 10,000 个元素 - 查找 10,000,000 个元素” 的顺序生成，共 10,110,000 次操作。

对于插入和查找操作，每一批次的大小设置为 10,000 ， 生成每一批次的数据时，首先按照`randbase = rand() % (MAX_DATA / RAND_MAX) * RAND_MAX; `的规则生成`randbase`（实验环境下，`RAND_MAX = 0x7fff`）。然后按照`randbase + rand() % generate_range`的规则生成 10,000 个数据，实验中`generate_range`定为1000。

对于删除操作，先对向量`v`进行排序，然后抽取其中较小范围内的数进行删除。

### 3. 单调数据

按 “插入 10,000,000 个元素 - 删除 100,000 个元素 - 查找 10,000,000 个元素” 的顺序生成，共 20,100,000 次操作。

对于插入和查找操作，直接生成范围为[0, 10,000,000)，公差为1的等差数列。

对于删除操作，直接生成范围为[0, 100,000)，公差为1的等差数列。



## 测试结果

| 数据结构 | 随机数据 | 局部性数据 | 单调数据 |
| -------- | -------- | ---------- | -------- |
| `AVL`    | 19.6 s   | 8.0 s      | 10.2 s   |
| `Splay`  | 29.8 s   | 7.5 s      | 9.9 s    |



## 性能分析

### 1. 随机数据

在数据随机的情况下，`AVL`的性能显著优于`Splay`。在数据随机的情况下，不能保证刚刚被访问的元素很快就能被访问到。`AVL`和`Splay`调用的`BST<T>::search()`接口部分用时基本相近，在此基础上，`Splay`每次伸展操作多花费$O(logn)$的时间，故总体较慢。



### 2. 局部性数据

在数据具有局部性的情况下，`Splay`的性能优于`AVL`。这是由于`Splay`在搜索的时候可以把搜索范围降到$O(logk)$。在命中的情况下，复杂度可以得到有效降低，但如果无法命中，仍需要进行$O(logn)$的搜索。

在实验中生成的局部性数据，查找过程有可能不命中，故`Splay`性能的提升不是十分明显，但性能依然优于`AVL`。



### 3. 单调数据

在数据具有单调性的情况下，`AVL`的性能与`Splay`相近。

对于`AVL`，建树时间为$O(nlogn)$。对于查找，虽然整棵树都被搜索了一遍，但此时的`AVL`接近一个满树，最后一层约有`n/2`个元素，最后一层查找复杂度$\Omega(\frac{n}{2}logn)$，故总体复杂度仍然为$O(nlogn)$。

对于`Splay`，插入n个元素后，其结构类似一个链表，每次插入操作在树根，仅需花费$\Omega(1)$的时间，故建树时间为$\Omega(n)$，优于`AVL`。但对于查找操作，第一次需要搜索`Splay`最小的元素，需要花费$\Omega(n)$的时间，复杂度较高。不过之后每次查找后查找链的长度都会减半，只需$log_2n$次查找，查找链的长度就可以降到常数级，此过程花费的时间为$\Omega(4n)$。（查找与伸展各花费$\Omega(2n)$）之后的查找用时为$\Omega(kn)$。故`Splay`的总体用时为$O(n)$。在实验中生成数据的规模下，`Splay`的常数与$logn$相当，故`Splay`与`AVL`用时相近。在数据规模更大的情况下，`Splay`的优势会更明显。

