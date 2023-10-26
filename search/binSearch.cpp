#include <cstdio>

int upperbound(int arr[], int e, int l, int h)
{
    //[l,h)
    //返回第一个大于给定值的地址
    while (l < h)
    {
        int mid = (l + h) >> 1;
        if (arr[mid] > e)
        {
            h = mid;
        }
        else
        {
            l = mid + 1;
        }
    }
    return l;
}

int upperbound(int arr[], int e, int n)
{
    return upperbound(arr, e, 0, n);
}

int lowerbound(int arr[], int e, int l, int h){
    //[l,h)
    //返回第一个大于等于给定值的地址
    while (l < h)
    {
        int mid = (l + h) >> 1; 
        if (arr[mid] < e)
        {
            l = mid + 1;
        }
        else
        {
            h = mid;
        }
    }
    return l;
}

int lowerbound(int arr[], int e, int n){
    return lowerbound(arr, e, 0, n);
}

int main()
{
    int e = 4;
    int a1[] = {0, 1, 2, 3, 3, 3};
    int a2[] = {5, 6, 7, 8, 9};
    int a3[] = {4, 4, 4, 4, 4};
    int a4[] = {1, 2, 3, 4, 4, 4, 5};

    printf("%d\n", upperbound(a1, e, 6));
    printf("%d\n", upperbound(a2, e, 5));
    printf("%d\n", upperbound(a3, e, 5));
    printf("%d\n", upperbound(a4, e, 7));

    printf("%d\n", lowerbound(a1, e, 6));
    printf("%d\n", lowerbound(a2, e, 5));
    printf("%d\n", lowerbound(a3, e, 5));
    printf("%d\n", lowerbound(a4, e, 7));

}