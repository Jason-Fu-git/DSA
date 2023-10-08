#include <stdio.h>

int **ju;
int **ju_sum; // 计算以该元素为右下角的矩阵的值

int getSum(int x, int y, int a, int b)
{
    return ju_sum[x + a - 1][y + b - 1] - ju_sum[x - 1][y + b - 1] - ju_sum[x + a - 1][y - 1] + ju_sum[x - 1][y - 1];
}

int main()
{
#ifndef _OJ_
    freopen("time.out", "r", stdin);
    freopen("ans.out", "w", stdout);
#endif

    int n, m;
    scanf("%d%d", &n, &m);

    // initialize ju
    ju = new int *[n + 1];
    ju_sum = new int *[n + 1];
    for (int i = 0; i <= n; i++)
    {
        ju[i] = new int[m + 1];
        ju_sum[i] = new int[m + 1];
    }

    // input
    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= m; j++)
        {
            scanf("%d", &ju[i][j]);
        }
    }

    ju_sum[0][0] = 0;

    // scan the row
    for (int i = 1; i <= n; ++i)
    {
        ju_sum[i][0] = 0;
        for (int j = 1; j <= m; ++j)
        {
            ju_sum[i][j] = ju_sum[i][j - 1] + ju[i][j];
        }
    }

    // scan the column
    for (int i = 1; i <= m; ++i)
    {
        ju_sum[0][i] = 0;
        for (int j = 1; j <= n; ++j)
        {
            ju_sum[j][i] += ju_sum[j - 1][i];
        }
    }

    // output
    int q;
    scanf("%d", &q);
    for (int i = 0; i < q; i++)
    {
        int x, y, a, b;
        scanf("%d%d%d%d", &x, &y, &a, &b);
        printf("%d\n", getSum(x, y, a, b));
    }

    // free
    for (int i = 0; i < n; i++)
    {
        delete[] ju[i];
    }
    delete[] ju;

    return 0;
}