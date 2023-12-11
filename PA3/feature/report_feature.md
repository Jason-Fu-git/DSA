# CST 3-1-2 Feature Report

付甲申 2022012206



## 1. 题目分析

### 数据结构设计

题目需要对一个数据序列进行多次查询、修改操作，以及topK查询，由于不能确保数据具有较强的局部性，数据结构选为**AVL Tree**。

题目中涉及的查询操作一共有三类：

- 查询index
- 查询按数据权重`w`排序的topK
- 查询按数据权重绝对值`|w|`排序的topK

前者可以简单地用一个数组实现，数组的每个元素为指向结构体`data`的指针：

```c++
struct data
{
    long long y; // 仅在ABS_AVL tree中维护，代表自己和左右子树的y和
    int w;       // 权重
    int x;       // 样本值
};
```

后两者可以使用AVL Tree来实现，仅需更改排序依据即可，节点结构体如下：

```c++
struct node
{
    node *lc;     // 左孩子
    node *rc;     // 右孩子
    node *parent; // 父亲
    int sort_var; // 排序字段
    int index;    // 对应索引
    int height;   // 树高
    int size;     // 规模
};
```

对于按`w`排序的树，排序字段`sort_var = w`；对于按`|w|`排序的树，排序字段`sort_var = |w|`。每个节点通过维护`index`指向`data`数组，不在节点里保存`w`，`x`，`y`等变量，以节省空间。

为统一接口，减少代码量，设计了基类`AVL`，按`w`排序的树便是`AVL`的实例。按`|w|`排序的树是派生类`AVL_abs`的实例，后者仅重载了`update`函数，因为`y`变量只需要在`AVL_abs`树中维护。

```c++
class AVL
{
protected:
    node *_hot = nullptr;
    // BT 部分
    inline virtual void update(node *);    // 更新自身的 height，size
    inline void updateHeight(node *);      // 仅仅更新 height
    void updateHeightAbove(node *); // 仅更新自身和所有父节点的height

    // BST 部分
    node *connect34(node *, node *, node *,
                    node *, node *, node *, node *); // 3 + 4 重构
    node *rotateAt(node *);                          // 旋转
    void remove_BST(int);                            // BST 的remove算法

    // AVL 部分
    inline bool isBalanced(node *);   // 是否平衡（balance factor绝对值小于2）
    inline node *tallerChild(node *); // 返回更高的孩子
public:
    node *root;

    inline node *succ_child(node *);    // 直接后继(孩子)
    inline node *succ_ancestor(node *); // 直接后继(祖先)
    void updateAbove(node *);           // 更新自身和所有父节点的 height, size 和 y

    node *searchK(int);   // 找到 top K 位元素
    node *searchV(int);   // 找到 abs_w 的位置
    node *insert(node *); // 插入给定的node*
    void remove(int);     // 移除
};

class AVL_abs : public AVL
{
protected:
    void update(node *x) override; // 仅重载update，更新自身的 height，size 和 y
};
```

其中大部分接口都改编自《数据结构》讲义，其中进行topK搜索的是`searchK`函数。

### topK搜索设计

考虑到AVL“中序遍历单调增”的属性，对于节点`p`，其左子树的排名都在`p`之后，右子树的排名都在`p`之前（`sort_var`越大，排名越靠前，`k`越小）。`p`的排名可以这样计算：
$$
rank(p) = size(p.rc) + rank(p.rc.rightmost)
$$
其中$rank(p.rc.rightmost)$是`p`最靠右的右孩子的排名。

因而，`searchK`可以这样实现：

```c++
node *AVL::searchK(int k)
{ // 找到第k大的元素（根据输入，一定会命中）
    node *p = root;
    int k_g = 0; // 上次搜索节点的排名
    while (p){
        int k_p = (p->rc) ? p->rc->size + k_g + 1 : k_g + 1; // p的排名
        if (k == k_p) // 命中
        {
            _hot = p->parent;
            return p;
        }
        else if (k < k_p) // k_p 偏大，向右走
            p = p->rc;    // 一定有右子树
        else              // k_p 偏小，向左走
        {
            p = p->lc; // 一定有左子树
            k_g = k_p; // 更新排名上界
        }
    }
    return p;
}
```

其中$k\_g = rank(p.rc.rightmost) - 1$，即上次搜索节点的排名。

topK算法实现后，计算topK的元素之和便十分自然：

