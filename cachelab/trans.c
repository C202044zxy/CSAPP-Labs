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
    if(N == 32 && M == 32)
    {
        int c[8];
        for(int i = 0; i < N; i += 8)
            for(int j = 0; j < M; j += 8)
                for(int k1 = 0; k1 < 8; k1++) {
                    for(int k2 = 0; k2 < 8; k2++)
                        c[k2] = A[i + k1][j + k2];
                    for(int k2 = 0; k2 < 8; k2++)
                        B[j + k2][i + k1] = c[k2];
                }
        return ;
    }
    if(N == 64 && M == 64)
    {
        // 1571
        int c[4];
        int tmp[2][4];
        for(int i = 0; i < N; i += 4)
            for(int j = 0; j < M; j += 4)
            {
                for(int k1 = 0; k1 < 4; k1++) {
                    for(int k2 = 0; k2 < 4; k2++)
                    {
                        if(k1 < 2 && (j/4)&1) c[k2] = tmp[k1][k2];
                        else c[k2] = A[i + k1][j + k2];
                    }
                    if(k1 < 2 && ((j/4)&1) == 0) { // save next
                        for(int k2 = 0; k2 < 4; k2++) {
                            tmp[k1][k2] = A[i + k1][j + k2 + 4];
                        }
                    }
                    for(int k2 = 0; k2 < 4; k2++)
                        B[j + k1][i + k2] = c[k2];
                }
                for(int k1 = 0; k1 < 4; k1++)
                    for(int k2 = 0; k2 < k1; k2++) {
                        c[0] = B[j + k1][i + k2];
                        B[j + k1][i + k2] = B[j + k2][i + k1];
                        B[j + k2][i + k1] = c[0];
                    }
            } 
        return ;
    }
    // 61 * 67
    // 2067
    int n = 56, m = 64;
    int c[8];
    for(int i = 0; i < n; i += 8)
        for(int j = 0; j < m; j += 8)
            for(int k1 = 0; k1 < 8; k1++) {
                for(int k2 = 0; k2 < 8; k2++)
                    c[k2] = A[i + k1][j + k2];
                for(int k2 = 0; k2 < 8; k2++)
                    B[j + k2][i + k1] = c[k2];
            }
    for(int i = n; i < N; i += 5)
        for(int j = 0; j < M; j += 4)
            for(int k1 = 0; k1 < 5; k1++) {
                for(int k2 = 0; k2 < 4; k2++)
                    c[k2] = A[i + k1][j + k2];
                for(int k2 = 0; k2 < 4; k2++)
                    B[j + k2][i + k1] = c[k2];
            }
    //
    for(int i = 0; i < n; i++)
        for(int j = m; j < M; j++)
            B[j][i] = A[i][j];
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
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

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

