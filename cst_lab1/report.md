# CST LAB1 ZUMA

## 01.cpp

### 1. 最严重错误类型
Runtime Error

### 2. 错误原因
数组访问越界错误，出现在`play`函数中：
```c++
void play(int rank) {
    int left = rank;
    int right = rank;
    char color = a.at(rank); // error here

    while (left >= 0 && a.at(left) == color) --left;
    left += 1;
    while (right < a.size() && a.at(right) == color) ++right;

    int size = right - left;
    if (size >= 3) {
        a.erase(left, size);
        play(left - 1); // error here
    }
}
```
`play(left-1)`语句没有考虑`left==0`的情况，导致`char color = a.at(rank)`语句`rank`为-1，越界。

### 3. 测例构造思路

只需保证过程中有前三个连续元素相消即可。如过程中序列为"CCCB"，即可使`left`为0，导致程序出现Runtime Error。故测例构造时，初始序列为"CCB",之后在位置1插入"C"即可。




## 02.cpp

### 1. 最严重错误类型
Runtime Error

### 2. 错误原因
数组访问越界错误，出现在`play`函数中：
```c++
void play(int rank) {
    int left = rank;
    int right = rank;
    char color = a.at(rank); // error here

    while (left >= 0 && a.at(left) == color) --left;
    left += 1;
    while (right < a.size() && a.at(right) == color) ++right;

    int size = right - left;
    if (size >= 3) {
        a.erase(left, size);

        int next = left;
        if (left - 1 >= 0) next = left - 1;

        play(next); // error here
    } 
}
```
相较于01.cpp, 02.cpp避免了`next`为-1的错误，但未考虑消除后序列为空串的情况。例如，若过程中序列为"CCC"，则next为0，递归调用`play`函数时，rank为0，访问空串中的0号元素，导致越界报错。

### 3. 测例构造思路

根据上述分析，使过程中序列为连续的三个相同元素。初始串为"CC",之后在位置0插入"C"即可。



## 03.cpp

### 1. 最严重错误类型
Time Limit Exceeded

### 2. 错误原因
使用了`string`模板类，没有采用分块数组，时间复杂度过高。假设每次插入及消除都发生在开头，则每次插入/消除的时间复杂度均为O(n)，故最坏情况下时间复杂度为O(mn)。当输入规模取最大，且使最坏情况发生时，需要进行的浮点运算次数为$10^{11}$量级，显然超时。

### 3. 测例构造思路
数据规模取最大，使最坏输入情况发生，此时程序运行用时最大。测例的m，n都取最大值。构造输入时，每次插入及消除都发生在开头，即可使最坏情况发生。



## 04.cpp

### 1. 最严重错误类型
Wrong Answer

### 2. 错误原因
消除过程错误，出现在`play`函数中：
```c++
void play(int rank) {
    int left = rank;
    int right = rank;
    char color = a.at(rank);

    while (left > 0 && a.at(left) == color) --left; // error here
    while (right < a.size() && a.at(right) == color) ++right;

    int size = right - left;
    if (size >= 3) {
        a.erase(left, size);

        if (left >= 0 && left < a.size()) {
            play(left);
        }
    }
}
```
问题出现在第一个while循环中，第一个while循环结束后，`left`未加1，导致区间长度相较于正确区间长度大1，此时，若rank指向两个连续相同元素中的一个，仍满足`size>=3`，erase函数会将这两个元素的上一个元素一并删除，答案错误。

### 3. 测例构造思路
插入过程中，使插入元素与其后一位元素相同，与其前一位元素不同即可。例如，初始序列为"ABA"，此时在1号位插入"B"，该串为"ABBA"，不应该发生消除。但根据上述分析，程序会删除"ABB"串，答案错误。



## 05.cpp

### 1. 最严重错误类型
Wrong Answer

### 2. 错误原因
错误发生在输入过程中：
```c++
cin >> a; // error here
int m = 0;
cin >> m;
```
读取字符串时，未考虑空串(n=0)的情况。当输入为空串时，`cin>>a`遇到换行符会直接读取下一行的m，导致初始字符串a的初始值为数字，而不是空串，最后的输出自然错误。

### 3. 测例构造思路
构造测例时，输入为空串，进行一次及以上的插入操作即可。



## 06.cpp


### 1. 最严重错误类型
Wrong Answer

### 2. 错误原因
`memmove`过程可能发生异常覆盖的现象，错误根源出现在`play`函数的插入过程中：

```c++
Rank pos = find(rank);
char *cur = &get(pos);
int succ_len = plen[pos.first] - pos.second;
if (succ_len > 0)
{
    memmove(cur + 1, cur, succ_len);
}
*cur = ch;
alen++;
plen[pos.first]++;
// error here
// 计算需要消除的开区间 (l, r)
Rank l = pos, r = pos;
Rank lbound, rbound;
```
插入之后，没有检查当前长度`plen[pos.first]`是否已经大于该分块数组长度`plen_bound`。如果当前长度已经大于该分块数组长度，超出的部分会将下一个分块数组的值覆盖，答案错误。

