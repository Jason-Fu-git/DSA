#pragma once

class bitmap
{
private:
    char *data;
    int N;

    void expand(int k)
    {
        if (k > N << 3)
        {
            int M = (k + 7) >> 3;
            char *tmp = new char[M];
            for (int i = 0; i < N; i++)
            {
                tmp[i] = data[i];
            }
            for (int i = N; i < M; i++)
            {
                tmp[i] = 0;
            }
            N = M;
            delete[] data;
            data = tmp;
            tmp = nullptr;
        }
    }

public:
    // 构造函数
    // @param n : minimum lenth, default value = 1
    bitmap(int n = 1)
    {
        N = (n > 0) ? (n + 7) >> 3 : 1;
        data = new char[N];
        for (int i = 0; i < N; ++i)
        {
            data[i] = 0;
        }
    }

    // 拷贝构造函数
    // @param b : another bitmap
    bitmap(const bitmap &b)
    {
        data = new char[N = b.N];
        for (int i = 0; i < N; ++i)
        {
            data[i] = b.data[i];
        }
    }

    // 移动构造函数
    // @param b : another bitmap
    bitmap(bitmap &&b)
    {
        data = b.data;
        N = b.N;
        b.data = nullptr;
    }

    // 析构函数
    ~bitmap()
    {
        delete[] data;
        data = nullptr;
    }

    // set the target bit to 1
    // @param i : target bit
    void set(int i)
    {
        expand(i);
        data[i >> 3] |= 0x80 >> (i & 0x07);
    }

    // clear the target bit to 0
    // @param i : target bit
    void clear(int i)
    {
        expand(i);
        data[i >> 3] &= ~(0x80 >> (i & 0x07));
    }

    // test the target bit
    // @param i : target bit
    // @return : T or F
    bool test(int i)
    {
        expand(i);
        return (data[i >> 3] & 0x80 >> (i & 0x07));
    }

    // reset all the bitmap
    void reset()
    {
        for (int i = 0; i < N; ++i)
        {
            data[i] = 0;
        }
    }

    // convert n bits to string
    char *to_string(int n)
    {
        expand(n - 1);
        char *str = new char[n+1];
        str[n] = 0;
        for (int i = 0; i < n; ++i){
            str[i] = test(i) ? '1' : '0';
        }
        return str;
    }
};