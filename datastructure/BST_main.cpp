#include "BST.h"
#include <iostream>

// visit
void visit(Node<int> *p)
{
    std::cout << p->data << " ";
}

int main(){
    BST<int> bst;
    int data[] = {2,6,1,3,6,4,7,5,9,8};
    for(int i = 0; i < 10; i++){
        bst.insert(data[i]);
    }
    
    bst.remove(0);
    bst.preorder(visit);
    std::cout << std::endl;
    
    bst.remove(3);
    bst.preorder(visit);
    std::cout << std::endl;

    bst.remove(9);
    bst.preorder(visit);
    std::cout << std::endl;

    bst.remove(8);
    bst.preorder(visit);
    std::cout << std::endl;

    bst.remove(2);
    bst.preorder(visit);
    std::cout << std::endl;


}