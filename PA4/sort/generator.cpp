#include <iostream>
#include <algorithm>
#include <random>
#include <ctime>
using namespace std;

int main()
{
    freopen("a.in", "w", stdout);
    int n = 1000000;
    int limit = n * 100;
    cout << n << ' ' << limit << endl;
    int *a = new int[n];
    for (int i = 0; i < n; ++i)
        a[i] = i;
    shuffle(a, a + n, default_random_engine(time(0)));
    for (int i = 0; i < n; ++i)
        cout << a[i] << ' ';
    cout << endl;
    delete[] a;
}