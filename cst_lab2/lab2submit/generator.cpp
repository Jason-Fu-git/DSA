#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <ctime>
#include <algorithm>
#include "hashtable.h"
#include <random>
#include <typeinfo>
using namespace std;
#define DATASIZE 500000
#define COLLISION_TABLE_SIZE 41011 // = 10252 * 4 + 3 

vector<pair<string, int>> samples;
vector<pair<string, int>> queries;

struct Generator
{
    ifstream fin; // 输入流
    int line;     // 行数指针
    Generator()
    {
        fin.open("../dataset/poj.txt");
    }
    ~Generator()
    {
        fin.close();
    }
    // 生成插入序列
    virtual void generateI(int) = 0;
    // 生成查询序列
    virtual void generateQ(int) = 0;
    // 输入流指针回到文件开头
    void reset()
    {
        fin.clear();
        fin.seekg(ios::beg);
        line = 0;
    }
    // 让输入流到第N行
    void toLineN(int n)
    {
        reset();
        char buf[100];
        while (n--)
            fin.getline(buf, 100);
        line = n;
    }
    // 随机返回一个
    pair<string, int> random(int);
    // 读取下一个
    pair<string, int> next();
};

pair<string, int> Generator::random(int size)
{
    reset();
    int i = rand() / (double)RAND_MAX * size; // 随机找一个
    toLineN(i);                               // 输入流到第i行
    line = i;
    // 读取第i行的数据
    return next();
}

pair<string, int> Generator::next()
{
    int a, b, c;
    string s;
    fin >> a >> s >> b >> c;
    ++line;
    return pair<string, int>(s, b);
}

// 顺序生成器
// last generate : 400000 500000
struct Sequential_Generator : public Generator
{
    void generateI(int) override; // 生成插入数据
    void generateQ(int) override; // 生成查询数据
};

void Sequential_Generator::generateI(int size)
{
    if (line < size)
        samples.push_back(next());
    else
        reset();
}

void Sequential_Generator::generateQ(int size)
{
    if (line < size)
        queries.push_back(next());
    else
        reset();
}

// 均匀数据生成器
// last generate : 120000 500000
struct Even_Generator : public Generator
{
    int len;
    int _count;
    int last;
    Even_Generator()
    {
        len = 4;
        _count = 0;
        last = 0;
    }
    void generateI(int) override;
    void generateQ(int) override;
};

void Even_Generator::generateI(int size)
{
    // 顺序插入同一长度的字符串
    while (line < DATASIZE)
    {
        auto e = next();
        if (e.first.length() == len)
        {
            samples.push_back(e);
            ++_count;
            break;
        }
    }

    if (_count == 10000)
    {
        ++len;
        _count = 0;
        reset();
    }
}

void Even_Generator::generateQ(int size)
{
    // 同样在输入中抽样
    int samples_index = rand() / (double)RAND_MAX * samples.size();
    if (samples_index >= samples.size()) // rand() == RAND_MAX 的情况
        samples_index = samples.size() - 1;
    auto e = samples[samples_index];
    queries.push_back(e);
}

// 尽量制造冲突的生成器，用于评判冲突处理的性能
// last generate : 10000 2000000
// collision count / sample count  :  9253 / 10000
struct Collision_Generator : public Generator
{
    bad_hashing *bad_s;
    good_hashing *good_s;
    int collision_count;
    Collision_Generator()
    {
        bad_s = new bad_hashing();
        good_s = new good_hashing();
        collision_count = 0; // 出现了多少次冲突
    }
    void generateI(int) override;
    void generateQ(int) override;
};

void Collision_Generator::generateI(int size)
{
    // 从两个strategy中随机选一个，尽可能生成产生冲突的数据
    // 数据规模不要太大，否则生成速度极慢
    if (samples.empty())
    {                                    // 空的
        samples.push_back(random(size)); // 随机插入一个
        return;
    }
    auto last_e = samples.back(); // 取最后一个元素

    hashing_strategy *s = good_s; // 随机取一个strategy
    if (rand() % 2)
        s = bad_s;

    // convert string to char*
    char *tmp;
    tmp = new char[last_e.first.size() + 1];
    tmp[last_e.first.size()] = '\0';
    strcpy(tmp, last_e.first.c_str());

    int last_choice = (*s)(tmp, COLLISION_TABLE_SIZE);
    delete[] tmp;

    while (line < size)
    {
        auto e = next();
        // convert string to char*
        tmp = new char[e.first.size() + 1];
        tmp[e.first.size()] = '\0';
        strcpy(tmp, e.first.c_str());
        if (last_choice == (*s)(tmp, COLLISION_TABLE_SIZE)) // 冲突
        {
            samples.push_back(e);
            ++collision_count;
            delete[] tmp;
            return;
        }
    }
    // 没有重复元素
    samples.push_back(random(size)); // 就随机插一个
}

void Collision_Generator::generateQ(int size)
{
    // 为扩大冲突，仍随机查询samples里的元素
    int samples_index = rand() / (double)RAND_MAX * samples.size();
    if (samples_index >= samples.size()) // rand() == RAND_MAX 的情况
        samples_index = samples.size() - 1;
    auto e = samples[samples_index];
    queries.push_back(e);
}

int main(int argc, char **argv)
{
    srand(time(0));
    Generator *gen = new Sequential_Generator();

    int sample_num = 20000;
    int query_num = 20000;
    string output_prefix = "1";

    // 解析命令行参数
    if (argc > 3)
    {
        sample_num = atoi(argv[1]);
        query_num = atoi(argv[2]);
        if (argv[3][0] == '2')
        {
            output_prefix = "2";
            gen = new Collision_Generator();
            cout << "using Collision_Generator" << endl;
        }
        else if (argv[3][0] == '3')
        {
            output_prefix = "3";
            gen = new Even_Generator();
            cout << "using Even_Generator" << endl;
        }
        cout << "sample_num = " << sample_num << endl;
        cout << "query_num = " << query_num << endl;
    }

    // 生成
    for (int i = 0; i < sample_num; i++)
    {
        gen->generateI(DATASIZE);
        if (i % 10000 == 0)
            cout << "sample : " << i << "steps" << endl;
    }
    gen->reset();
    for (int i = 0; i < query_num; i++)
    {
        gen->generateQ(DATASIZE);
        if (i % 10000 == 0)
            cout << "query : " << i << "steps" << endl;
    }

    // 输出
    ofstream fout(output_prefix + ".in");
    ofstream aout(output_prefix + ".ans");

    // 对于均匀分布，额外打乱输出
    cout << "shuffling" << endl;
    shuffle(samples.begin(), samples.end(), default_random_engine(time(0)));
    shuffle(queries.begin(), queries.end(), default_random_engine(time(0)));

    for (int i = 0; i < sample_num; i++)
        fout << 0 << " " << samples[i].first << " " << samples[i].second << endl;

    sort(samples.begin(), samples.end());
    for (int i = 0; i < query_num; i++)
    {
        fout << 1 << " " << queries[i].first << endl;
        if (binary_search(samples.begin(), samples.end(), queries[i]))
            // 该entry已经在字典里
            aout << queries[i].second << endl;
        else
            aout << -1 << endl;
    }
    fout << 2 << endl;
    fout.close();
    aout.close();

    cout << "output saved in " << output_prefix + ".in"
         << " and " << output_prefix + ".ans" << endl;
    if (typeid(*gen) == typeid(Collision_Generator))
        cout << "collision count / sample count " << dynamic_cast<Collision_Generator *>(gen)->collision_count << " / " << sample_num << endl;
    delete gen;
}