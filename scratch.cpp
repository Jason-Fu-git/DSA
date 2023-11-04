#include <stdio.h>
#include <stdlib.h>

void parseStrings(int N, char **str)
{
    for (int i = 0; i < N; i++)
    {
        int m;
        if (sscanf(str[i], "%d", &m) != 1)
        {
            // 如果无法解析第一个数字，可以处理错误或者跳过该行
            printf("无法解析行 %d\n", i + 1);
            continue;
        }

        printf("第 %d 行包含 %d 个数字: ", i + 1, m);

        char *token = strtok(str[i], " ");
        while (token != NULL)
        {
            printf("%s ", token);
            token = strtok(NULL, " ");
        }
        printf("\n");
    }
}

int main()
{
    char *str[] = {"3 1 2 3", "1 2"};
    int N = sizeof(str) / sizeof(str[0]);

    parseStrings(N, str);

    return 0;
}
