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