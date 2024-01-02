#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <random>

#define MAX_DATA 100000000 // 数据的最大值

// 生成器抽象类
class Generator
{
protected:
    int randint(int n);

public:
    std::vector<int> v;                           // 存储data
    std::vector<std::pair<char, int>> op_history; // 存储操作历史
    int op_num;                                   // 操作次数
    virtual void generate(int n) = 0;             // 生成n个数
    virtual void remove(int n) = 0;               // 删除n个数
    virtual void search(int n) = 0;               // 查找n个数
    Generator() : op_num(0) {}
};

// 随机生成器
class RandomGenerator : public Generator // 随机数据生成器
{
public:
    void generate(int n) override;
    void remove(int n) override;
    void search(int n) override;
    RandomGenerator() : Generator() { srand(time(0)); }
};

// 空间局部性生成器
class LocalityGenerator : public Generator // 空间局部性数据生成器
{
private:
    int randbase; // 生成局部性数据的base
public:
    int generate_range; // 每次生成局部数据的range
    int rm_range;       // 每次删除局部数据的range
    void generate(int n) override;
    void remove(int n) override;
    virtual void search(int n) override;
    LocalityGenerator(int range = 1000, int _rm_range = 1000) : Generator(),
                                                                randbase(0), generate_range(range), rm_range(_rm_range) { srand(time(0)); }
};

// 单调序列生成器
class SequentialGenerator : public Generator // 时间局部性数据生成器
{
public:
    void generate(int n) override;
    void remove(int n) override;
    void search(int n) override;
    SequentialGenerator() : Generator() {}
};

//-----------------Generator----------------//

// [0,n)全范围随机数
int Generator::randint(int n)
{
    int step_num = n / RAND_MAX;
    int step = rand() % step_num;
    return step * RAND_MAX + rand() % RAND_MAX;
}

//-------------------RandomGenerator--------------------//

void RandomGenerator::generate(int n)
{
    v.reserve(v.size() + n);
    op_history.reserve(op_history.size() + n);
    for (int i = 0; i < n; i++)
    {
        int x = randint(MAX_DATA);
        v.push_back(x);
        op_history.push_back(std::make_pair('A', x));
    }
    op_num += n;
}

void RandomGenerator::remove(int n)
{
    sort(v.begin(), v.end());   // 排序
    unique(v.begin(), v.end()); // 去重，因为insert时可能插入重复元素
    op_history.reserve(op_history.size() + n);
    for (int i = 0; i < n; i++)
    {
        int index = randint(v.size()); // 从v中随机remove
        op_history.push_back(std::make_pair('B', v[index]));
        v.erase(v.begin() + index);
    }
    op_num += n;
}

void RandomGenerator::search(int n)
{
    sort(v.begin(), v.end());
    op_history.reserve(op_history.size() + n);
    for (int i = 0; i < n; i++)
    {
        int x = randint(MAX_DATA); // 从全局范围内随机search
        op_history.push_back(std::make_pair('C', x));
    }
    op_num += n;
}

// //-----------------LocalityGenerator-------------//
void LocalityGenerator::generate(int n)
{
    op_num += n;
    v.reserve(v.size() + n);
    op_history.reserve(op_history.size() + n);
    while (n > 0)
    {
        randbase = rand() % (MAX_DATA / RAND_MAX) * RAND_MAX; // 确定局部base
        for (int i = 0; i < std::min(n, generate_range * 10); i++)
        {
            int x = rand() % generate_range + randbase;
            v.push_back(x);
            op_history.push_back(std::make_pair('A', x));
        }
        n -= generate_range * 10;
    }
}

void LocalityGenerator::remove(int n)
{
    sort(v.begin(), v.end());   // 排序
    unique(v.begin(), v.end()); // 去重，因为insert时可能插入重复元素
    op_history.reserve(op_history.size() + n);
    op_num += n;
    while (n > 0)
    {
        randbase = rand() % (v.size() / RAND_MAX) * RAND_MAX; // 确定局部base
        for (int i = 0; i < std::min(n, rm_range); i++)
        {
            int index = randbase + rand() % rm_range; // 从v中局部remove
            op_history.push_back(std::make_pair('B', v[index]));
        }
        for (int j = op_history.size() - std::min(n, rm_range); j < op_history.size(); j++) // 根据刚刚的历史记录remove
        {
            auto it = find(v.begin(), v.end(), op_history[j].second);
            if (it != v.end())
                v.erase(it);
        }
        n -= rm_range;
    }
}

void LocalityGenerator::search(int n)
{
    op_num += n;
    sort(v.begin(), v.end());
    op_history.reserve(op_history.size() + n);
    while (n > 0)
    {
        randbase = rand() % (MAX_DATA / RAND_MAX) * RAND_MAX; // 确定局部base
        for (int i = 0; i < std::min(generate_range * 10, n); i++)
        {
            int x = randbase + rand() % generate_range; // 从局部search
            op_history.push_back(std::make_pair('C', x));
        }
        n -= generate_range * 10;
    }
}

// //-------------------SequentialGenerator--------------------//

void SequentialGenerator::generate(int n)
{
    v.reserve(v.size() + n);
    op_history.reserve(op_history.size() + n);
    for (int i = 0; i < n; i++)
    {
        int x = i;
        v.push_back(x);
        op_history.push_back(std::make_pair('A', x));
    }
    op_num += n;
}

void SequentialGenerator::remove(int n)
{
    op_history.reserve(op_history.size() + n);
    for (int i = 0; i < n; i++)
    {
        int index = 0; // 从v中按顺序remove（每次删除开头元素等价于连续删除）
        op_history.push_back(std::make_pair('B', v[index]));
        v.erase(v.begin() + index);
    }
    op_num += n;
}

void SequentialGenerator::search(int n)
{
    sort(v.begin(), v.end());
    op_history.reserve(op_history.size() + n);
    for (int i = 0; i < n; i++)
    {
        int x = v[i];
        op_history.push_back(std::make_pair('C', x));
    }
    op_num += n;
}

int main()
{
    freopen("args_dl.in", "r", stdin); // 输入重定向
    int gen_type = 0;                  // 0表示随机生成，1表示局部性，2表示顺序序列
    std::cout << "Generating...\n";
    std::cin >> gen_type;
    Generator *gen;
    if (gen_type == 0)
        gen = new RandomGenerator();
    else if (gen_type == 1)
        gen = new LocalityGenerator();
    else if (gen_type == 2)
        gen = new SequentialGenerator();
    std::ofstream fout("dl.in"); // 操作数输出文件

    char c;
    std::cin >> c;
    while (c != 'Q')
    {
        int n = 0;
        std::cin >> n;
        std::cout << c << ' ' << n << std::endl;
        if (c == 'A')
            gen->generate(n);
        else if (c == 'B')
            gen->remove(n);
        else if (c == 'C')
            gen->search(n);
        else
            break;
        std::cin >> c;
    }

    fout << gen->op_num << std::endl;     // 输出操作数
    for (int i = 0; i < gen->op_num; i++) // 输出操作历史
        fout << gen->op_history[i].first << " " << gen->op_history[i].second << std::endl;

    fout.close();
}