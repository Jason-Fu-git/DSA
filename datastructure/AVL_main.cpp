#include "AVL.h"
#include <iostream>

// visit
void visit(Node<int> *p)
{
    std::cout << p->data << " ";
}

int main()
{
    AVL<int> avl;
    int data[] = {20, 60, 10, 30, 60, 40, 70, 50, 90, 80, 0};
    for (int i = 0; i < 11; i++)
    {
        avl.insert(data[i]);
    }

    avl.levelorder(visit);
    std::cout << std::endl;

    avl.remove(50);
    avl.levelorder(visit);
    std::cout << std::endl;

    avl.insert(15);
    avl.insert(25);
    avl.insert(-1);
    avl.levelorder(visit);
    std::cout << std::endl;

    avl.remove(90);
    avl.levelorder(visit);
    std::cout << std::endl;

}