```c++
long long calculate(int k)
{
    long long result = 0;
    for (node *p = avl_abs.searchK(k); p; p = avl_abs.succ_ancestor(p))
    {
        // 累加右子树及其自身的y值
        result += data_arr[p->index]->y;
        if (p->lc)
            result -= data_arr[p->lc->index]->y;
    }
    return result;
}
```

只需在查找链上不断回溯，累加自身单节点和右子树的y值即可，可以用`data_arr[p->index]->y - data_arr[p->lc->index]->y`来快捷计算。其中回溯的元素是`p`在中序遍历意义上直接后继的祖先版本（最小的左祖先；事实上，应该称为`p`最靠右的右孩子的直接后继）。





## 2. 解题步骤

### (1) 初始化

在`init`函数中，进行如下操作：

- 把`w`和`x`复制到数组`data_arr`中。
- 向`avl_abs`和`avl_w`两棵树中插入新节点，排序依据分别为`|w|`和`w`。

### （2）执行操作

#### `modify_weight`

- 修改`data_arr[i]`的`w`变量。
- 删除两棵树中的对应节点。
- 向两棵树中插入新节点。

#### `modify_value`

- 在`avl_w`中进行topK搜索，得到节点`p`。
- 在`avl_abs`中进行值搜索，依据为`|p|`，得到节点`q`。
- 更改`p`对应`data`结构体中的`x`变量（也是`q`对应的）
- `avl_w.updateAbove(p);` ` avl_abs.updateAbove(q);`

#### `calculate`

- 在`avl_abs`中进行topK搜索。
- `result+=data_arr[p->index]->y - data_arr[p->lc->index]->y;`
- 当前节点转为`p`最小的左祖先。





## 3. 复杂度分析

### （1）时间复杂度

- 建立AVL : $O(nlogn)$
- topK搜索与值搜索均为$O(logn)$，故三种操作的时间复杂度均为$O(logn)$，操作的总时间复杂度为：$O(mlogn)$

总时间复杂度$O((m+n)logn)$，需要注意，由于涉及大量访存和加法、乘法操作，常数可能比较大。

### （2）空间复杂度

- AVL :  $O(n)$
- 数组：$O(n)$

总空间复杂度：$O(n)$。若以字节为单位，应为$\Omega(104n)$。



## 4. 源代码

