#include <iostream>
using namespace std;

const int plen_target = 1 << 11; // 分块时每一块的目标长度
const int plen_bound = 1 << 12;  // 每一块的最大长度
const int MAX = 500000;

int main()
{
#ifndef _OJ_
    freopen("03.in", "w", stdout);
#endif
    for (int i = 0; i < MAX; i++)
    {
        char c = (i % 2 == 0) ? 'A' : 'B';
        cout << c;
    }
    cout << endl;

    cout << MAX << endl;
    for (int i = 0; i < MAX; i++)
    {
        cout << 0 << ' ' << "A" << endl;
    }
}