### 3. 测例构造思路

使插入过程中，`plen[pos.first]`>`plen_bound`即可。初始构造两个分块数组，第一个分块数组为重复的"AB"串，第二个分块数组为重复的"CD"串。之后在0位置相继插入共`plen_bound`次的"B"和"A"。正确答案应该是"AB"串后接"CD"串，但程序答案为"AB"串。



## 07.cpp

### 1. 最严重错误类型
Wrong Answer

### 2. 错误原因

分块数组出现空行时，空行前后的分块数组无法正常执行跨分块的连续消除过程，错误出现在`play`函数中`while(1)`中的第一个循环中：

```c++
while (l.first >= 0 && get(l) == ch) {
    l.second--;
    dis++;
    if (l.second < 0 && l.first >= 0) { // error here
        l.first--;
        if (l.first >= 0)
            l.second += plen[l.first];
    }
}
```
上方所示代码所标记行的`if`应为`while`，此错误会导致`l.first`无法跳过空行，进一步可能导致无法正常连续消除。例如，假设第一行为...AB, 第二行为空行，第三行为AABB。此时在第三行行首插入A时，第三行AAA会被消掉。但`l.first`在之后的循环中会停在第二行，`l.second`为-1，此时第一行末尾的B不会与第三行开头的BB相消，最后输出...BBB...,答案错误。

### 3. 测例构造思路
构造跨空行的连续消除过程即可。初始构造三个分块数组。第一个分块为连续的"AB"串，第二个分块为连续的"CCDD"串，第三个分块为连续的"AB"串。之后相继在第二个分块的尾部插入"D""C"，直至第二个分块为空串。接着在第三个分块的1号位先后插入"B""A"，使第三个分块的开头为"AABB"。接着在第三行行首插入"A"，出现上一部分分析的错误，最后输出部分包含...ABBBA...,答案错误。




## 08.cpp

### 1. 最严重错误类型
Wrong Answer

### 2. 错误原因
与其他同类算法相比，该程序未设置`while(1)`循环，导致无法连续相消。

### 3. 测例构造思路
只需设置一个连续相消的测例即可。初始串为"AABBAC"，插入操作为在2号位插入"B"。程序输出"AAAC"，答案错误。




## 09.cpp

### 1. 最严重错误类型
Runtime Error

### 2. 错误原因
`memcopy`过程可能出现`Segmegation fault`，错误根源出现在`play`函数执行消除部分：

```c++
// 执行消除
if (eliminated > 0) {
    alen -= eliminated;
    l = lbound;
    r = rbound;
    // error here
    if (l.first >= 0) {
        plen[l.first] = l.second + 1;
    }
    if (r.first < pn) {
        int len = plen[r.first] - r.second;
        if (len > 0) {
            memmove(&p[r.first][0], &p[r.first][r.second], len);
        }
        plen[r.first] = len;
    }
    for (int i = l.first + 1; i < r.first; i++)
        plen[i] = 0;
    
}
```
条件判断缺少`if (l.first == r.first) `。导致`l.first==r.first`情况出现错误。这种情况下，必有`r.second > l.second + 3`为真，此时`len<=-3`为真，后续`p2a`函数调用时，`memcpy(&a[copied], p[i], plen[i])`语句最后的`size`参数为负数，会出现Segmengation fault。

### 3. 测例构造思路

只需使`l`，`r`在同一行，在中间发生消除即可。初始串为"ABBA"，在1号位插入"B"，此时`l.first==r.first`为真，`l.second`为0， `r.second`为4，`len`为-3，在`p2a`函数出现Segmengation fault。




## 10.cpp

### 1. 最严重错误类型
Wrong Answer

### 2. 错误原因
复制过程错误，错误根源出现在`play`函数的执行消除部分：
```c++
for (int i = l.first; i < r.first; i++) // error here
    plen[i] = 0;
```
正确情况下，`i`的初始值应设为`l.first+1`。否则若`l.first`行仍有数据，这样做会把该行的plen置为0，导致后续的相消或输出时出现错误。例如，假设相消发生在第一个分块与第二个分块的交界处，这次相消后立即输出。则在消除部分，`plen[0]`为0，`p2a`函数执行时会错误地把第二个分块的剩余部分复制到`a`的首部，最后输出结果可能有误。

### 3. 测例构造思路
使消除部分发生在两个分块的交界处，之后立即输出即可。首先用"AB"串填充第一个分块，第二个分块设为"BC"。唯一的一次操作是将"B"插到B与C之间，此时相消发生在两个分块的交界处，出现如上一部分分析的情况，第二个分块的剩余部分"C"被复制到`a`的首部，但正确情况下`a`的首部应为"A"，答案错误。