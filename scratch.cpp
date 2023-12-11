#include <iostream>
#include <cmath>
#include <typeinfo>
using namespace std;

bool isPrime(int n)
{
    for (int i = 2; i < (int)sqrt(n); i++)
    {
        if (n % i == 0)
            return false;
    }
    return true;
}

int main()
{
    int *p;
    *p = 12301823;
    
    if(typeid(*p) == typeid(int))
        cout << "p is an int" << endl;
}
