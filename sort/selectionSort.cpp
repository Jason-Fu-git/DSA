// 列表的直接选择排序
#include "../datastructure/list.h"
#include <iostream>

template <class T>
void list_select_sort(List<T> *list)
{
    int n = list->getSize();
    ListNode<T> *p = list->end();
    while (n > 0)
    {
        list->insert(list->remove(select_max(list, n)), p);
        --n;
        p = p->pred;
    }
}

template <class T>
ListNode<T> *select_max(List<T> *list, int n)
{
    ListNode<T> *p = list->begin();
    ListNode<T> *max = p;
    while ((p = p->succ) != nullptr && (--n) > 0)
    {
        if (p->data >= max->data)
        {
            max = p;
        }
    }
    return max;
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
    int arr1[] = {2, 1, 3, 9, 10, 5, 4, 6, 7, 8};
    int arr2[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int arr3[] = {1, 1, 1, 1, 1};
    int arr4[] = {5, 4, 3, 2, 1};

    int *arrs[] = {arr1, arr2, arr3, arr4};
    int len[] = {10, 10, 5, 5};

    for (int i = 0; i < 4; ++i)
    {
        int *arr = arrs[i];
        List<int> *list1 = new List<int>();
        for (int j = 0; j < len[i]; ++j)
        {
            list1->insert(arr[j], list1->end());
        }
        list_select_sort(list1);
        print(list1);
        delete list1;
    }
}