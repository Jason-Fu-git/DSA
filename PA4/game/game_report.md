# CST 4-2 Game Report

付甲申



## 1. 题目分析

### 算法选择

典型最短路问题，采用**Dijkstra算法**。

### 数据结构选择

#### 图的存储

为方便遍历每个点的直接后继，图采用**邻接表**的形式存储。

邻接表的每一行用`VNode`封装，每一行的每个直接后继用`ArcNode`封装：

```c++
struct ArcNode
{
    int v; // 顶点编号
    ArcNode *next; // 下一个直接后继
    ArcNode(int vv, ArcNode *nextv = nullptr) : v(vv), next(nextv) {}
};
// 邻接表的一行
struct VNode
{
    ArcNode *first = nullptr;
    ArcNode *last = nullptr;
    void insert(int u)
    {              // 插入下一个顶点
        if (!last) // last为空
        {
            last = new ArcNode(u);
            first = last;
        }
        else
        { // last非空，插到它后边
            last->next = new ArcNode(u);
            last = last->next; // last后移
        }
    }
};
```

`VNode`设计了`last`指针，便于提高插入效率。



#### Dijkstra算法中集合V的存储

每次选择下一个节点时，Dijkstra算法需要遍历集合V(discovered)中的所有元素，选择其中的最大者放入集合T(visited)中。为提高选取最大值的效率，集合V使用**完全二叉堆（小顶堆）**存储，访问操作为$O(1)$，更改操作为$O(logn)$。

```c++
struct HNode
{
    int v;         // 节点编号
    int sumTime;   // 目前最短路长度
    int pathCount; // 到该点的最短路径条数
    HNode(int _v, int _length, int _pathCount) : v(_v), sumTime(_length), pathCount(_pathCount) {}
};

class CompHeap // 完全二叉堆
{
public:
    HNode **data;
    int top;                         // 堆顶位置
    inline int percolateUp(int k);   // 上滤, 返回结束位置的index
    inline int percolateDown(int k); // 下滤, 返回结束位置的index

    CompHeap(int capacity) : data(new HNode *[capacity]), top(-1) {}
    HNode *getMin() { return data[0]; } // 获取堆顶
    HNode *delMin();                    // 移除堆顶
    int insert(HNode *x);               // 插入元素, 返回插入位置的index
    int modify(int pos);                // 修改元素的值, 返回结束位置的index
    ~CompHeap() { delete[] data; }
};
```

完全二叉堆`CompHeap`的基本接口与《数据结构》讲义相似，考虑到Dijkstra算法运行过程中对堆中元素有修改，需要编写`int modify(int pos)`函数，其中`pos`为被修改元素的位置：

```c++
// 更改
int CompHeap::modify(int pos)
{
    if (Parent(pos) != -1 && data[Parent(pos)]->sumTime > data[pos]->sumTime) // 比父亲小，需要上滤
        return percolateUp(pos);
    return percolateDown(pos); // 否则下滤
}
```



#### 关卡编号到堆中编号的映射

为了迅速在堆中找到对应关卡，需要维护关卡编号到堆中编号的映射，存储在数组`posOf`中，具体规则如下：

```c++
int *posOf;  // 每个顶点在小顶堆中的位置，如果在T（visited）中，值为-1，如果在V中但没有入堆，值为-2
```





## 2. 解题步骤

### （1）初始化

- 读取通关时间，存储在数组`int* timeOf`中
- 读取道路，一分为二，正向和反向插入邻接表

### （2）Dijkstra 算法

- 当前关卡为1

- 若关卡N仍不在集合T中，依次执行以下操作：

  - 若堆非空，弹出堆顶，置为当前关卡v，`posOf[v]=-1`。
  - 遍历v的每个直接后继u
    - 若`posOf[u]==-2`，调用`heap->insert`，把u插入到堆中，更新`posOf`数组。令最短用时`time(u)=timeOf[u]+time(v)`，路径数`path(u)=path(v)`。
    - 若`posOf[u]>-1`，代表u已经在堆中。令最短用时`time(u)=min{time(u), timeOf[u]+time(v)}`。若最短用时发生更改，令路径数`path(u)=path(v)`；若之前最短用时与`timeOf[u]+time(v)`相等，累加路径数并取模。调用`heap->modify`，更新u在堆中的位置，并更新`posOf`。

