#include <cstdio>

// 采用多叉树的二叉树表示法
struct Node
{
    Node *lc = nullptr;     // 左孩子是firstChild
    Node *rc = nullptr;     // 右孩子是nextSibling
    Node *parent = nullptr; // 此处设置parent是为了方便移除操作
    int size = 1;           // 子树规模
    int height = 0;         // 树高
} *head;                    // 树根

// 邻接表的每一行
struct Line
{
    int n;           // 每一行的节点数
    int height = -1; // 这一行的最大子树高
    int size = 0;    // 这一行的总子树规模
    int *data;       // 节点数组
};

int N, M;
Line *ju; // 邻接表

Node **node_stack;    // node*栈
int p_node_stack = 0; // 栈顶

// 建立树，并初始化每个节点的树高和子树规模
void construct()
{
    head = new Node;

    // 维护一个栈，在过程中使用，指向上次“展开”的元素index，相当于一个call stack
    int *call_stack = new int[N];
    int p_call_stack = 0;

    // 建树
    if (ju[0].n != 0)
    {
        call_stack[p_call_stack++] = 0;    // 第0行进调用栈
        node_stack[p_node_stack++] = head; // 首节点进节点栈

        Node *p = new Node;
        p->parent = node_stack[p_node_stack - 1]; // 更新父亲
        node_stack[p_node_stack - 1]->lc = p;     // 第一个孩子进树
        node_stack[p_node_stack++] = p;           // 第一个孩子进栈
        // 第一个孩子的弟弟进树并进栈
        for (int i = 1; i < ju[0].n; ++i)
        {
            Node *p = new Node;
            p->parent = node_stack[p_node_stack - 1]; // 更新父亲
            node_stack[p_node_stack - 1]->rc = p;     // 进树
            node_stack[p_node_stack++] = p;           // 进栈
        }
    }

    // 若调用栈不为空
    while (p_call_stack > 0)
    {
        if (ju[call_stack[p_call_stack - 1]].n == 0)
        {
            // 这一行遍历结束
            node_stack[p_node_stack - 1]->height = ju[call_stack[p_call_stack - 1]].height + 1; // 最大子树高度加一
            node_stack[p_node_stack - 1]->size = ju[call_stack[p_call_stack - 1]].size + 1;     // 子树总规模加一
            if (p_call_stack > 1)
            { // 更新上一行
                ju[call_stack[p_call_stack - 2]].size += node_stack[p_node_stack - 1]->size;
                ju[call_stack[p_call_stack - 2]].height =
                    (ju[call_stack[p_call_stack - 2]].height < node_stack[p_node_stack - 1]->height) ? node_stack[p_node_stack - 1]->height : ju[call_stack[p_call_stack - 2]].height;
            }
            --p_node_stack;
            --p_call_stack; // 弹出
        }
        else
        {
            // 栈中最后一个元素添加孩子
            int index = (--ju[call_stack[p_call_stack - 1]].n);
            index = ju[call_stack[p_call_stack - 1]].data[index] - 1; // 获取对应行的index
            call_stack[p_call_stack++] = index;                       // index 进调用栈

            // 该元素不是叶节点
            if (ju[call_stack[p_call_stack - 1]].n != 0)
            {
                Node *p = new Node;
                p->parent = node_stack[p_node_stack - 1]; // 更新父亲
                node_stack[p_node_stack - 1]->lc = p;     // 第一个孩子进树
                node_stack[p_node_stack++] = p;           // 第一个孩子进栈
                // 第一个孩子的弟弟进树并进栈
                for (int i = 1; i < ju[index].n; ++i)
                {
                    Node *p = new Node;
                    p->parent = node_stack[p_node_stack - 1]; // 更新父亲
                    node_stack[p_node_stack - 1]->rc = p;     // 进树
                    node_stack[p_node_stack++] = p;           // 进栈
                }
            }
        }
    }

    // 保险起见，手动清空node栈
    p_node_stack = 0;

    // 释放局部栈的内存
    delete[] call_stack;
}

// 输入一行，返回对应的Node
Node *getNode(bool push_stack)
{
    bool is_valid = true; // 路径是否合法
    Node *p = head;       // 从head开始搜

    if (push_stack)
        node_stack[p_node_stack++] = p; // head节点入栈

    int n;
    scanf("%d", &n);
    for (int i = 0; i < n; ++i)
    {
        Node *q = p;
        if (q->lc)
            // q有孩子
            q = q->lc;
        else
            // 长度过长
            is_valid = false;

        // 读取下一个数字
        int x;
        scanf("%d", &x);

        if (is_valid)
        { // 若路径合法

            for (int j = 0; j < x; ++j)
            {
                if (q->rc)
                    // q有兄弟
                    q = q->rc;
                else
                    // q没有兄弟，索引过大
                    is_valid = false;
            }
        }

        if (is_valid)
        { // 只有在索引合适时更新
            p = q;
            if (push_stack)
                node_stack[p_node_stack++] = p; // p 入栈
        }
    }
    return p;
}

