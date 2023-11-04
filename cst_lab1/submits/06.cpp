#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;

char a[1 << 22]; // 不分块的珠子
int alen;        // 珠子总长度

const int plen_target = 1 << 11; // 分块时每一块的目标长度
const int plen_bound = 1 << 12;  // 每一块的最大长度

char p[1 << 12][plen_bound]; // 分块数组
size_t plen[1 << 12];        // 每一块的长度
size_t pn;                   // 块数

struct Rank
{
    int first, second;
};

// a to p：将 a 切分成 p
void a2p()
{
    if (alen == 0)
    {
        pn = 1;
        plen[0] = 0;
        return;
    }
    int i = 0, j = 0;
    for (; j < alen; i++, j += plen_target)
    {
        int m = alen - j < plen_target ? alen - j : plen_target;
        memcpy(p[i], &a[j], m);
        plen[i] = m;
    }
    pn = i;
}

// p to a：将 p “组装”回 a
void p2a()
{
    int copied = 0;
    for (int i = 0; i < pn; i++)
    {
        memcpy(&a[copied], p[i], plen[i]);
        copied += plen[i];
    }
}

// 调试用：打印所有块，每块一行
void viewp()
{
    for (int i = 0; i < pn; i++)
    {
        for (int j = 0; j < plen[i]; j++)
            putchar(p[i][j]);
        putchar('\n');
    }
}

Rank find(int rank)
{
    int group = 0;
    while (group < pn - 1 && rank >= plen[group])
    {
        rank -= plen[group];
        group++;
    }
    return {group, rank};
}

inline char &get(Rank pos)
{
    return p[pos.first][pos.second];
}

// 在 p 上计算珠子的插入和消除
void play(int rank, char ch)
{
    Rank pos = find(rank);
    char *cur = &get(pos);
    int succ_len = plen[pos.first] - pos.second;
    if (succ_len > 0)
    {
        memmove(cur + 1, cur, succ_len);
    }
    *cur = ch;
    alen++;
    plen[pos.first]++;

    // 计算需要消除的开区间 (l, r)
    Rank l = pos, r = pos;
    Rank lbound, rbound;
    int dis = 0;
    int eliminated = 0;
    while (1)
    {
        while (l.first >= 0 && get(l) == ch)
        {
            l.second--;
            dis++;
            while (l.second < 0 && l.first >= 0)
            {
                l.first--;
                if (l.first >= 0)
                    l.second += plen[l.first];
            }
        }
        while (r.first < pn && get(r) == ch)
        {
            r.second++;
            dis++;
            while (r.second >= plen[r.first] && r.first < pn)
            {
                r.second -= plen[r.first];
                r.first++;
            }
        }
        if (dis > 3)
        {
            eliminated += dis - 1; // 注意要减1
            lbound = l;
            rbound = r;
            if (l.first >= 0)
            {
                ch = get(l);
                dis = 1; // 检查是否可以继续执行消除
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    // 执行消除
    if (eliminated > 0)
    {
        alen -= eliminated;
        l = lbound;
        r = rbound;
        if (l.first == r.first)
        { // 在同一个分块内
            int len = plen[r.first] - r.second;
            if (len > 0)
            {
                memmove(&p[l.first][l.second + 1], &p[r.first][r.second], len); // 移动后续元素
            }                                                                   // len = 0，后续元素不用移动
            plen[l.first] -= eliminated;                                        // 删除
        }
        else
        { // 不在同一个分块内
            if (l.first >= 0)
            {
                plen[l.first] = l.second + 1; // 删除l分块后续元素
            }
            if (r.first < pn)
            {
                int len = plen[r.first] - r.second;
                if (len > 0)
                {
                    memmove(&p[r.first][0], &p[r.first][r.second], len);
                }
                plen[r.first] = len; // 删除r分块前序元素
            }
            for (int i = l.first + 1; i < r.first; i++)
                plen[i] = 0; // 删除中间的分块
        }
    }
}

int main()
{

#ifndef _OJ_
    freopen("06.in", "r", stdin);
    freopen("06.out", "w", stdout);
#endif

    int n;
    fgets(a, sizeof(a), stdin);
    alen = strlen(a);
    if (alen > 0 && a[alen - 1] == '\n')
        a[--alen] = '\0';
    while (alen > 0 && (a[alen - 1] == '\n' || a[alen - 1] == '\r'))
        alen--;
    a2p();
    scanf("%d", &n);
    while (n--)
    {
        char ch;
        int rank;
        scanf("%d %c", &rank, &ch);
        play(rank, ch);
    }
    p2a();
    a[alen] = '\0';
    puts(a);
    return 0;
}
