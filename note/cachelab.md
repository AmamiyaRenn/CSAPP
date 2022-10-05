#! https://zhuanlan.zhihu.com/p/570586637
![image](https://w.wallhaven.cc/full/7p/wallhaven-7pgm59.png)

# CSAPP lab4: cachelab

archlab 就不出题解了，因为主要内容做过课内习题后立马能明白

## 题目思路与解答

### Part A

要求在`csim.c`中写一个 cache 模拟器，要求实现与`csim-ref`一样的功能
首先是要写一个打印使用说明的函数

```c
static void usage(char *name)
{
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", name);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    exit(0);
}
```

然后需要能够解析命令行指令
这一部分内容则是可以借用 c 语言标准库中的 getopt 函数，使用这个函数需要包含

```c
#include <unistd.h>
#include <getopt.h>
```

然后就可以写出如下命令行处理代码
这个代码同时有缺少参数的提示

```c
    // 以下参数参考书中标记
    bool verbose = false;
    uint s = 0;
    uint E = 0;
    uint b = 0;
    char *trace_filepath = 0;

    // 1. read arguments and initialize
    int option = 0;
    char argFlag = 0;
    // read arguments
    while ((option = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    {
        switch (option)
        {
        case 'h':
            usage(argv[0]);
            break;

        case 'v':
            verbose = true;
            break;

        case 's':
            s = atoi(optarg);
            argFlag |= 0b1;
            break;

        case 'E':
            E = atoi(optarg);
            argFlag |= 0b10;
            break;

        case 'b':
            b = atoi(optarg);
            argFlag |= 0b100;
            break;

        case 't':
            trace_filepath = optarg;
            argFlag |= 0b1000;
            break;

        default:
            printf("Invalid option '%c'\n", option);
            usage(argv[0]);
            break;
        }
    }
    if (argFlag ^ 0b1111)
    {
        printf("%s: Missing required command line argument\n", argv[0]);
        usage(argv[0]);
    }
```

我们需要模拟一个缓存，所以需要设计一个缓存行结构体

```c
typedef unsigned long long int uint;

typedef struct
{
    uint valid;// 是否有效
    uint tag;// tag value
    uint time_stamp;// 根据LRU（最近最少使用策略），此行记载最近一次该行被使用的时间
} cache_line;
```

于是我们可以开出一个缓存

```c
    // initialize cache_line
    cache_line cache[1 << s][E]; // cache[set][line]
    memset(cache, 0, sizeof(cache));
```

接下来我们要读入文件并进行处理
值得注意的是因为读入行的方法是用`fgets`（记得开足够大，不然可能发生缓存行溢出），还有就是读入的数据可能超过了 32 位，要使用`long long int`
对于`L`与`S`，从内存命中角度来说其实是同样的，而`M`操作无非是进行了一个`L`再一个`S`罢了，所以只要实现一个`L`就足够了

```c
    // 2. read trace file and process
    FILE *trace_file = fopen(trace_filepath, "r");
    if (trace_file == NULL)
    {
        printf("%s: Open file failed!\n", argv[0]);
        usage(argv[0]);
    }

    uint hits = 0, misses = 0, evictions = 0;
    uint time_stamp = 0;// 操作次数计数器
    while (!feof(trace_file))
    {
        time_stamp++;

        // get one command line
        char op = 'x';
        uint address, size;
        char command_line[64];
        memset(command_line, 0, sizeof(command_line));
        fgets(command_line, sizeof(command_line), trace_file);
        if (command_line[0] == 'I')
            continue;
        sscanf(command_line, " %c %llx,%lld", &op, &address, &size);
        if (op == 'x')
            break;

        // process
        if (verbose)
            printf("%c %llx,%lld", op, address, size);
        bool hit = false;
        uint tag = address >> b;
        uint set = tag % (1 << s);
        for (uint isM = op == 'M' ? 2 : 1; isM != 0; isM--)
        {
            for (uint line = 0; line < E; line++)
            {
                if (cache[set][line].valid && tag == cache[set][line].tag)// 当此行valid且tag相同时命中
                {
                    cache[set][line].time_stamp = time_stamp;
                    hits++;
                    hit = true;
                    if (verbose)
                        printf(" hit");
                    break;
                }
            }
            if (!hit)// 遍历完所有缓存行后都没命中
            {
                misses++;// 产生了miss
                if (verbose)
                    printf(" miss");
                uint leastUsedLine = 0;
                for (uint line = 1; line < E; line++)
                    if (cache[set][line].time_stamp < cache[set][leastUsedLine].time_stamp)// 找到最近最少使用行
                        leastUsedLine = line;
                if (cache[set][leastUsedLine].time_stamp)// 如果时间不是0，则已经缓存数据，需要evict
                {
                    evictions++;
                    cache[set][leastUsedLine].time_stamp = time_stamp;
                    cache[set][leastUsedLine].tag = tag;
                    if (verbose)
                        printf(" eviciton");
                }
                else// 没缓存数据
                {
                    cache[set][leastUsedLine].time_stamp = time_stamp;
                    cache[set][leastUsedLine].tag = tag;
                    cache[set][leastUsedLine].valid = true;
                }
            }
        }
        if (verbose)
            printf("\n");
    }

    printSummary(hits, misses, evictions);
```

### Part B

要求我们在`trans.c`中写一个矩阵转置函数，cache miss 的次数越少越好

分析和代码可以直接看这三篇文章(就不再写了)
[CSAPP:Lab4-Cache Lab](https://zhuanlan.zhihu.com/p/142942823)
[CSAPP 实验-CacheLab](http://t.csdn.cn/3gelC)
[【读厚 CSAPP】IV Cache Lab](https://wdxtub.com/csapp/thick-csapp-lab-4/2016/04/16/)

代码如下

```c
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if (M == 32 && N == 32)
    {
        int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
        for (int row = 0; row < M; row += 8)
            for (int col = 0; col < N; col += 8)
                for (int i = row; i < row + 8; i++)
                {
                    tmp0 = A[i][col];
                    tmp1 = A[i][col + 1];
                    tmp2 = A[i][col + 2];
                    tmp3 = A[i][col + 3];
                    tmp4 = A[i][col + 4];
                    tmp5 = A[i][col + 5];
                    tmp6 = A[i][col + 6];
                    tmp7 = A[i][col + 7];
                    B[col][i] = tmp0;
                    B[col + 1][i] = tmp1;
                    B[col + 2][i] = tmp2;
                    B[col + 3][i] = tmp3;
                    B[col + 4][i] = tmp4;
                    B[col + 5][i] = tmp5;
                    B[col + 6][i] = tmp6;
                    B[col + 7][i] = tmp7;
                }
    }
    else if (M == 64 && N == 64)
    {
        int tmp0, tmp1, tmp2, tmp3;
        for (int row = 0; row < N; row += 4)
            for (int col = 0; col < M; col += 4)
                for (int i = row; i < row + 4; i++)
                {
                    tmp0 = A[i][col];
                    tmp1 = A[i][col + 1];
                    tmp2 = A[i][col + 2];
                    tmp3 = A[i][col + 3];
                    B[col][i] = tmp0;
                    B[col + 1][i] = tmp1;
                    B[col + 2][i] = tmp2;
                    B[col + 3][i] = tmp3;
                }
    }
    else if (M == 61 && N == 67)
    {
        int tmp0, tmp1, tmp2, tmp3;
        int row, col;
        int rowMax = N / 4 * 4, colMax = M / 4 * 4;
        for (row = 0; row < rowMax; row += 4)
            for (col = 0; col < colMax; col += 4)
                for (int i = row; i < row + 4; i++)
                {
                    tmp0 = A[i][col];
                    tmp1 = A[i][col + 1];
                    tmp2 = A[i][col + 2];
                    tmp3 = A[i][col + 3];
                    B[col][i] = tmp0;
                    B[col + 1][i] = tmp1;
                    B[col + 2][i] = tmp2;
                    B[col + 3][i] = tmp3;
                }
        for (row = rowMax; row < N; row++)
            for (col = 0; col < colMax; col++)
            {
                tmp0 = A[row][col];
                B[col][row] = tmp0;
            }
        for (row = 0; row < rowMax; row++)
            for (col = colMax; col < M; col++)
            {
                tmp0 = A[row][col];
                B[col][row] = tmp0;
            }
        for (row = rowMax; row < N; row++)
            for (col = colMax; col < M; col++)
            {
                tmp0 = A[row][col];
                B[col][row] = tmp0;
            }
    }
}
```

64\*64 与 61\*67 部分继续提升的难度较大，摆了

## 感想

A 部分要自己手动实现一个可以用的缓存模拟器，完成时还是很有成就感的；B 部分需要深思缓存行冲突问题，对缓存的理解加深了
看完了链接部分的我准备把 csapp 先放着了，去看 jyy 的操作系统力！
