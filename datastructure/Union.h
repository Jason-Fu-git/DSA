#pragma once

// 并查集

class UnionFindSet
{
public:
    int *size;
    int *tag;
    int _n;
    UnionFindSet(int n)
    {
        _n = n;
        size = new int[n];
        tag = new int[n];
        for (int i = 0; i < n; i++)
        {
            tag[i] = i;
            size[i] = 1;
        }
    }
    ~UnionFindSet()
    {
        delete[] size;
        delete[] tag;
    }

    int Union(int a, int b); // 合并两个等价类，默认合并到a的等价类上，返回tag
    int Find(int a);         // 找到a的等价类
};

int UnionFindSet::Find(int a) // a和等价类标志节点间所有节点都attach到a上
{
    if (a >= _n || a < 0) // 越界情况
        return -1;

    int b = tag[a];
    while (b != tag[b]) // 不形成自环，一定有父亲
    {
        int next = tag[b];
        tag[b] = a; // a成为父亲
        b = next;   // 上溯至祖先
    }
    tag[a] = b; // 压缩路径长度至2
    return b;   // 返回等价类标志节点
}

int UnionFindSet::Union(int a, int b)
{
    // 找到各自的等价类
    int fa = Find(a);
    int fb = Find(b);
    if (fa == fb) // 同一个等价类
        return fa;
    // 规模小的等价类合并到规模大的等价类上，以减小高度
    if (size[fa] < size[fb])
    {
        tag[fa] = fb; // 合并到b的等价类上
        size[fb] += size[fa];
        return fb;
    }

    tag[fb] = fa; // 合并到a的等价类上
    size[fa] += size[fb];
    return fa;
}