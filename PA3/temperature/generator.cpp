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
    int temp;

    node(int _x, int _y, int _temp) : x(_x), y(_y), temp(_temp) {}
};

int main()
{
    freopen("a.in", "w", stdout);

    srand(time(0));
    int n = 20000;
    int m = 15000;
    printf("%d \n", n);

    vector<node *> nodes;
    for (int i = 0; i < n; i += 2)
    {
        int x = rand();
        int y = rand();
        int tmp = abs(rand());
        nodes.push_back(new node(x, y, tmp));
        printf("%d %d %d\n", x, y, tmp);
        printf("%d %d %d\n", x, y, tmp);
    }

    ofstream fout("a.ans");
    for (int i = 0; i < m; i++)
    {
        int x1 = rand();
        int x2 = rand();
        int y1 = rand();
        int y2 = rand();
        if (x1 > x2)
        {
            int temp = x1;
            x1 = x2;
            x2 = temp;
        }
        if (y1 > y2)
        {
            int temp = y1;
            y1 = y2;
            y2 = temp;
        }
        int _min = -1;
        int _max = -1;
        printf("%d %d %d %d\n", x1, x2, y1, y2);
        for (node *n : nodes)
        {
            if (n->x <= x2 && n->x >= x1 && n->y <= y2 && n->y >= y1)
            {
                _max = (n->temp > _max) ? n->temp : _max;
                _min = (n->temp < _min || _min == -1) ? n->temp : _min;
            }
        }
        fout << _min << " " << _max << endl;
    }
    fout.close();
}