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