#include <cstdio>

// 注意，第一个节点不一定是最小的。

int *X; // x坐标
int *Y; // y坐标
int n;  // 节点数

int *stack;   // 主栈，注意进栈元素是索引
int top = -1; // 栈顶指针

int main()
{
    
    // 输入
    scanf("%d", &n);
    X = new int[n];
    Y = new int[n];
    stack = new int[n];

    int min_x_index = 0;
    int max_x_index = 0;
    for (int i = 0; i < n; ++i)
    {
        scanf("%d %d", &X[i], &Y[i]);
        if (X[i] < X[min_x_index])
        {
            min_x_index = i;
        }
        else if (X[i] > X[max_x_index])
        {
            max_x_index = i;
        }
    }

    // 计算多边形输入取向
    int p1 = min_x_index + 1; // [min_x_index+1,  max_x_index-1]
    int p2 = min_x_index - 1; // [min_x_index-1,  max_x_index+1]
    if (p1 >= n)
        p1 = 0;
    if (p2 < 0)
        p2 = n - 1;

    // 计算 vec(p2, min_x_index) X vec(min_x_index, p1)
    long long vec_z = ((long long)X[min_x_index] - (long long)X[p2]) * ((long long)Y[p1] - (long long)Y[min_x_index]) - ((long long)Y[min_x_index] - (long long)Y[p2]) * ((long long)X[p1] - (long long)X[min_x_index]);
    bool clockwise = (vec_z < 0); // 输入节点是顺时针则小于0

    // 主算法，一边划分，一边区分
    stack[++top] = min_x_index;

    int prev_edge = 0; // 上个节点在哪一条边上
    // 三层循环设计类比merge
    // 每个序列都是严格单调递增的
    while (p1 != max_x_index && p2 != max_x_index)
    {
        // 限定到合法范围
        if (p1 >= n)
            p1 = 0;
        if (p2 < 0)
            p2 = n - 1;

        // 模拟扫描线，取小者
        int edge = (X[p1] < X[p2]) ? 1 : 2;    // 本次所选节点在哪条边上
        int index = (edge == 1) ? p1++ : p2--; // 本次所选节点的index

        if (top == 0) // 栈中只有一个元素，初始情况，仅入栈
        {
            stack[++top] = index;
        }
        else
        {                          // 开始判断
            if (prev_edge != edge) // 不在一条边上
            {
                // 记录栈顶元素
                int top_index = stack[top--];
                int tmp = top_index;
                while (top >= 0) // 栈中还有元素
                {
                    int prev_index = stack[top--]; // 栈中上一个元素
                    // 输出
                    printf("%d %d %d %d %d %d\n", X[index], Y[index], X[top_index], Y[top_index], X[prev_index], Y[prev_index]);
                    top_index = prev_index;
                }
                stack[++top] = tmp;
                stack[++top] = index; // 进栈
            }
            else
            {                   // 在同一条边上
                while (top > 0) // 栈中至少有两个元素
                {
                    int top_index = stack[top];
                    int prev_index = stack[top - 1];
                    // 计算是否为优节点
                    // 计算 vec(prev, top) X vec(top, index)
                    long long z = ((long long)X[top_index] - (long long)X[prev_index]) * ((long long)Y[index] - (long long)Y[top_index]) - ((long long)X[index] - (long long)X[top_index]) * ((long long)Y[top_index] - (long long)Y[prev_index]);
                    if ((z > 0) == ((edge == 1) == clockwise))
                    {          // 栈顶元素为优节点
                        break; // 跳出循环
                    }
                    else
                    {
                        // 输出
                        printf("%d %d %d %d %d %d\n", X[index], Y[index], X[top_index], Y[top_index], X[prev_index], Y[prev_index]);
                        // 出栈
                        --top;
                    }
                }
                stack[++top] = index;
            }
        }
        prev_edge = edge; // 更新
    }

    /*
     */

    while (p1 != max_x_index)
    {
        // 限定到合法范围
        if (p1 >= n)
            p1 = 0;

        // 模拟扫描线，取小者
        int edge = 1;     // 本次所选节点在哪条边上
        int index = p1++; // 本次所选节点的index

        if (top == 0) // 栈中只有一个元素，初始情况，仅入栈
        {
            stack[++top] = index;
        }
        else
        {                          // 开始判断
            if (prev_edge != edge) // 不在一条边上
            {
                // 记录栈顶元素
                int top_index = stack[top--];
                int tmp = top_index;
                while (top >= 0) // 栈中还有元素
                {
                    int prev_index = stack[top--]; // 栈中上一个元素
                    // 输出
                    printf("%d %d %d %d %d %d\n", X[index], Y[index], X[top_index], Y[top_index], X[prev_index], Y[prev_index]);
                    top_index = prev_index;
                }
                stack[++top] = tmp;
                stack[++top] = index; // 进栈
            }
            else
            {                   // 在同一条边上
                while (top > 0) // 栈中至少有两个元素
                {
                    int top_index = stack[top];
                    int prev_index = stack[top - 1];
                    // 计算是否为优节点
                    // 计算 vec(prev, top) X vec(top, index)
                    long long z = ((long long)X[top_index] - (long long)X[prev_index]) * ((long long)Y[index] - (long long)Y[top_index]) - ((long long)X[index] - (long long)X[top_index]) * ((long long)Y[top_index] - (long long)Y[prev_index]);
                    if ((z > 0) == ((edge == 1) == clockwise) || z == 0)
                    {          // 栈顶元素为优节点
                        break; // 跳出循环
                    }
                    else
                    {
                        // 输出
                        printf("%d %d %d %d %d %d\n", X[index], Y[index], X[top_index], Y[top_index], X[prev_index], Y[prev_index]);
                        // 出栈
                        --top;
                    }
                }
                stack[++top] = index;
            }
        }
        prev_edge = edge; // 更新
    }

    /*
     */

    while (p2 != max_x_index)
    {
        // 限定到合法范围
        if (p2 < 0)
            p2 = n - 1;

        // 模拟扫描线，取小者
        int edge = 2;     // 本次所选节点在哪条边上
        int index = p2--; // 本次所选节点的index

        if (top == 0) // 栈中只有一个元素，初始情况，仅入栈
        {
            stack[++top] = index;
        }
        else
        {                          // 开始判断
            if (prev_edge != edge) // 不在一条边上
            {
                // 记录栈顶元素
                int top_index = stack[top--];
                int tmp = top_index;
                while (top >= 0) // 栈中还有元素
                {
                    int prev_index = stack[top--]; // 栈中上一个元素
                    // 输出
                    printf("%d %d %d %d %d %d\n", X[index], Y[index], X[top_index], Y[top_index], X[prev_index], Y[prev_index]);
                    top_index = prev_index;
                }
                stack[++top] = tmp;
                stack[++top] = index; // 进栈
            }
            else
            {                   // 在同一条边上
                while (top > 0) // 栈中至少有两个元素
                {
                    int top_index = stack[top];
                    int prev_index = stack[top - 1];
                    // 计算是否为优节点
                    // 计算 vec(prev, top) X vec(top, index)
                    long long z = ((long long)X[top_index] - (long long)X[prev_index]) * ((long long)Y[index] - (long long)Y[top_index]) - ((long long)X[index] - (long long)X[top_index]) * ((long long)Y[top_index] - (long long)Y[prev_index]);
                    if ((z > 0) == ((edge == 1) == clockwise))
                    {          // 栈顶元素为优节点
                        break; // 跳出循环
                    }
                    else
                    {
                        // 输出
                        printf("%d %d %d %d %d %d\n", X[index], Y[index], X[top_index], Y[top_index], X[prev_index], Y[prev_index]);
                        // 出栈
                        --top;
                    }
                }
                stack[++top] = index;
            }
        }
        prev_edge = edge; // 更新
    }

    // 最后一个元素, 和剩余所有元素连线
    int top_index = stack[top--];
    while (top >= 0) // 栈中还有元素
    {
        int prev_index = stack[top--]; // 栈中上一个元素
        // 输出
        printf("%d %d %d %d %d %d\n", X[max_x_index], Y[max_x_index], X[top_index], Y[top_index], X[prev_index], Y[prev_index]);
        top_index = prev_index;
    }

    delete[] X;
    delete[] Y;
    delete[] stack;
    return 0;
}