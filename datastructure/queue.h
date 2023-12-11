#include "list.h"
#pragma once
template <class T>
class Queue : public List<T>
{
public:
    void enqueue(T const &e) { List<T>::insertAsLast(e); } // 入队
    T dequeue() { return List<T>::remove(List<T>::begin()); } // 出队
    T &front() { return List<T>::begin()->data; }          // 队首
                                                           // 以列表首/末端为队列头/尾
};