// 在插入操作中维护数高和子树规模
void update_h_and_size(Node *p, bool is_insert)
{

    int prev_height = p->height;
    int decreased_height = prev_height + 1; // 子树降低的高度
    if (!is_insert)
    {
        prev_height = -1;   // 除去的子树高度为-1（一个节点也没有）
    }
    int size = p->size;

    while (p_node_stack > 0)
    {
        Node *q = node_stack[--p_node_stack];
        if (is_insert)
        {                                    // 插入模式
            q->size += size;                 // 子树规模直接加
            if (q->height < prev_height + 1) // 如果新添加的是最大子树，就更新树高
                q->height = prev_height + 1;
        }
        else
        { // 移除模式
            q->size -= size;
            if (decreased_height && q->height == prev_height + decreased_height + 1)
            { // 最大子树高度降低，需要重新遍历一遍
                // 此处较耗时
                int original_height = q->height;
                prev_height = -1;
                Node *tmp = q;
                if (tmp->lc)
                {                              // tmp有孩子
                    tmp = tmp->lc;             // 移动到孩子
                    prev_height = tmp->height; // 更新prev_height
                    while (tmp->rc)
                    {                  // tmp 还有兄弟
                        tmp = tmp->rc; // 移动到下个兄弟
                        prev_height = (tmp->height > prev_height) ? tmp->height : prev_height;
                    }
                }
                q->height = prev_height + 1;
                decreased_height = original_height - q->height;
            }
        }
        prev_height = q->height;
    }
}

// 移除节点
void remove(Node *p)
{
    if (p->parent)
    { // 有父亲
        // 兄弟移动
        if (p->parent->lc == p)
        {
            p->parent->lc = p->rc;
        }
        else
        {
            p->parent->rc = p->rc;
        }
        if (p->rc) // 有兄弟
        {
            p->rc->parent = p->parent; // 更新兄弟的父亲
        }
    }
    p->parent = nullptr; // 没有父亲
    p->rc = nullptr;     // 没有兄弟
}

// 根据输入索引插入节点
void insert(Node *p, Node *parent)
{
    int index;
    scanf("%d", &index);
    if (index == 0)
    {                          // 作为长子插入
        p->parent = parent;    // 更新p的父亲
        p->rc = parent->lc;    // 更新p的兄弟
        if (p->rc)             // 有兄弟
            p->rc->parent = p; // 更新p的兄弟的父亲
        parent->lc = p;        // 更新p父亲的长子
    }
    else
    {
        Node *q = parent->lc;
        while (--index)
        {
            q = q->rc;
        }
        p->parent = q;         // 更新p的父亲
        p->rc = q->rc;         // 更新p的兄弟
        if (p->rc)             // 有兄弟
            p->rc->parent = p; // 更新p的兄弟的父亲
        q->rc = p;             // 更新p父亲的兄弟
    }
}

int main()
{
#ifndef _OJ_
    freopen("a.in", "r", stdin);
    freopen("a.out", "w", stdout);
#endif
    // 输入
    scanf("%d %d", &N, &M);
    ju = new Line[N];
    node_stack = new Node *[N];
    for (int i = 0; i < N; ++i)
    {
        int n;
        scanf("%d", &n);
        ju[i].n = n;
        ju[i].data = new int[n];
        for (int j = 0; j < n; ++j)
        {
            scanf("%d", &ju[i].data[j]);
        }
    }

    // 建立树并初始化高度和子树规模
    construct();

    // 执行操作
    for (int i = 0; i < M; ++i)
    {
        int x;
        scanf("%d", &x);
        if (x == 0)
        {                                  // 移动操作
            Node *src = getNode(true);     // 获取待移动的node, 需要压栈
            --p_node_stack;                // 弹出栈顶，即src
            remove(src);                   // 从树中移除
            update_h_and_size(src, false); // 维护树

            Node *to = getNode(true); // 待插入
            insert(src, to);
            update_h_and_size(src, true); // 维护树
        }
        else
        {                             // 查询操作
            Node *p = getNode(false); // 无需压栈
            if (x == 1)
                printf("%d\n", p->height);
            else
                printf("%d\n", p->size);
        }
    }

    // 释放内存
    delete[] ju;
    delete[] node_stack;
}