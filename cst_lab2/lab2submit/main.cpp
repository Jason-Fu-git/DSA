#include <cstdio>
#include <ctime>
#include "hashtable.h"

int main(int argc, char *argv[])
{
#ifndef _OJ_
    freopen("3.in", "r", stdin);
    freopen("3.out", "w", stdout);
#endif
    int type;
    char buffer[1000];
    int data;

    hashing_strategy *my_hashing = new good_hashing();
    collision_strategy *my_collision = new quadratic_probe();
    // 提取命令行参数
    if (argc > 2)
    { /// 参数1 ： 1-坏hash 2-好hash
        if (argv[1][0] == '1')
            my_hashing = new bad_hashing();
        else if (argv[1][0] == '2')
            my_hashing = new good_hashing();

        /// 参数2 ： 1-线性试探 2-平方试探 3-公共溢出区策略
        if (argv[2][0] == '1')
            my_collision = new linear_probe();
        else if (argv[2][0] == '2')
            my_collision = new quadratic_probe();
        else if (argv[2][0] == '3')
            my_collision = new overflow_probe();
    }

    // 注：此处表长必须与TABLE_SIZE宏保持一致
    // 注：该计时机制要求所有插入在查询之前
    hashtable table(TABLE_SIZE, my_hashing, my_collision);
    double insert_time = 0;
    double query_time = 0;
    double start = clock();
    while (true)
    {
        scanf("%d", &type);
        if (type == 0)
        {
            scanf("%s", buffer);
            scanf("%d", &data);
            table.insert(hash_entry(buffer, data));
        }
        else if (type == 1)
        {
            if (insert_time == 0){
                insert_time = clock() - start;
                start = clock();
            }
            scanf("%s", buffer);
            printf("%d\n", table.query(buffer));
        }
        else{
            query_time = clock() - start;
            break;
        }
    }
    printf("Insert time: %.2f ms, query time: %.2f ms", insert_time, query_time);
    return 0;
}