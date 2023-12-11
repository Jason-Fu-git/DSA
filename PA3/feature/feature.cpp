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