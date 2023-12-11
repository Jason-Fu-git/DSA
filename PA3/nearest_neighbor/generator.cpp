#include <cstdio>
#include <vector>
#include <random>
#include <ctime>
#include <fstream>
#include <cmath>
using namespace std;

struct node
{
    int x;
    int y;

    node(int _x, int _y) : x(_x), y(_y) {}
};

int main()
{
    freopen("a.in", "w", stdout);

    srand(time(0));
    int n = 20000;
    int m = 15000;
    printf("2 %d \n", n);

    vector<node *> nodes;
    for (int i = 0; i < n; i += 2)
    {
        int x = rand();
        int y = rand();
        nodes.push_back(new node(x, y));
        printf("%d %d\n", x, y);
        printf("%d %d\n", x, y);
    }

    ofstream fout("a.ans");
    printf("%d\n", m);
    for (int i = 0; i < m; i++)
    {
        int x1 = rand();
        int y1 = rand();
        int _min = 1 << 30;
        printf("%d %d\n", x1, y1);
        for (node *n : nodes)
        {
            int dis = (n->x - x1) * (n->x - x1) + (n->y - y1) * (n->y - y1);
            if (dis < _min)
            {
                _min = dis;
            }
        }
        fout << _min << endl;
    }
    fout.close();
}