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
// reference：
// 1.https://zhuanlan.zhihu.com/p/387662272
// 2.https://www.zhihu.com/column/c_1480603406519238656
void trans_64x64(int M, int N, int A[N][M], int B[M][N])
{
    int a_0, a_1, a_2, a_3, a_4, a_5, a_6, a_7;

    // Loop over the matrix in 8x8 blocks to optimize cache usage.
    for (int i = 0; i < 64; i += 8)
    {
        for (int j = 0; j < 64; j += 8)
        {
            // Transpose the upper left 4x4 part of the current 8x8 block.
            for (int k = i; k < i + 4; k++)
            {
                // Load one row of the block into local variables.
                a_0 = A[k][j + 0];
                a_1 = A[k][j + 1];
                a_2 = A[k][j + 2];
                a_3 = A[k][j + 3];
                a_4 = A[k][j + 4];
                a_5 = A[k][j + 5];
                a_6 = A[k][j + 6];
                a_7 = A[k][j + 7];

                // Write the transposed row to B.
                B[j + 0][k] = a_0;
                B[j + 1][k] = a_1;
                B[j + 2][k] = a_2;
                B[j + 3][k] = a_3;

                // Store the elements that will be moved to the lower right 4x4 part of the block.
                B[j + 0][k + 4] = a_4;
                B[j + 1][k + 4] = a_5;
                B[j + 2][k + 4] = a_6;
                B[j + 3][k + 4] = a_7;
            }

            // Adjust the lower left and upper right 4x4 parts of the block.
            for (int k = j; k < j + 4; k++)
            {
                // Temporarily store the values from B that will be moved to the lower left 4x4 part.
                a_0 = B[k][i + 4];
                a_1 = B[k][i + 5];
                a_2 = B[k][i + 6];
                a_3 = B[k][i + 7];

                // Load the upper right 4x4 part of A.
                a_4 = A[i + 4][k];
                a_5 = A[i + 5][k];
                a_6 = A[i + 6][k];
                a_7 = A[i + 7][k];

                // Complete the transposition of the upper right 4x4 part.
                B[k][i + 4] = a_4;
                B[k][i + 5] = a_5;
                B[k][i + 6] = a_6;
                B[k][i + 7] = a_7;

                // Move the temporarily stored values to the lower left 4x4 part.
                B[k + 4][i + 0] = a_0;
                B[k + 4][i + 1] = a_1;
                B[k + 4][i + 2] = a_2;
                B[k + 4][i + 3] = a_3;
            }

            // Transpose the lower right 4x4 part of the current 8x8 block.
            for (int k = i + 4; k < i + 8; k++)
            {
                // Load one row of the block into local variables.
                a_4 = A[k][j + 4];
                a_5 = A[k][j + 5];
                a_6 = A[k][j + 6];
                a_7 = A[k][j + 7];

                // Write the transposed row to B.
                B[j + 4][k] = a_4;
                B[j + 5][k] = a_5;
                B[j + 6][k] = a_6;
                B[j + 7][k] = a_7;
            }
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
