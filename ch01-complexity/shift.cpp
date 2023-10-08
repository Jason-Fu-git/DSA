// 采用多次反转进行数组整体shift，O(3n)
// 由于reverse操作调用连续内存较方便，此算法一般优于其他shift算法

#include <cstdio>

// 前闭后比
void reverse(int *a, int l, int r)
{
    if (l < r)
    {
        int temp = a[l];
        a[l] = a[r];
        a[r] = temp;
        reverse(a, l + 1, r - 1);
    }
}

/**
 * a - array
 * k - shift k step
 * n - len(a)
 */
void shift(int *a, int k, int n)
{
    k %= n;
    reverse(a, 0, n - 1);
    reverse(a, 0, k - 1);
    reverse(a, k, n - 1);
}

void printArray(int *a, int n){
    for(int i = 0; i < n; i++){
        printf("%d ", a[i]);
    }
    printf("\n");
}

int main(){
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    shift(a, 3, 9);
    printArray(a, 9);
}