```c++
#include "feature.h"

struct data
{
    long long y; // 仅在ABS_AVL tree中维护，代表自己和左右子树的y和
    int w;       // 权重
    int x;       // 样本值

    data(int _w, int _x) : w(_w), x(_x)
    {
        y = (long long)_w * (long long)_x;
    }
} **data_arr;

struct node
{
    node *lc;     // 左孩子
    node *rc;     // 右孩子
    node *parent; // 父亲
    int sort_var; // 排序字段
    int index;    // 对应索引
    int height;   // 树高
    int size;     // 规模

    node(int _index, int _sort_var) : index(_index), lc(nullptr), rc(nullptr), parent(nullptr),
                                      sort_var(_sort_var), height(0), size(1) {}
};

// AVL tree
class AVL
{
protected:
    node *_hot = nullptr;
    // BT 部分
    inline virtual void update(node *);    // 更新自身的 height，size 和 y
    inline void updateHeight(node *);      // 仅仅更新 height
    void updateHeightAbove(node *); // 仅更新自身和所有父节点的height

    // BST 部分
    node *connect34(node *, node *, node *,
                    node *, node *, node *, node *); // 3 + 4 重构
    node *rotateAt(node *);                          // 旋转
    void remove_BST(int);                            // BST 的remove算法

    // AVL 部分
    inline bool isBalanced(node *);   // 是否平衡（balance factor绝对值小于2）
    inline node *tallerChild(node *); // 返回更高的孩子
public:
    node *root;

    inline node *succ_child(node *);    // 直接后继(孩子)
    inline node *succ_ancestor(node *); // 直接后继(祖先)
    void updateAbove(node *);           // 更新自身和所有父节点的 height, size 和 y

    node *searchK(int);   // 找到 top K 位元素
    node *searchV(int);   // 找到 abs_w 的位置
    node *insert(node *); // 插入给定的node*
    void remove(int);     // 移除
};

// 以绝对值为排序依据的AVL tree
class AVL_abs : public AVL
{
protected:
    void update(node *x) override; // 仅重载update
};

AVL_abs avl_abs; // 以w绝对值为排序依据的AVL tree
AVL avl_w;       // 以w为排序依据的AVL tree

// ------------------------------------------
// 下面的接口改编自《数据结构》讲义

// AVL函数定义
// BT 部分
void AVL_abs::update(node *x)
{
    // update height, size and y
    x->height = 0;
    x->size = 1;
    data *d = data_arr[x->index];
    d->y = ((long long)(d->w)) * ((long long)(d->x));
    if (x->lc)
    {
        x->height = x->lc->height + 1;
        x->size += x->lc->size;
        d->y += data_arr[x->lc->index]->y;
    }
    if (x->rc)
    {
        x->height = (x->height > x->rc->height + 1) ? x->height : x->rc->height + 1;
        x->size += x->rc->size;
        d->y += data_arr[x->rc->index]->y;
    }
}

void AVL::update(node *x)
{
    // update height, size only
    x->height = 0;
    x->size = 1;
    if (x->lc)
    {
        x->height = x->lc->height + 1;
        x->size += x->lc->size;
    }
    if (x->rc)
    {
        x->height = (x->height > x->rc->height + 1) ? x->height : x->rc->height + 1;
        x->size += x->rc->size;
    }
}

void AVL::updateHeight(node *x)
{
    // update height only
    x->height = 0;
    if (x->lc)
        x->height = x->lc->height + 1;
    if (x->rc)
        x->height = (x->height > x->rc->height + 1) ? x->height : x->rc->height + 1;
}

void AVL::updateAbove(node *x)
{
    // 一直update，直到树根
    while (x)
    {
        update(x);
        x = x->parent;
    }
}

void AVL::updateHeightAbove(node *x)
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

inline node *AVL::succ_child(node *p)
{
    p = p->rc;
    while (p->lc)
    {
        p = p->lc;
    }

    return p;
}

inline node *AVL::succ_ancestor(node *q)
{
    // 返回最小的左祖先
    while (q->parent && q->parent->rc == q) // 向左上走
        q = q->parent;
    q = q->parent; // 再向右上走一步
    return q;
}

// BST 部分
node *AVL::connect34(node *a, node *b, node *c,
                     node *T0, node *T1, node *T2, node *T3)
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

    update(a);
    update(c);
    update(b);
    // b的parent仅需更新高度
    updateHeightAbove(b->parent);
    return b;
}

node *AVL::rotateAt(node *v)
{
    node *p = v->parent;
    node *g = p->parent;
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

void AVL::remove_BST(int p)
{
    node *x = searchV(p); // 一定可以命中
    _hot = x->parent;
    node *_succ;
    if (!x->lc) // 利用右子树替换
        _succ = x->rc;
    else if (!x->rc)
        _succ = x->lc;
    else
    {                          // 存在两个分支
        _succ = succ_child(x); // 找到最靠左的右孩子

        // 交换排序字段、数组索引
        int tmp = x->sort_var;
        x->sort_var = _succ->sort_var;
        _succ->sort_var = tmp;

        tmp = x->index;
        x->index = _succ->index;
        _succ->index = tmp;

        x = _succ; // 迈向直接后继
        _hot = x->parent;
        _succ = x->rc; // 向右迈一步（必定没有左孩子）
    }

    // replace x with succ
    if (_hot)
    {
        if (_hot->lc == x)
            _hot->lc = _succ;
        else if (_hot->rc == x)
            _hot->rc = _succ;
        if (_succ)
        {
            _succ->parent = _hot;
            x->parent = _succ;
        }
    }
    else
    {
        root = _succ;
    }

    // x此时已经被disattach出去了
    updateAbove(x->parent);
    delete x;
}

// AVL 部分
bool AVL::isBalanced(node *p)
{
    int lh = (p->lc) ? p->lc->height : -1;
    int rh = (p->rc) ? p->rc->height : -1;
    return (lh - rh) <= 1 && (rh - lh) <= 1;
}

node *AVL::tallerChild(node *p)
{
    int lh = (p->lc) ? p->lc->height : -1;
    int rh = (p->rc) ? p->rc->height : -1;
    if (lh > rh)
        return p->lc;
    else if (lh < rh)
        return p->rc;
    else
    { // 与父亲同侧者优先
        if (p == p->parent->lc)
            return p->lc;
        return p->rc;
    }
}

node *AVL::searchV(int v)
// _hot指向"命中节点"的父亲，根据输入，一定不会有命中节点
{
    if (!root || root->sort_var == v)
    {
        _hot = nullptr;
        return root;
    }
    for (_hot = root;;)
    {
        node *p = (v < _hot->sort_var) ? _hot->lc : _hot->rc;
        if (!p || p->sort_var == v)
            return p;
        _hot = p;
    }
}

node *AVL::searchK(int k)
{ // 找到第k大的元素（根据输入，一定会命中）
    node *p = root;
    int k_g = 0; // 上次搜索节点的排名
    while (p)
    {

        int k_p = (p->rc) ? p->rc->size + k_g + 1 : k_g + 1; // p的排名
        if (k == k_p) // 命中
        {
            _hot = p->parent;
            return p;
        }
        else if (k < k_p) // k_p 偏大，向右走
            p = p->rc;    // 一定有右子树
        else              // k_p 偏小，向左走
        {
            p = p->lc; // 一定有左子树
            k_g = k_p; // 更新排名上界
        }
    }
    return p;
}

node *AVL::insert(node *q)
{
    searchV(q->sort_var); // 一定找不到

    // q作为_hot的子节点
    if (!_hot)
    { // 空树
        root = q;
    }
    else
    { // _hot至多有一个孩子
        if (q->sort_var < _hot->sort_var)
            _hot->lc = q;
        else
            _hot->rc = q;
        q->parent = _hot;
    }

    // 重平衡
    for (node *g = _hot; g; g = g->parent)
    {
        if (!isBalanced(g))
        {                  // 祖先节点失衡
            if (root == g) // 根节点失衡
                root = rotateAt(tallerChild(tallerChild(g)));
            else
            {
                if (g->parent->lc == g)
                    g->parent->lc = rotateAt(tallerChild(tallerChild(g)));
                else
                    g->parent->rc = rotateAt(tallerChild(tallerChild(g)));
            }
            updateAbove(g); // 调用一次更新
            break;          // 全树必定复衡
        }
        else
        { // 祖先节点未必平衡，仅更新
            update(g);
        }
    }

    return q;
}

void AVL::remove(int p) // O(logn) 复杂度较高，可能一次调整多个节点
{
    remove_BST(p);
    // 重平衡
    for (node *g = _hot; g; g = g->parent)
    {
        if (!isBalanced(g))
        {                  // 祖先节点失衡
            if (root == g) // 根节点失衡
                g = root = rotateAt(tallerChild(tallerChild(g)));
            else
            {
                if (g->parent->lc == g)
                    g = g->parent->lc = rotateAt(tallerChild(tallerChild(g)));
                else
                    g = g->parent->rc = rotateAt(tallerChild(tallerChild(g)));
            }
        }
        update(g);
    }
}
// -------------------------------------------

// feature.h 接口实现：
void init(int n, const int *w, const int *x)
{
    data_arr = new data *[n];
    for (int i = 0; i < n; ++i)
    {
        // insert into array
        int abs_w = (w[i + 1] > 0) ? w[i + 1] : -w[i + 1];
        data *d = new data(w[i + 1], x[i + 1]);
        data_arr[i] = d;

        // insert into abs_tree
        node *p = new node(i, abs_w);
        avl_abs.insert(p);

        // insert into w_tree
        node *q = new node(i, w[i + 1]);
        avl_w.insert(q);
    }
}

void modify_weight(int i, int w)
{

    --i;
    data *d = data_arr[i];

    // update array
    int old_w = d->w;
    d->w = w;
    d->y = (long long)w * (long long)(d->x);

    // remove the corresponding node
    avl_abs.remove((old_w > 0) ? old_w : -old_w);
    avl_w.remove(old_w);

    // create
    node *p = new node(i, (w > 0) ? w : -w);
    node *q = new node(i, w);

    // insert
    avl_abs.insert(p);
    avl_w.insert(q);
}

void modify_value(int k, int x)
{
    node *p = avl_w.searchK(k); // 一定命中
    data_arr[p->index]->x = x;
    // search in avl_abs
    int w = data_arr[p->index]->w;
    node *q = avl_abs.searchV((w > 0) ? w : -w);

    // all modify
    avl_w.updateAbove(p);
    avl_abs.updateAbove(q);
}

long long calculate(int k)
{
    long long result = 0;
    for (node *p = avl_abs.searchK(k); p; p = avl_abs.succ_ancestor(p))
    {
        // 累加右子树及其自身的y值
        result += data_arr[p->index]->y;
        if (p->lc)
            result -= data_arr[p->lc->index]->y;
    }
    return result;
}
```

