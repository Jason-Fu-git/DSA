# CST 2-4 Triangulation Report

付甲申 2022012206 



## 1. 题目分析

该题目要求对任意x-单调多边形进行三角剖分。核心算法已经给出，数据结构采用**栈**。题目难点在于数据预处理以及优节点的判断。

考虑到输入可能从一个x-单调多边形的任意点沿任意方向开始，在预处理阶段需要划分两条单调边，并且需要判断输入的顺序是顺时针还是逆时针。为充分节省空间，存储单调多边形的数据结构采用两个**循环数组**，分别存储每个节点的x坐标和y坐标。在输入阶段，可确认x坐标最小及最大的点。维护两个指针，一个沿着输入方向，从x坐标最小的点遍历到x坐标最大的点，记为`p1`；另一个沿相反方向，从x坐标最小的点遍历到x坐标最大的点，记为`p2`。**这两个指针分别指向两条单调边**。为方便后续处理，规定x坐标最小的点和x坐标最大的点不属于任何单调边。

接下来需要判断输入点的顺序。对此可以采用向量叉乘的方法判断，假设x坐标最小的点为$B(x_{min}, y_{min})$，输入过程中该点的上一个点为$A(x_2,y_2)$，下一个点为$C(x_1, y_1)$。计算向量叉积：
$$
\vec{AB}\times\vec{BC}=
\begin{vmatrix}
i & j & k \\
x_{min}-x_2 & y_{min}-y_2 & 0 \\
x_{1}-x_{min} & y_1 -y_{min} &0\\
\end{vmatrix}
$$
其z坐标分量为：
$$
z=(x_{min}-x_2)*(y_1-y_{min}) - (x_1-x_{min})*(y_{min}-y_2) 
$$
若$z<0$，则输入顺序是顺时针。若$z>0$，则输入顺序是逆时针。不会出现$z=0$的情况。

判断优节点时，记当前遍历到的点为$(x_i, y_i)$，栈顶及次顶的点分别为$(x_{i-1},y_{i-1})$和$(x_{i-2},y_{i-2})$，仍根据叉乘公式，沿栈中元素的顺序计算：
$$
z=(x_{i-1}-x_{i-2})*(y_i-y_{i-1}) - (x_i-x_{i-1})*(y_{i-1}-y_{i-2})
$$
经过判断，如果栈顶是优节点，必须满足下表的任意一种情况：

|  z   | 是否为顺时针（clockwise） | 是否在`p1`所指的边上(`edge==1`) |
| :--: | :-----------------------: | :-----------------------------: |
|  >0  |             T             |                T                |
|  >0  |             T             |                F                |
|  <0  |             F             |                T                |
|  <0  |             F             |                F                |
|  =0  |             -             |                -                |

经过观察，当条件`((z > 0) == ((edge == 1) == clockwise) || z == 0)`为真时，栈顶节点为优节点。



## 2. 解题步骤

### (1) 输入及数据预处理

- 初始化循环数组`X`,`Y`及栈`stack`，规模均为`n`。

- 存储输入的数据至`X`和`Y`，同时计算最小x的索引`min_x_index`，最大x的索引`max_x_index`。

- 初始化两个指针`p1`,`p2`，指向两条单调边。

- 根据上一部分分析的公式，计算输入取向，若为顺时针，`clockwise`置为`true`。

### (2) 三角剖分

- `min_x_index`对应的点进栈。

- 模拟扫描线，取`X[p1]`和`X[p2]`中的较小者对应节点$v_i$，所在边（1 or 2）记录在`edge`变量里。

- 情况1：若$v_i$与栈顶元素$v_{i-1}$不在同一条边上，执行三角剖分，把栈清空。$v_i$与$v_{i-1}$入栈。

- 情况2.1：若$v_i$与栈顶元素$v_{i-1}$在同一条边上，且`((z > 0) == ((edge == 1) == clockwise) || z == 0)`为真，即$v_{i-1}$是优节点，仅把$v_i$入栈。

- 情况2.2：若$v_i$与栈顶元素$v_{i-1}$在同一条边上，且`((z > 0) == ((edge == 1) == clockwise) || z == 0)`为假，即$v_{i-1}$是劣节点，执行三角剖分，弹出栈顶元素，重复该步骤，直至情况2.1出现或栈中仅剩一个节点。

- 重复执行上述步骤，直至指针`p1`及`p2`均指向`max_x_index`。

- 最后，令$v_i$为`max_x_index`所指的点，类似情况1，对栈中所有点执行三角剖分，清空栈。

### （3）释放空间

- 释放数组`X`,`Y`,`stack`的空间。



## 3. 复杂度分析

### （1）时间复杂度

输入过程为`O(n)`，执行三角剖分过程也为`O(n)`，算法总体时间复杂度为`O(n)`。

### （2）空间复杂度

过程中仅申请了三个规模为`n`的动态数组，故空间复杂度为`O(n)`。



## 4. 源代码

```c++
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
```

