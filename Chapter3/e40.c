/*
 * @Description:
 * @Author: Amamiya
 * @Date: 2022-08-17 14:56:40
 * TechChangeTheWorld
 * WHUROBOCON_Alright_Reserved
 */
#include <stdio.h>

const int N = 16;
typedef int fix_matrix[N][N];

void fix_set_diag(fix_matrix A, int val)
{
    int *Aptr = &A[0][0];
    long i = 0;
    long iend = N;
    do
    {
        *Aptr = val;
        i++;
        Aptr += (N + 1) * i;
    } while (i != iend);
}