#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <memory.h>

typedef unsigned long long int uint;

// print helpful diagnostic information
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

typedef struct
{
    uint valid;
    uint tag;
    uint time_stamp;
} cache_line;

int main(int argc, char **argv)
{
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
    // initialize cache_line
    cache_line cache[1 << s][E]; // cache[set][line]
    memset(cache, 0, sizeof(cache));

    // 2. read trace file and process
    FILE *trace_file = fopen(trace_filepath, "r");
    if (trace_file == NULL)
    {
        printf("%s: Open file failed!\n", argv[0]);
        usage(argv[0]);
    }

    uint hits = 0, misses = 0, evictions = 0;
    uint time_stamp = 0;
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
                if (cache[set][line].valid && tag == cache[set][line].tag)
                {
                    cache[set][line].time_stamp = time_stamp;
                    hits++;
                    hit = true;
                    if (verbose)
                        printf(" hit");
                    break;
                }
            }
            if (!hit)
            {
                misses++;
                uint leastUsedLine = 0;
                for (uint line = 1; line < E; line++)
                    if (cache[set][line].time_stamp < cache[set][leastUsedLine].time_stamp)
                        leastUsedLine = line;
                if (cache[set][leastUsedLine].time_stamp)
                {
                    evictions++;
                    cache[set][leastUsedLine].time_stamp = time_stamp;
                    cache[set][leastUsedLine].tag = tag;
                    if (verbose)
                        printf(" miss eviciton");
                }
                else
                {
                    cache[set][leastUsedLine].time_stamp = time_stamp;
                    cache[set][leastUsedLine].tag = tag;
                    cache[set][leastUsedLine].valid = true;
                    if (verbose)
                        printf(" miss");
                }
            }
        }
        if (verbose)
            printf("\n");
    }

    printSummary(hits, misses, evictions);
    return 0;
}