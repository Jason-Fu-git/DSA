#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
using namespace std;

void generate_data(int n, int m, int q)
{
    // randomly generate input data:
    // matrix element between 0 and 1000
    // q between 10 and 20
    // x between 1 and n
    // y between 1 and m
    // a between 1 and n-x+1,
    // b between 1 and m-y+1
    srand(time(0));
    ofstream f("time.out");
    f << n << " " << m << endl;
    // TODO: generate matrix, q, and queries.
    for (int i = 1; i <= n; ++i)
    {
        for (int j = 1; j <= m; ++j)
        {
            f << rand() % 1001 << " ";
        }
        f << endl;
    }
    f << q << endl;
    for (int i = 1; i <= q; ++i)
    {
        int x = rand() % n + 1, y = rand() % m + 1;
        int a = rand() % (n - x + 1) + 1, b = rand() % (m - y + 1) + 1;
        f << x << " " << y << " " << a << " " << b << endl;
    }
    f.close();
}

int main()
{
    system("g++ solution_1.cpp -o solution_1");
    system("g++ solution_2.cpp -o solution_2");
    system("g++ solution_3.cpp -o solution_3");

    int n = 10000;
    int m = 10000;
   
            for (int q = 10; q <= 1000; q += 200)
            {
                generate_data(n, m, q);
                int time1 = clock();
                system(".\\solution_1.exe");
                int time2 = clock();
                system(".\\solution_2.exe");
                int time3 = clock();
                system(".\\solution_3.exe");
                int time4 = clock();
                printf("%d %d %d %d %d %d\n", n, m, q, time2 - time1, time3 - time2, time4 - time3);
            }
        
    system("pause");
}