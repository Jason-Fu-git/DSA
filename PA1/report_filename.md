# CST 1-3 filename Report

付甲申 2022012206



## 解题思路

### 1. 题目分析

典型动态规划问题，递推公式如下：
$$
dp[i][j] = \begin{cases}
dp[i-1][j-1] &, a[i-1]=b[j-1]\\
min(dp[i-1][j]+1, dp[i][j-1]+1) &, a[i-1]\neq b[j-1]

\end{cases}
$$
式中`dp[i][j]`为字符串`a[0:i]`与`b[0:j]`的最小修改次数。

上述递推公式可以这样解释：

- $a[i-1]=b[j-1]$时，增加$a[i-1]$和$b[j-1]$不改变最小修改次数，故$dp[i][j]=dp[i-1][j-1]$。
- $a[i-1]\neq b[j-1]$时，需要进行一次删除或插入操作，取其中最小值，故$dp[i][j]=min(dp[i-1][j]+1, dp[i][j-1]+1)$。

### 2. 算法优化

#### 2.1. 优化空间复杂度

上述算法的空间复杂度为 $O(NM)$, 显然过大。

因为$dp[i][j]$的计算仅涉及第$i$行和第$i-1$行，故可以仅使用两个数组`prev` (对应`dp[i-1]`) 和`next ` (对应`dp[i]`) 解决问题，空间复杂度为$O(2M)$, 满足要求。

#### 2.2. 优化时间复杂度

经典动态规划算法的时间复杂度为$O(NM)$, 经计算，N与M取最大时需要100s以上，超出时间上限。

题目数据的特点是N,M可以极大，但K极小。当两个字符串的长度差大于K时，至少需进行K+1次删除操作，一定不满足要求，故只需计算$[max(i-K, 1), min(i+K, lenb)]$区间即可。时间复杂度优化为$O(KN)$，满足要求。

### 3. 解题步骤

- 输入。
- 初始化`prev`的第0行。`prev[j] = j`。
- 遍历`a`子串长度`i`，限制`b`子串长度`j`的上下界，应用递推公式，交换`prev`与`next`。
- 输出`prev[lenb]`（注意`prev`和`next`在最后一个循环也交换了）或`-1`。



## 源代码

```c++
#include <iostream>
#include <cstring>

// 使用滚动数组降低时间复杂度
int *prev; // prev[i] 代表 dp[index-1][i] , 使用长度表示法
int *next; // next[i] 代表 dp[index][i] , 使用长度表示法
int *tmp;

// 字符串部分
int lena, lenb, K;
char *a;
char *b;

int main()
{
    // 输入
    std::cin >> lena >> lenb >> K;
    a = new char[lena];
    b = new char[lenb];
    std::cin >> a >> b;

    // 初始化
    prev = new int[lenb + 1];
    next = new int[lenb + 1];
    for (int j = 0; j <= lenb; ++j)
    {
        prev[j] = j; // dp[0][j] = j
    }

    // 动态规划
    for (int i = 1; i <= lena; ++i)
    {                // a[i](i为子串长度)
        next[0] = i; // dp[i][0] = i

        // 限制上下界以剪枝
        int lb = i - K;
        int ub = i + K;
        // lb
        if (lb >= 1)
            next[lb - 1] = K + 1;
        else
            lb = 1;
        // ub
        if (ub<lenb)
            next[ub + 1] = K + 1;
        else
            ub = lenb;

        // 遍历b[j]
        for (int j = lb; j <= ub; ++j)
        { // b[j](j为子串长度)
            next[j] = (a[i - 1] == b[j - 1]) ? prev[j - 1] : std::min(prev[j] + 1, next[j - 1] + 1);
        }
        // swap
        tmp = prev;
        prev = next;
        next = tmp;
    }

    // print
    // 注：最后一轮 prev为next
    if (prev[lenb] <= K)
    {
        std::cout << prev[lenb] << std::endl;
    }
    else
    {
        std::cout << -1 << std::endl;
    }
}
```