- 输出关卡N的最短路径长度及数目（取模后）

  

  

## 3. 复杂度分析

### （1）时间复杂度

- 初始化：通关时间$\Omega(n)$，邻接表$\Omega(n+2m)$，总用时$\Omega(2n+2m)$
- Dijkstra算法：最坏情况下，每个节点的所有直接后继都被访问一遍，或是插入，或是修改，总复杂度$O(2mlogn)$；另外，每个节点自身从堆中弹出用时为$O(nlogn)$。

总体时间复杂度$O((n+2m)logn)$，其中n为顶点的数目，m为边的数目。

### （2）空间复杂度

所有数据结构如下：

```c++
int *timeOf; // O(n)
int *posOf;  // O(n)
VNode *AdjList; // O(n+2m)
CompHeap *heap; // O(n)
```

总体空间复杂度$O(4n+2m)$。





## 4. 源代码

```c++
#include <cstdio>

//  -------- 模仿《数据结构》讲义完全二叉堆结构(小顶堆) ----------
#define Parent(i) (((i)-1) >> 1)
#define Lc(i) (((i) << 1) + 1)
#define Rc(i) (((i) << 1) + 2)
#define MOD 1000000007

int *timeOf; // 每一关的通关时间
int *posOf;  // 每个顶点在小顶堆中的位置，如果在T（visited）中，值为-1，如果在V中但没有入堆，值为-2
int N, M;

struct HNode
{
    int v;         // 节点编号
    int sumTime;   // 目前最短路长度
    int pathCount; // 到该点的最短路径条数
    HNode(int _v, int _length, int _pathCount) : v(_v), sumTime(_length), pathCount(_pathCount) {}
};

class CompHeap // 完全二叉堆
{
public:
    HNode **data;
    int top;                         // 堆顶位置
    inline int percolateUp(int k);   // 上滤, 返回结束位置的index
    inline int percolateDown(int k); // 下滤, 返回结束位置的index

    CompHeap(int capacity) : data(new HNode *[capacity]), top(-1) {}
    HNode *getMin() { return data[0]; } // 获取堆顶
    HNode *delMin();                    // 移除堆顶
    int insert(HNode *x);               // 插入元素, 返回插入位置的index
    int modify(int pos);                // 修改元素的值, 返回结束位置的index
    ~CompHeap() { delete[] data; }
};

// 插入
int CompHeap::insert(HNode *x)
{
    data[++top] = x;         // 插到堆的最后位置
    return percolateUp(top); // 上滤
}

// 删除堆顶
HNode *CompHeap::delMin()
{
    HNode *oldMax = data[0];        // 取出堆顶元素
    data[0] = data[top--];          // 将堆尾元素放到堆顶
    HNode *x = data[0];             // 堆顶元素
    posOf[x->v] = percolateDown(0); // 堆顶元素下沉
    return oldMax;                  // 返回被删除的堆顶元素
}

// 上滤
int CompHeap::percolateUp(int k)
{
    int p;
    while ((p = Parent(k)) != -1) // 没到堆顶
    {
        if (data[p]->sumTime <= data[k]->sumTime) // 父节点小于等于子节点，即可停止，减少上滤长度
            break;
        // 交换
        HNode *temp = data[p];
        posOf[data[p]->v] = k; // 记录位置
        data[p] = data[k];
        data[k] = temp;
        // 继续上滤
        k = p;
    }
    return k; // 返回结束的位置
}

// 下滤
int CompHeap::percolateDown(int k)
{
    int lc, rc;                 // 左右孩子
    while ((lc = Lc(k)) <= top) // 左孩子若越界，必定到底
    {
        HNode *tmp = data[lc];   // 保存 data[lc], data[rc]中的最小值
        int j = lc;              // 保存最小值对应的索引
        if ((rc = Rc(k)) <= top) // 右孩子没有越界
        {
            if (data[rc]->sumTime < tmp->sumTime) // 右孩子更小
            {
                tmp = data[rc];
                j = rc;
            }
        }
        if (data[k]->sumTime <= tmp->sumTime) // 父节点小于等于子节点的最小值，满足偏序关系，停止
            break;
        // 交换
        posOf[data[j]->v] = k; // 记录位置
        data[j] = data[k];
        data[k] = tmp;
        // 继续下滤
        k = j;
    }
    return k; // 返回最后停止的位置
}

// 更改
int CompHeap::modify(int pos)
{
    if (Parent(pos) != -1 && data[Parent(pos)]->sumTime > data[pos]->sumTime) // 比父亲小，需要上滤
        return percolateUp(pos);
    return percolateDown(pos); // 否则下滤
}

// -------------- 图的邻接表表示法 ------------------
// 链表的node
struct ArcNode
{
    int v; // 顶点编号
    ArcNode *next;
    ArcNode(int vv, ArcNode *nextv = nullptr) : v(vv), next(nextv) {}
};
// 邻接表的一行
struct VNode
{
    ArcNode *first = nullptr;
    ArcNode *last = nullptr;
    void insert(int u)
    {              // 插入下一个顶点
        if (!last) // last为空
        {
            last = new ArcNode(u);
            first = last;
        }
        else
        { // last非空，插到它后边
            last->next = new ArcNode(u);
            last = last->next; // last后移
        }
    }
};

VNode *AdjList; // 邻接表
CompHeap *heap; // 小顶堆

// ---------- Dijkstra ------------
HNode *Dijkstra() // 执行Dijkstra算法, 返回最后一关
{
    HNode *currentV = new HNode(0, timeOf[0], 1); // 当前节点
    posOf[0] = -1;                                // 第一个节点纳入T
    while (posOf[N - 1] != -1)                    // 最后一关还没有进入T
    {
        if (heap->top != -1) // 堆非空
        {
            currentV = heap->delMin(); // 弹出堆顶
            posOf[currentV->v] = -1;   // 纳入T
        }
        for (ArcNode *p = AdjList[currentV->v].first; p; p = p->next)
        {                          // 遍历新纳入点的邻接表
            if (posOf[p->v] == -2) // undiscovered
            {
                HNode *newV = new HNode(p->v, currentV->sumTime + timeOf[p->v], currentV->pathCount); // 生成新节点
                posOf[p->v] = heap->insert(newV);
            }
            else if (posOf[p->v] > -1) // discovered
            {
                if (heap->data[posOf[p->v]]->sumTime > currentV->sumTime + timeOf[p->v]) // 最短路需要更新
                {
                    heap->data[posOf[p->v]]->sumTime = currentV->sumTime + timeOf[p->v]; // 更新最短路
                    heap->data[posOf[p->v]]->pathCount = currentV->pathCount;            // 更新路径数
                    posOf[p->v] = heap->modify(posOf[p->v]);                             // 更改其在小顶堆中的位置
                }
                else if (heap->data[posOf[p->v]]->sumTime == currentV->sumTime + timeOf[p->v]) // 仅更新路径数
                {
                    heap->data[posOf[p->v]]->pathCount = (heap->data[posOf[p->v]]->pathCount + currentV->pathCount) % MOD;
                }
            }
        }
        if (currentV->v != N - 1)
            delete currentV; // 释放空间
    }
    return currentV; // 一定是最后一关
}

int main()
{
    scanf("%d %d", &N, &M);
    // 初始化
    AdjList = new VNode[N];
    timeOf = new int[N];
    posOf = new int[N];
    heap = new CompHeap(N); // 初始化小顶堆
    // 更新每一关的通关时间
    for (int i = 0; i < N; ++i)
    {
        scanf("%d", &timeOf[i]);
        posOf[i] = -2; // 默认undiscovered
    }
    // 插入边
    for (int i = 0; i < M; ++i)
    {
        int v, u;
        scanf("%d %d", &v, &u);
        // 无向图，两个方向都要插入
        AdjList[u - 1].insert(v - 1);
        AdjList[v - 1].insert(u - 1);
    }
    // 开始搜索
    HNode *boss = Dijkstra();
    printf("%d\n%d\n", boss->sumTime, boss->pathCount);
    // 释放空间
    delete boss;
    delete heap;
    delete[] AdjList;
    delete[] timeOf;
    delete[] posOf;
}
```

