/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
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

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */
char transpose_submit_32_32_desc[] = "Transpose submission 32*32";
void transpose_submit_32_32(int M, int N, int A[N][M], int B[M][N])
{
    for (int row = 0; row < N; row += 8)
        for (int col = 0; col < M; col += 8)
            for (int i = row; i < row + 8; i++)
                for (int j = col; j < col + 8; j++)
                    B[j][i] = A[i][j];
}

char transpose_submit_64_64_desc[] = "Transpose submission 64*64";
void transpose_submit_64_64(int M, int N, int A[N][M], int B[M][N])
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

char transpose_submit_61_67_desc[] = "Transpose submission 61*67";
void transpose_submit_61_67(int M, int N, int A[N][M], int B[M][N])
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

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;
    for (i = 0; i < N; i++)
        for (j = 0; j < M; j++)
        {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    // registerTransFunction(transpose_submit_64_64, transpose_submit_64_64_desc);

    /* Register any additional transpose functions */
    // registerTransFunction(trans, trans_desc);
}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; ++j)
        {
            if (A[i][j] != B[j][i])
            {
                return 0;
            }
        }
    }
    return 1;
}
