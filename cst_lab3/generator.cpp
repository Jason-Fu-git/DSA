#include <vector>
#include <iostream>
#include <algorithm>
#include <random>
#include <ctime>
#include <fstream>
using namespace std;

vector<int> v;
int main()
{
    freopen("a.in", "w", stdout);

    v.reserve(100000);
    int n = 220000;
    srand(time(0));
    cout << n << endl;

    for (int i = 0; i < 100000; i++)
    {
        v.push_back(rand());
    }

    for (int i = 0; i < 100000; i++)
    {
        cout << 'A' << ' ' << v[i] << endl;
    }

    sort(v.begin(), v.end());
    for (int i = 0; i < 50000; i++)
    {
        int rm = rand();
        cout << 'B' << ' ' << rm << endl;
        auto it = find(v.begin(), v.end(), rm);
        if (it != v.end())
        {
            v.erase(it);
        }
    }

    ofstream fout("a.ans");
    for (int i = 0; i < 70000; i++)
    {
        int se = rand();
        cout << 'C' << ' ' << se << endl;
        auto u = lower_bound(v.begin(), v.end(), se);
        if (u == v.begin())
            fout << -1 << endl;
        else
        {
            if (*u > se)
                u--;

            fout << *u << endl;
        }
    }
}