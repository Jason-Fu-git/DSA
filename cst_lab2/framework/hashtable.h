#include <cstring>
#include <cstdio>
#include <typeinfo>
#define ASCII_OFFSET 32
#define ASCII_SIZE 94
#define CACHE_SIZE 100
// const int TABLE_SIZE = 41011; // = 10252 * 4 + 3 (collision)
// const int TABLE_SIZE = 499883; // = 124970 * 4 + 3 (sequential)
const int TABLE_SIZE = 1000003; // = 31247 * 4 + 3 (even)
struct hash_entry
{
    char *my_string;
    int my_data;
    hash_entry()
    {
        my_string = NULL;
        my_data = 0;
    }
    hash_entry(char *str, int data) : my_data(data)
    {
        my_string = new char[strlen(str) + 1];
        strcpy(my_string, str);
    }
};
struct hashing_strategy
{
    virtual int operator()(char *str, int N) = 0;
};
struct naive_hashing : public hashing_strategy
{
    int operator()(char *str, int N) override;
};
struct bad_hashing : public hashing_strategy // “坏”的hash函数
{
    int operator()(char *str, int N) override;
};
struct good_hashing : public hashing_strategy // “好”的hash函数
{
    long long Q[CACHE_SIZE]; // 涉及大量乘方操作，需要缓存
    good_hashing()
    {
        // 初始化缓存
        Q[0] = 1;
        for (int i = 1; i < CACHE_SIZE; i++)
            Q[i] = Q[i - 1] * ASCII_SIZE % TABLE_SIZE;
    }
    int operator()(char *str, int N) override;
};

struct collision_strategy
{
    virtual void init() = 0; // pure virtual function
    virtual int operator()(hash_entry *Table, int table_size, int last_choice) = 0;
};
struct linear_probe : public collision_strategy
{
    void init();
    int operator()(hash_entry *Table, int table_size, int last_choice) override;
};
// 平方试探
struct quadratic_probe : public collision_strategy
{
    int last_offset_base; // 上一次试探所加的平方底数（带符号），init为0，可以取为0,1,-1,2,-2,3,-3,...
    void init() override;
    int operator()(hash_entry *Table, int table_size, int last_choice) override;
};
// 公共溢出区试探
struct overflow_probe : public collision_strategy
{
    int overflow_p; // 上次试探时overflow位置的指针
    void init() override;
    int operator()(hash_entry *Table, int table_size, int last_choice) override;
};
struct hashtable
{
    hash_entry *Table;
    int table_size;
    hashing_strategy *my_hashing; // 如果改为hashing_strategy my_hashing, 即不是用指针作为hashtable的成员, 而是让hashing_strategy结构体直接作为hashtable的成员. 会发生什么bug?
    collision_strategy *my_collision;
    hashtable(int size, hashing_strategy *hashing, collision_strategy *collision)
        : table_size(size), my_hashing(hashing), my_collision(collision)
    {
        Table = new hash_entry[table_size];
    }
    bool insert(hash_entry entry)
    {
        // 注意：此函数不进行验重，即会进行重复key的插入
        // 注意：该函数未处理hashtable已满的情况，可能发生覆盖
        int last_choice = (*my_hashing)(entry.my_string, table_size);
        //// 此处发生更改
        if (typeid(my_collision) == typeid(overflow_probe))
            last_choice = (*my_hashing)(entry.my_string, table_size / 2); // 缓冲区分割
        //// ----------
        my_collision->init();
        while (Table[last_choice].my_string != NULL)
        { // loop infinitely? return false when no more space?
            last_choice = (*my_collision)(Table, table_size, last_choice);
            if (last_choice >= table_size || last_choice < 0)
                printf("Error: hash function returns negative value %d\n", entry.my_data);
        }
        Table[last_choice] = entry;
        return true;
    }
    int query(char *query_string)
    {
        // 注意：该函数未处理hashtable已满的情况，查找未出现词条时可能死循环
        int last_choice = (*my_hashing)(query_string, table_size);
        //// 此处发生更改
        if (typeid(my_collision) == typeid(overflow_probe))
            last_choice = (*my_hashing)(query_string, table_size / 2); // 缓冲区分割
        //// ----------
        my_collision->init();
        while (Table[last_choice].my_string != NULL &&
               strcmp(Table[last_choice].my_string, query_string) != 0)
        { // 未处理的情况: 哈希表已满?
            last_choice = (*my_collision)(Table, table_size, last_choice);
        }
        if (Table[last_choice].my_string == NULL)
        {
            return -1; // 不存在，return -1
        }
        else
        {
            return Table[last_choice].my_data;
        }
    }
};
