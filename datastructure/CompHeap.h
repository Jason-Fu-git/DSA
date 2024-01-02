#pragma once
#define Parent(i) (((i)-1) >> 1)
#define Lc(i) (((i) << 1) + 1)
#define Rc(i) (((i) << 1) + 2)

template <class T>
class CompHeap // 完全二叉堆
{
private:
    T *data;
    int size;                  // 堆中元素个数
    int top;                   // 堆顶位置
    void percolateUp(int k);   // 上浮
    void percolateDown(int k); // 下沉
public:
    CompHeap(int capacity) : data(new T[capacity + 1]()), top(-1) {}
    T getMax() { return data[0]; } // 获取堆顶
    T delMax();                    // 移除堆顶
    void insert(const T &x);       // 插入元素
    ~CompHeap() { delete[] data; }
};

template <class T>
void CompHeap<T>::insert(const T &x)
{
    data[++top] = x;  // 插到堆的最后位置
    percolateUp(top); // 向上浮动
}

template <class T>
T CompHeap<T>::delMax()
{
    T oldMax = data[0];    // 取出堆顶元素
    data[0] = data[top--]; // 将堆尾元素放到堆顶
    percolateDown(0);      // 堆顶元素下沉
    return oldMax;         // 返回被删除的堆顶元素
}

template <class T>
void CompHeap<T>::percolateUp(int k)
{
    int p;
    while ((p = Parent(k)) != -1) // 没到堆顶
    {
        if (data[p] > data[k]) // 父节点大于子节点
            break;
        // 交换
        T temp = data[p];
        data[p] = data[k];
        data[k] = temp;
        // 继续上滤
        k = p;
    }
}

template <class T>
void CompHeap<T>::percolateDown(int k)
{
    int lc, rc;
    while ((lc = Lc(k)) <= top) // 左孩子若越界，必定到底
    {
        T tmp = data[lc];
        int j = lc;
        if ((rc = Rc(k)) <= top) // 右孩子没有越界
        {
            if (data[rc] > tmp) // 右孩子更大
            {
                tmp = data[rc];
                j = rc;
            }
        }
        if (tmp <= data[k]) // 满足偏序关系，无需下滤
            break;
        // 交换
        data[j] = data[k];
        data[k] = tmp;
        // 继续下滤
        k = j;
    }
}