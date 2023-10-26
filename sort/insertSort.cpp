// 列表的插入排序
#include "../datastructure/list.h"
#include <iostream>

int cmp_count = 0;

template <typename T>
void insert_sort(List<T> *list)
{
    int r = 0;
    int n = list->getSize();
    ListNode<T> *p = list->begin();
    while (n-- > 0)
    {
        list->insert(p->data, find_le(list, p, r));
        p = p->succ;
        list->remove(p->pred);
        ++r;
    }
}

template <typename T>
ListNode<T> *find_le(List<T> *list, ListNode<T> *p, int r)
{
    ListNode<T> *q = p->pred;
    while (r-- > 0 && q->data > p->data)
    {
        q = q->pred;
        ++cmp_count;
    }
    ++cmp_count;
    return q->succ;
}

void print(List<int> *list)
{
    ListNode<int> *p = list->begin();
    while (p->succ != nullptr)
    {
        std::cout << p->data << " ";
        p = p->succ;
    }
    std::cout << std::endl;
}

int main()
{
    // int arr1[] = {2, 1, 3, 9, 10, 5, 4, 6, 7, 8};
    // int arr2[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    // int arr3[] = {1, 1, 1, 1, 1};
    // int arr4[] = {5, 4, 3, 2, 1};

    // int *arrs[] = {arr1, arr2, arr3, arr4};
    // int len[] = {10, 10, 5, 5};

    // for (int i = 0; i < 4; ++i)
    // {
    //     int *arr = arrs[i];
    //     List<int> *list1 = new List<int>();
    //     for (int j = 0; j < len[i]; ++j)
    //     {
    //         list1->insert(arr[j], list1->end());
    //     }
    //     insert_sort(list1);
    //     print(list1);
    //     delete list1;
    // }

    int *arr = new int[63];
    for (int i = 0; i < 63; ++i)
    {
        if (i < 60)
            arr[i] = 61 - i;
        else
            arr[i] = i - 60;
    }
    List<int> *list1 = new List<int>();
    for (int j = 0; j < 63; ++j)
    {
        list1->insert(arr[j], list1->end());
    }
    insert_sort(list1);
    std::cout << cmp_count << std::endl;
    print(list1);
    delete list1;
}