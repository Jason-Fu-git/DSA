#include <cstdio>
// 马鞍点查找（二维数组，A[k]是所在行中最大，所在列中最小的元素）

/*Q:
A[i][j]是每行、每列严格递增的矩阵，请找出其中所有值等于e的元素位置。
*/

int upperbound(int *A, int n, int e)
{
    // O(logn)
    // 二分查找
    int l = 0, r = n;
    while (l < r)
    {
        int mid = (l + r) >> 1;
        if (A[mid] <= e)
        {
            l = mid + 1;
        }
        else
        {
            r = mid;
        }
    }
    return l - 1;
}

int saddlebackSearch(int **A, int n, int m, int e)
{
    // O(s+r+logm)
    // 找第0行的鞍点（不大于e的最大值）

    // binary search
    int count = 0;
    int r = upperbound(A[0], m, e);
    int i = 0;
    int j = r;
    while (i < n && j >= 0) // O(r+s)
    {
        if (A[i][j] > e)
        {
            --j;
        }
        else if (A[i][j] < e)
        {
            ++i;
        }
        else
        {
            printf("%d %d\n", i, j);
            --j;
            ++i;
            ++count;
        }
    }
    return count;
}

int main()
{

    // 测试数据
    int n, m, e;
    scanf("%d %d %d", &n, &m, &e);

    int **A = new int *[n];
    for (int i = 0; i < n; i++)
    {
        A[i] = new int[m];
        for (int j = 0; j < m; j++)
        {
            scanf("%d", &A[i][j]);
        }
    }

    printf("%d\n", saddlebackSearch(A, n, m, 5));
}