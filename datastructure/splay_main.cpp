#include "Splay.h"
#include <iostream>

// visit
void visit(Node<int> *p)
{
    std::cout << p->data << " ";
}

int main()
{
    Splay<int> splay;
    splay.insert(10);
    splay.insert(20);
    splay.insert(30);
    splay.levelorder(visit);
    std::cout << std::endl;
    splay.insert(40);
    splay.insert(50);
    splay.levelorder(visit);
    std::cout << std::endl;
    splay.search(10);
    splay.levelorder(visit);
    std::cout << std::endl;
    splay.search(30);
    splay.levelorder(visit);
    std::cout << std::endl;
    splay.remove(20);
    splay.levelorder(visit);
    std::cout << std::endl;
}