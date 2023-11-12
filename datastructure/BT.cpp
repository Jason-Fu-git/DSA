// binary tree

#include <iostream>
using namespace std;

#define STACK_SIZE 1000

struct node
{
    node *parent = nullptr;
    node *lc = nullptr;
    node *rc = nullptr;
    /*
    data
    */
    int data;
};

// stack
node *stack[STACK_SIZE];
int stack_top = -1;

// queue
struct Q
{
    Q *next = nullptr;
    node *data = nullptr;
} *head, *tail;

void enqueue(node *p)
{
    tail->data = p;
    Q *q = new Q;
    tail->next = q;
    tail = q;
}

node *dequeue()
{
    Q *q = head;
    head = head->next;
    return q->data;
}

// visit
void visit(node *p)
{
    // do something
    cout << p->data << " ";
}

// preorder
void preorder(node *root)
{
    // 先序遍历 data -> l -> r
    node *p = root;
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

// succ 直接后继
node *succ(node *p)
{
    node *q = p;
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

// pred 直接前驱
node *pred(node *p)
{
    node *q = p;
    if (q->lc) // 最靠右的左孩子 （对应最低的左祖先）
    {
        q = q->lc;
        while (q->rc)
        {
            q = q->rc;
        }
    }
    else
    { // 最低的右祖先 （对应最靠左的右孩子）
        while (q->parent && q->parent->lc == q)
        {
            q = q->parent;
        }
        q = q->parent; // 往左上走一步
    }
    return q;
}

// inorder
// using stack
void inorder(node *root)
{
    node *p = root;
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

// inorder_succ
void inorder_succ(node *root)
{
    node *x = root;
    while (1)
    {
        if (x->lc) // 探到最左边的孩子
        {
            x = x->lc;
        }
        else
        {
            visit(x); // 没有左孩子，访问自己
            while (!(x->rc))
            { // 没有右孩子
                if (!(x = succ(x)))
                    return; // 直接后继为nullptr，直接return
                else
                    visit(x); // 访问直接后继，重复上探
            }
            x = x->rc; // 有右孩子，则转向
        }
    }
}

// postorder
void postorder(node *root)
{
    if (root)
        stack[++stack_top] = root;
    node *x = root;
    while (stack_top != -1) // 栈不为空
    {
        if (stack[stack_top] != x->parent)     // 不是x的父亲，必定是x的右兄弟（因为左孩子后于右孩子进栈）
        {                                      // 更新栈，直至找到最靠左的孩子
            while (node *y = stack[stack_top]) // 栈顶不为nullptr
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

// level order
void levelorder(node *root)
{
    // 需要用到队列
    head = new Q;
    tail = new Q;
    head->next = tail;
    if (root)
    {
        head->data = root;
    }
    while (head->data)
    {
        node *x = dequeue();
        visit(x);
        if (x->lc)
            enqueue(x->lc);
        if (x->rc)
            enqueue(x->rc);
    }
}

// reconstruct from preorder and inorder
node *reconstruct_from_pre_and_in(int *pre, int *in, int n)
{
    if (n == 0)
        return nullptr;
    node *x = new node;
    x->data = pre[0]; // 从pre的第一个节点开始搜索

    // search
    for (int i = 0; i < n; i++)
    {
        if (in[i] == pre[0])
        { // found it!
            x->lc = reconstruct_from_pre_and_in(pre + 1, in, i);
            x->rc = reconstruct_from_pre_and_in(pre + i + 1, in + i + 1, n - i - 1);
            break;
        }
    }

    return x;
}

// reconstruct from postorder and inorder
node *reconstruct_from_post_and_in(int *post, int *in, int n)
{
    if (n == 0)
        return nullptr;
    node *x = new node;
    x->data = post[n - 1];

    // search
    for (int i = 0; i < n; i++)
    {
        if (in[i] == post[n - 1])
        {
            x->lc = reconstruct_from_post_and_in(post, in, i);
            x->rc = reconstruct_from_post_and_in(post + i, in + i + 1, n - i - 1);
            break;
        }
    }
    return x;
}

int main()
{
    // construct a tree
    node *root = new node;
    node *x2 = new node;
    node *x3 = new node;
    node *x4 = new node;
    node *x5 = new node;
    node *x6 = new node;
    node *x7 = new node;
    node *x8 = new node;
    node *x9 = new node;
    root->data = 1;
    x2->data = 2;
    x3->data = 3;
    x4->data = 4;
    x5->data = 5;
    x6->data = 6;
    x7->data = 7;
    x8->data = 8;
    x9->data = 9;

    root->lc = x2;
    x2->parent = root;
    root->rc = x3;
    x3->parent = root;
    x2->lc = x4;
    x4->parent = x2;
    x2->rc = x5;
    x5->parent = x2;
    x3->lc = x6;
    x6->parent = x3;
    x3->rc = x7;
    x7->parent = x3;
    x4->lc = x8;
    x8->parent = x4;
    x6->rc = x9;
    x9->parent = x6;

    cout << "preorder: ";
    preorder(root);
    cout << endl;

    cout << "inorder: ";
    inorder(root);
    cout << endl;

    cout << "inorderV2: ";
    inorder_succ(root);
    cout << endl;

    cout << "postorder: ";
    postorder(root);
    cout << endl;

    cout << "levelorder: ";
    levelorder(root);
    cout << endl;

    int pre[] = {1, 2, 4, 8, 5, 3, 6, 9, 7};
    int in[] = {8, 4, 2, 5, 1, 6, 9, 3, 7};
    int post[] = {8, 4, 5, 2, 9, 6, 7, 3, 1};

    node *recon1 = reconstruct_from_pre_and_in(pre, in, 9);
    cout << "reconstruct from pre and in in in order: ";
    inorder(recon1);
    cout << endl;

    node *recon2 = reconstruct_from_post_and_in(post, in, 9);
    cout << "reconstruct from post and in in in order: ";
    inorder(recon2);
    cout << endl;
}