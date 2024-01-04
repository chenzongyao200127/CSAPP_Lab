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

// Function prototypes
void trans(int M, int N, int A[N][M], int B[M][N]);
void trans_32x32(int M, int N, int A[N][M], int B[M][N]);
void trans_61x67(int M, int N, int A[N][M], int B[M][N]);
void trans_64x64(int M, int N, int A[N][M], int B[M][N]);

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
    switch (M)
    {
    case 32:
        if (N == 32)
        {
            trans_32x32(M, N, A, B);
            return;
        }
    case 61:
        if (N == 67)
        {
            trans_61x67(M, N, A, B);
            return;
        }
    case 64:
        if (N == 64)
        {
            trans_64x64(M, N, A, B);
            return;
        }
    default:
        trans(M, N, A, B);
    }
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

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

// Description: Optimized matrix transposition for a 32x32 size matrix
char trans_32x32_desc[] = "一个针对32x32大小矩阵优化的转置";
// Function 4 (5 total)
// Step 1: Validating and generating memory traces
// Step 2: Evaluating performance (s=5, E=1, b=5)
// func 4 (一个针对32x32大小矩阵优化的转置): hits:1765, misses:288, evictions:256

void trans_32x32(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, row, col;
    const int blockSize = 8; // Optimized block size for 32x32 matrix

    for (row = 0; row < N; row += blockSize)
    {
        for (col = 0; col < M; col += blockSize)
        {
            for (i = row; i < row + blockSize; i++)
            {
                for (j = col; j < col + blockSize; j++)
                {
                    // Skip diagonal elements to avoid modifying A
                    if (i != j)
                    {
                        B[j][i] = A[i][j];
                    }
                }
                // Handling diagonal element separately
                if (row == col)
                {
                    B[i][i] = A[i][i];
                }
            }
        }
    }
}

// Description: Optimized matrix transposition for a 64x64 matrix
char trans_64x64_desc[] = "一个针对64x64大小矩阵优化的转置";

void trans_64x64(int M, int N, int A[N][M], int B[M][N])
{
    int blockSize = 32; // 子矩阵大小是32x32

    for (int i = 0; i < N; i += blockSize)
    {
        for (int j = 0; j < M; j += blockSize)
        {
            int(*subA)[blockSize] = (int(*)[blockSize]) & A[i][j];

            int newI = j;
            int newJ = i;
            int(*subB)[blockSize] = (int(*)[blockSize]) & B[newI][newJ];

            trans_32x32(blockSize, blockSize, subA, subB);
        }
    }
}

// Description: Optimized matrix transposition for a 61x67 size matrix
char trans_61x67_desc[] = "一个针对61x67大小矩阵优化的转置";
// Function 2 (5 total)
// Step 1: Validating and generating memory traces
// Step 2: Evaluating performance (s=5, E=1, b=5)
// func 2 (一个针对61x67大小矩阵优化的转置): hits:6186, misses:1993, evictions:1961
void trans_61x67(int M, int N, int A[N][M], int B[M][N])
{
    // Loop variables for iterating through the matrix
    int i, j, row, col;

    // Block size used for optimizing the transposition process
    const int blockSize = 16;

    // Outer loops iterate over the matrix in blocks of size 'blockSize'
    for (row = 0; row < N; row += blockSize)
    {
        for (col = 0; col < M; col += blockSize)
        {
            // Inner loops iterate within each block
            for (i = row; i < row + blockSize && i < N; i++)
            {
                for (j = col; j < col + blockSize && j < M; j += 4)
                {
                    // Prefetching to improve cache performance
                    __builtin_prefetch(&A[i][j + 8], 0, 0); // Prefetch next elements in row of A
                    __builtin_prefetch(&B[j][i], 1, 0);     // Prefetch next elements in column of B

                    // Transposing elements from A to B
                    B[j][i] = A[i][j];

                    // Unrolling loop to copy next three elements if within bounds
                    if (j + 1 < col + blockSize && j + 1 < M)
                        B[j + 1][i] = A[i][j + 1];
                    if (j + 2 < col + blockSize && j + 2 < M)
                        B[j + 2][i] = A[i][j + 2];
                    if (j + 3 < col + blockSize && j + 3 < M)
                        B[j + 3][i] = A[i][j + 3];
                }
            }
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

    registerTransFunction(trans_61x67, trans_61x67_desc);

    registerTransFunction(trans_64x64, trans_64x64_desc);

    registerTransFunction(trans_32x32, trans_32x32_desc);
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
