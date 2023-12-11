# CST 3-2-2 Kidd Report

付甲申 2022012206



## 1. 题目分析

题目涉及两部分操作：对某一区间进行一致的操作；查询某一区间操作的次数之和。考虑到操作和查找都是按照区间进行的，故数据结构采用 **Segment Tree**。

若线段的插入操作简单地按照讲义进行，在进行查找时，由于无法确定线段所处的节点，需要对区间范围内的所有节点进行搜索，复杂度极大。

因此，涉及节点时，采用**类似懒惰标记**的方法，节点结构体的定义如下：

```c++
struct node
{
    node *lc = nullptr;                   // left child
    node *rc = nullptr;                   // right child
    unsigned long long contain_count = 0; // 该区间包含的翻转纸牌个数
    unsigned int full_count = 0;          // 撑满整个区间的次数 （纸牌个数需乘上区间长度）
}
```

- `full_count`变量记录整个区间的翻转个数，只有当整个区间的所有扑克牌在一次操作中被全部翻转时，该变量才会加一。
- `contain_count`变量类似懒惰标记，递归到该区间$I$时，若扑克牌所在区间$S$与$I$有交集，会把区间$S\cap I$的长度追加到`contain_count`里。但追加后仍将继续进行递归操作。故该变量不是真正的懒惰标记。

`contain_count`变量的设计是为了提高查找效率。例如，假设线段树的某个区间$I$被包含于查找区间$S$中，若不设置此变量，查找过程将递归$I$ 的所有孩子；但设置此变量后，仅需把`contain_count`追加到计数器中即可，不用继续遍历。这样，查找操作的时间复杂度也可以降到$O(logn)$。



## 2. 解题步骤

### （1）建立Segment Tree

逐个读取线段$S$，记当前Segment Tree递归到的区间为$I$，进行如下操作：

- 若$I \subseteq S$，`full_count++`, `contain_count+=len(I)`，返回。
- 若$I$不是$S$的子集，对$I$进行二分，左孩子为$I_l$，右孩子为$I_r$。若$I_l \cap S \neq \empty$，`contain_count+=len(intersection(Il,S))`，递归$I_l$；若$I_r \cap S \neq \empty$，`contain_count+=len(intersection(Ir,S))`，递归$I_r$。

### （2）查找

对于查找区间$S$，记当前Segment Tree递归到的区间为$I$，累加器为`count`，进行如下操作：

- 若$I \subseteq S$，`count+=contain_count`，返回。
- 若$I$不是$S$的子集，对$I$进行二分，左孩子为$I_l$，右孩子为$I_r$。若$I_l \cap S \neq \empty$，`count+=len(intersection(Il,S))*full_count`，递归$I_l$；若$I_r \cap S \neq \empty$，`count+=len(intersection(Ir,S))*full_count`，递归$I_r$。



## 3. 复杂度分析

记扑克牌的数量（即区间范围）为`n`，插入次数为`i`，查询次数为`q`。

### （1）时间复杂度

事实上，插入区间与查找区间的操作逻辑与讲义基本一致，考虑到每一层最多涉及4个节点，树高为$O(logn)$。故每次插入/查找的时间复杂度为$O(logn)$，总的时间复杂度为$O((i+q)logn)$。

### （2） 空间复杂度

插入每个线段时，每一层最多申请4个节点，故空间复杂度为$O(ilogn)$。





## 4. 源代码

```c++
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
```

