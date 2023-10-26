#include <cstdio>
// mergeSort

void mergeSort(int arr[], int l, int r);
void merge(int arr[], int l, int mid, int r);

void mergeSort(int arr[], int l, int r)
{ // sort [l, r)
    if (r - l < 2)
        return;
    int mid = (l + r) >> 1;
    mergeSort(arr, l, mid); // [l,mid)
    mergeSort(arr, mid, r); // [mid, r)
    if (arr[mid - 1] > arr[mid])
        merge(arr, l, mid, r); // merge
}

void merge(int arr[], int l, int mid, int r)
{
    int la = mid - l;
    int lb = r - mid;
    int *A = new int[la]; // A - [l,mid)
    for (int i = l; i < mid; ++i)
        A[i - l] = arr[i];
    int *B = arr + mid; // B - [mid, r)

    int i = l, j = 0, k = 0;
    while (j < la && k < lb)
    {
        if (A[j] <= B[k])
            arr[i++] = A[j++];
        else
            arr[i++] = B[k++];
    }

    while (j < la)
        arr[i++] = A[j++]; // B is done

    // if A is done, do nothing
    delete[] A;
}

int main()
{
    // test
    int arr1[] = {2, 1, 3, 9, 10, 5, 4, 6, 7, 8};
    int arr2[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int arr3[] = {1, 1, 1, 1, 1};
    int arr4[] = {5, 4, 3, 2, 1};
    mergeSort(arr1, 0, 10);
    for (int i = 0; i < 10; ++i)
        printf("%d ", arr1[i]);
    printf("\n");
    mergeSort(arr2, 0, 10);
    for (int i = 0; i < 10; ++i)
        printf("%d ", arr2[i]);
    printf("\n");
    mergeSort(arr3, 0, 5);
    for (int i = 0; i < 5; ++i)
        printf("%d ", arr3[i]);
    printf("\n");
    mergeSort(arr4, 0, 5);
    for (int i = 0; i < 5; ++i)
        printf("%d ", arr4[i]);
    printf("\n");
}