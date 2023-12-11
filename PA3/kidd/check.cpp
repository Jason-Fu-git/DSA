#include <iostream>
using namespace std;

void construct(int *a, int l, int r, int len)
{
    for (int i = l; i <= r; i++)
    {
        a[i] += 1;
    }
}

int search(int *a, int l, int r, int len)
{
    int res = 0;
    for(int i = l; i <= r; i++){
        res += a[i];
    }
    return res;
}

int main()
{
    int n, m;
    cin >> n >> m;
    int *a = new int[n + 1];
    for (int i = 0; i <= n; i++)
    {
        a[i] = 0;
    }
    for (int i = 0; i < m; i++)
    {
        char c;
        int l, r;
        cin >> c >> l >> r;
        if (c == 'H')
        {
            construct(a, l, r, n);
        }
        else
        {
            cout << search(a, l, r, n) << endl;
        }
    }
    delete[] a;
}