#include "BT.h"
#include "queue.h"
#include <iostream>

// visit
void visit(Node<int> *p)
{
    std::cout << p->data << " ";
}

int main()
{
    BT<int> bt;
    auto x1 = bt.insert(1);
    auto x2 = bt.insertAsLc(x1, 2);
    auto x3 = bt.insertAsRc(x1, 3);
    auto x4 = bt.insertAsLc(x2, 4);
    auto x5 = bt.insertAsRc(x2, 5);
    auto x6 = bt.insertAsLc(x3, 6);
    auto x7 = bt.insertAsRc(x3, 7);
    auto x8 = bt.insertAsLc(x4, 8);
    auto x9 = bt.insertAsRc(x6, 9);

    bt.preorder(visit);
    std::cout << std::endl;
    bt.inorder(visit);
    std::cout << std::endl;
    bt.postorder(visit);
    std::cout << std::endl;
    bt.levelorder(visit);
    std::cout << std::endl;
    std::cout << "Height: " << bt.getRoot()->height << std::endl;
    std::cout << "size: " << bt.getSize() << std::endl;
}