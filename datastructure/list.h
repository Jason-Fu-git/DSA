#pragma once
template <class T>
class ListNode
{
public:
    T data;
    ListNode<T> *pred, *succ;
    // constructors
    ListNode() : pred(nullptr), succ(nullptr) {}
    ListNode(T _data, ListNode<T> *_pred = nullptr, ListNode<T> *_succ = nullptr) : data(_data), pred(_pred), succ(_succ) {}
    ListNode(const ListNode<T> &) = delete;
    // insert
    ListNode<T> *insertAsPred(const T &t)
    {
        ListNode<T> *node = new ListNode<T>(t, pred, this);
        this->pred->succ = node;
        this->pred = node;
        return node;
    }

    ListNode<T> *insertAsSucc(const T &t)
    {
        ListNode<T> *node = new ListNode<T>(t, this, succ);
        this->succ->pred = node;
        this->succ = node;
        return node;
    }
};

template <class T>
class List
{
    ListNode<T> *head, *tail;
    int size = 0;

public:
    // constructors
    List()
    {
        head = new ListNode<T>;
        tail = new ListNode<T>;
        head->succ = tail;
        tail->pred = head;
    }
    List(const List<T> &) = delete;
    ~List()
    {
        clear();
        delete head;
        delete tail;
    }
    // clear
    int clear()
    {
        int OldSize = size;
        while (size > 0)
        {
            remove(head->succ);
        }
        return OldSize;
    }

    // insert (default Pred)
    ListNode<T> *insert(const T &t, ListNode<T> *p)
    {
        ++size;
        return p->insertAsPred(t);
    }

    ListNode<T> *insertAsPred(const T &t, ListNode<T> *p)
    {
        ++size;
        return p->insertAsPred(t);
    }

    ListNode<T> *insertAsSucc(const T &t, ListNode<T> *p)
    {
        ++size;
        return p->insertAsSucc(t);
    }

    ListNode<T> *insertAsLast(const T &t)
    {
        ++size;
        return tail->insertAsPred(t);
    }

    // remove
    T remove(ListNode<T> *p)
    {
        T t = p->data;
        p->pred->succ = p->succ;
        p->succ->pred = p->pred;
        delete p;
        --size;
        return t; // 备份数据
    }

    // getter
    ListNode<T> *getHead() const
    {
        return head;
    }
    ListNode<T> *getTail() const
    {
        return tail;
    }
    int getSize() const
    {
        return size;
    }

    // iterator
    ListNode<T> *begin() const
    {
        return head->succ;
    }
    ListNode<T> *end() const
    {
        return tail;
    }

    // finder (the last one)
    ListNode<T> *find(const T &t, int n, ListNode<T> *p)
    { // 0 <= n <= rank(p) < _size
        while (0 < n--)
        {
            if (p->data == t)
            {
                return p;
            }
            p = p->pred;
        }
        return nullptr;
    }
};
