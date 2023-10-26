#include <cstdio>
// bubbleSort - advanced

void bubbleSort(int arr[], int l, int r);
int bubble(int arr[], int l, int r);
void swap(int &a, int &b);

void bubbleSort(int arr[], int l, int r)
{
    while (l < (r = bubble(arr, l, r)))
        ;
}

int bubble(int arr[], int l, int r)
{ // rval guarantee that [rval, r) is Sorted
    int rval = l;
    while (++l < r)
    {
        if (arr[l] < arr[l - 1])
        {
            swap(arr[l], arr[l - 1]);
            rval = l;
        }
    }
    return rval;
}

void swap(int &a, int &b)
{
    int tmp = a;
    a = b;
    b = tmp;
}

int main()
{
    // test
    int arr1[] = {2, 1, 3, 9, 10, 5, 4, 6, 7, 8};
    int arr2[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int arr3[] = {1, 1, 1, 1, 1};
    int arr4[] = {5, 4, 3, 2, 1};
    bubbleSort(arr1, 0, 10);
    for (int i = 0; i < 10; ++i)
        printf("%d ", arr1[i]);
    printf("\n");
    bubbleSort(arr2, 0, 10);
    for (int i = 0; i < 10; ++i)
        printf("%d ", arr2[i]);
    printf("\n");
    bubbleSort(arr3, 0, 5);
    for (int i = 0; i < 5; ++i)
        printf("%d ", arr3[i]);
    printf("\n");
    bubbleSort(arr4, 0, 5);
    for (int i = 0; i < 5; ++i)
        printf("%d ", arr4[i]);
    printf("\n");
}