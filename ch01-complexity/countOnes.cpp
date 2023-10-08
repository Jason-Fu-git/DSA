// 使用位运算，统计二进制数中1的个数，时间复杂度O(countOnes(n))

#include <cstdio>

int countOnes(int n) {
    int count=0;
    while (n>0)
    {
        count++;
        n &= n-1;
    }
    return count;
}

int main(){
    int n;
    scanf("%d", &n);
    printf("%d", countOnes(n));
    return 0;
}