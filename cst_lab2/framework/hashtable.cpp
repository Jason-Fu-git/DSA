#include "hashtable.h"
#include <cstring>

int naive_hashing::operator()(char *str, int N)
{
    if (str == NULL)
        return 0;
    else
        return (str[0] + N) % N;
}
int linear_probe::operator()(hash_entry *Table, int table_size, int last_choice)
{
    return (last_choice + 1) % table_size;
}
void linear_probe::init()
{
    return; // do nothing
}

// "坏的"hash 函数 - 映射不均匀
int bad_hashing::operator()(char *str, int N)
{
    int n = strlen(str);
    int sum = 0;
    for (int i = 0; i < n; ++i)
        sum += (str[i] - ASCII_OFFSET); // 直接暴力加
    return sum % N;
}

// “好的”hash 函数 - 均匀映射
int good_hashing::operator()(char *str, int N)
{
    int n = strlen(str);
    long long sum = 0;
    // 如果n小于等于CACHE_SIZE（绝大部分情况），可以使用缓存
    if (n <= CACHE_SIZE)
        for (int i = 0; i < n; ++i)
            sum += (str[i] - ASCII_OFFSET) * Q[n - i - 1];
    else
        // 大于CACHE_SIZE不可以使用缓存
        for (int i = 0; i < n; ++i)
            sum += ((str[i] - ASCII_OFFSET) + sum * ASCII_SIZE) % N;

    return sum % (long long)N;
}

// 双向平方试探

void quadratic_probe::init()
{
    last_offset_base = 0;
}

int quadratic_probe::operator()(hash_entry *Table, int table_size, int last_choice)
{
    int offset = 0;
    if (last_offset_base > 0) // 上次试探是负的，下次试探就是正的
    {
        last_offset_base = -last_offset_base; // 取负平方项
        offset = -(last_offset_base * last_offset_base);
        if (-offset > table_size)
            offset %= table_size;
        last_choice += 2 * offset; // 下次试探位置
    }
    else
    {
        last_offset_base = -last_offset_base;               // 改为正数
        last_choice += last_offset_base * last_offset_base; // 回到原位
        ++last_offset_base;                                 // 底数加一
        offset = last_offset_base * last_offset_base;
        last_choice += offset; // 下次试探位置
    }
    return (last_choice + 2 * table_size) % table_size; // 保证返回值不是负的
}

// 公共溢出区试探

void overflow_probe::init()
{
    overflow_p = TABLE_SIZE / 2;
}

int overflow_probe::operator()(hash_entry *Table, int table_size, int last_choice)
{
    return (overflow_p++) % table_size;
}