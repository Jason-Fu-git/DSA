# 实验报告
付甲申 2022012206

## 2.关于黑盒测试

问题一：n最大为2236
问题二：n最大为145746


## 3.调试代码

### 1. 找bug
- `solution_1.cpp`
`for`循环外部的`int sum = 0` 应移至循环内部, 采用静态检查方法。

- `solution_2.cpp`
第一处错误与`solution_1.cpp`相同。
第二处错误应把`sum += rowsum[x + j][y + b] - rowsum[x + j][y];`改为`sum += rowsum[x + j][y + b - 1] - rowsum[x + j][y - 1];`

### 2.如何用调试器单步执行
```shell
(gdb) next
```
编译选项 `-g`


### 3.`srand(time(0))`
为生成随机数设置seed为当前时间戳，确保每次seed不相同。

### 4.`system`
`>` `<` 为输入输出重定向。
`diff`为比较文件内容差异的函数。

### 5. `main`函数`argv[0]`的含义
为当前程序路径。

## 4. 优化和比较

### 1. 算法思路
在 `solution_2.cpp`的基础上，沿着列再加一次，这样`ju_sum[i][j]`中存储的就是以`ju[i][j]`为右下角的矩阵所有元素和，最后返回`ju_sum[x + a - 1][y + b - 1] - ju_sum[x - 1][y + b - 1] - ju_sum[x + a - 1][y - 1] + ju_sum[x - 1][y - 1]`即可

### 2. 测试数据
若使`solution_3.cpp`处于劣势，取n=m=10000, q=1
若使`solution_3.cpp`处于优势，取n=m=q=10000
