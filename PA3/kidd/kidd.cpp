#include <cstdio>

struct node
{
    node *lc = nullptr;                   // left child
    node *rc = nullptr;                   // right child
    unsigned long long contain_count = 0; // 该区间包含的翻转纸牌个数
    unsigned int full_count = 0;          // 撑满整个区间的次数 （纸牌个数需乘上区间长度）
} *root;

/*
区分full_count 与 contain_count 的例子:

区间为[1,10], 插入两个线段[1, 10], [1, 5]
则 full_count = 1, contain_count = 15
*/

/**
 * 建树
 * root: 父节点指针
 * il:线段的左端(闭)
 * ir:线段的右端(闭)
 * sl:区间的左端(闭)
 * sr:区间的右端(闭)
 */
void construct(node *root, unsigned int il, unsigned int ir, unsigned int sl, unsigned int sr)
{
    if ((il <= sl && sr <= ir) || sl == sr)
    {                                       // [sl, sr] in [il, ir] or len(s)==1
        ++(root->full_count);               // 完全包含的线段加一
        root->contain_count += sr - sl + 1; // 增加为区间长度
        return;                             // 不继续递归孩子
    }

    unsigned int sm = (sl + sr) >> 1; // 区间的mid
    // 左孩子: [sl,sm]
    // 右孩子: [sm+1,sr]
    if (il <= sm)
    {
        // 线段与左孩子有交集
        unsigned int lb = (il < sl) ? sl : il;
        unsigned int rb = (ir > sm) ? sm : ir;
        root->contain_count += rb - lb + 1; // 取交集的长度
        if (!root->lc)
            root->lc = new node;             // 动态开点
        construct(root->lc, il, ir, sl, sm); // 递归左孩子
    }
    if (sm < ir)
    {
        // 线段与右孩子有交集
        unsigned int lb = (il < sm + 1) ? sm + 1 : il;
        unsigned int rb = (ir > sr) ? sr : ir;
        root->contain_count += rb - lb + 1; // 取交集的长度
        if (!root->rc)
            root->rc = new node;                 // 动态开点
        construct(root->rc, il, ir, sm + 1, sr); // 递归右孩子
    }
}

/**
 * 查找
 * root: 父节点指针
 * il:查找区间的左端(闭)
 * ir:查找区间的右端(闭)
 * sl:区间的左端(闭)
 * sr:区间的右端(闭)
 */
unsigned long long search(node *root, unsigned int il, unsigned int ir, unsigned int sl, unsigned int sr)
{
    unsigned long long contain_count = 0;

    if ((il <= sl && sr <= ir) || sl == sr)
    { // 查找区间包含该区间
        contain_count += root->contain_count;
        return contain_count; // 不继续递归孩子
    }

    unsigned int sm = (sl + sr) >> 1; // 区间的mid
    // 左孩子: [sl,sm]
    // 右孩子: [sm+1,sr]
    if (il <= sm)
    {
        // 查找区间与左孩子有交集
        unsigned int lb = (il < sl) ? sl : il;
        unsigned int rb = (ir > sm) ? sm : ir;
        contain_count += (rb - lb + 1) * (unsigned long long)root->full_count;     // 取交集的长度乘以张满的个数
        if (root->lc)                                          // 左孩子若存在
            contain_count += search(root->lc, il, ir, sl, sm); // 递归左孩子
    }
    if (sm < ir)
    {
        // 查找区间与右孩子有交集
        unsigned int lb = (il < sm + 1) ? sm + 1 : il;
        unsigned int rb = (ir > sr) ? sr : ir;
        contain_count += (rb - lb + 1) * (unsigned long long)root->full_count; // 取交集的长度乘以张满的个数
        if (root->rc)                                              // 左孩子若存在
            contain_count += search(root->rc, il, ir, sm + 1, sr); // 递归左孩子
    }

    return contain_count;
}

int main()
{
    unsigned int n, m;
    scanf("%u %u", &n, &m);
    root = new node; // 初始化root
    for (unsigned int i = 0; i < m; ++i)
    {
        char c;
        unsigned int l, r;
        scanf("%c", &c); // 把回车读了
        scanf("%c %u %u", &c, &l, &r);
        if (c == 'H')
        {
            construct(root, l, r, 1, n);
        }
        else if (c == 'Q')
        {
            printf("%llu\n", search(root, l, r, 1, n));
        }
    }
}