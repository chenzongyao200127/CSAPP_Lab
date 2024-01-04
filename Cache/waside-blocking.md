# CS:APP2e Web Aside MEM:BLOCKING: Using Blocking to Increase Temporal Locality

## Notice
The material in this document is supplementary material to the book Computer Systems, A Programmer’s Perspective, Second Edition, by Randal E. Bryant and David R. O’Hallaron, published by Prentice-Hall and copyrighted 2011. In this document, all references beginning with “CS:APP2e ” are to this book. 

More information about the book is available at csapp.cs.cmu.edu. 

This document is being made available to the public, subject to copyright provisions. You are free to copy and distribute it, but you should not use any of this material without attribution.


## 1 Introduction

There is an interesting technique called `blocking` that can improve the temporal locality of inner loops. The general idea of blocking is to organize the data structures in a program into large chunks called blocks. (In this context, “block” refers to an application-level chunk of data, not to a cache block.) The program is structured so that it loads a chunk into the L1 cache, does all the reads and writes that it needs to on that chunk, then discards the chunk, loads in the next chunk, and so on.

这里有一种有趣的技术，叫做`分块`，它可以提高内部循环的时间局部性。分块的基本思想是将程序中的数据结构组织成大块的数据块。在这里，“块”指的是应用级的数据块，而不是缓存块。程序被结构化为先将一个数据块加载到L1缓存中，完成对该块的所有读写操作，然后丢弃该块，加载下一个块，依此类推。

Unlike the simple loop transformations for improving spatial locality, blocking makes the code harder to read and understand. For this reason, it is best suited for optimizing compilers or frequently executed library routines. Still, the technique is interesting to study and understand because it is a general concept that can produce big performance gains on some systems

与简单的循环转换用于提高空间局部性不同，分块使代码更难阅读和理解。因此，它最适合用于优化编译器或频繁执行的库例程。尽管如此，研究和理解这种技术仍然很有趣，因为它是一个通用概念，可以在一些系统上带来巨大的性能提升。


1. **技术名称：分块（Blocking）**
   - 这是一种编程技术，用于提高计算机程序的效率。

2. **目的：提高时间局部性**
   - “时间局部性”是指程序在执行时频繁访问某些数据。如果一个数据项被用到，那么在不久的将来很可能会再次用到。分块技术通过改变数据的组织方式来利用这一点，以提高效率。

3. **如何操作：组织数据成大块**
   - 在分块技术中，你把数据结构（如数组、矩阵等）划分成较大的“块”或“区块”。这里的“块”指的是较大的数据集合，不同于计算机缓存系统中的小块。

4. **程序结构的改变**
   - 程序被重新设计，以便一次处理一个这样的数据块。首先，它加载一个数据块到L1缓存（这是计算机处理器内部的一个非常快速的存储区域）。然后，程序对这个块进行所有必要的读取和写入操作。完成后，这个数据块被丢弃，程序加载下一个块，以此类推。

5. **优势与劣势**
   - 优势：这种方法可以显著提高某些系统的性能，特别是在处理大量数据时。
   - 劣势：它使代码更难以阅读和理解。因为数据不是按照简单、直接的方式处理，而是分成块，需要更复杂的逻辑来控制。

6. **适用场景**
   - 由于其复杂性，分块技术通常不适用于普通程序。它更适合用于那些需要高效率的场合，比如编译器的优化过程或者是被频繁调用的库函数。


## 2 A Blocked Version of Matrix Multiply

好的，让我们用矩阵乘法来具体说明如何使用分块技术进行优化。

### 基本概念

在没有使用分块技术的标准矩阵乘法中，我们将两个矩阵 A 和 B 相乘以得到矩阵 C。如果 A 是一个 \( m \times n \) 矩阵，B 是一个 \( n \times p \) 矩阵，那么结果矩阵 C 将是 \( m \times p \) 的。计算 C 的每个元素需要遍历 A 的一行和 B 的一列。

### 分块技术应用

在分块技术中，我们将矩阵 A 和 B 划分为更小的子矩阵（或块）。这些块的大小通常选择以便它们可以有效地装入缓存中。这样，当我们对这些小块进行操作时，它们可以保持在快速访问的缓存中，减少访问主内存的次数。

### 具体示例

假设我们有两个 4x4 的矩阵 A 和 B，并且我们将它们分成 2x2 的子矩阵。这意味着每个矩阵会被分为四个块：

\[ A = \begin{bmatrix} A_{11} & A_{12} \\ A_{21} & A_{22} \end{bmatrix}, \quad B = \begin{bmatrix} B_{11} & B_{12} \\ B_{21} & B_{22} \end{bmatrix} \]

每个 \( A_{ij} \) 和 \( B_{ij} \) 是一个 2x2 的子矩阵。计算结果矩阵 C 的分块也是类似的：

\[ C = \begin{bmatrix} C_{11} & C_{12} \\ C_{21} & C_{22} \end{bmatrix} \]

其中每个 \( C_{ij} \) 也是 2x2 的。对于每个 \( C_{ij} \) 的计算，我们使用下面的公式：

\[ C_{ij} = A_{i1}B_{1j} + A_{i2}B_{2j} \]

例如，为了计算 \( C_{11} \)，我们执行：

\[ C_{11} = A_{11}B_{11} + A_{12}B_{21} \]

### 优势

使用分块技术的优势在于：

1. **改善缓存利用**：因为子矩阵更可能完整地保留在快速的缓存中，从而减少内存访问延迟。
2. **并行化潜力**：不同的块可以在不同的处理器核心上同时处理，提高并行性。

### 注意事项

- 分块大小的选择对性能有重大影响。块太大或太小都可能不理想。
- 这种方法增加了编程的复杂性，因为需要额外的逻辑来处理块的分割和组合。

分块技术通过优化数据访问模式来提高矩阵乘法的性能，这在大型矩阵操作中尤其有效。
然而，它需要仔细的设计和测试，以确保选择最适合特定应用和硬件的块大小。

--------------

对矩阵乘法进行分块处理是通过将矩阵分割成子矩阵，然后利用这些子矩阵可以像标量一样操作的数学事实。例如，假设我们要计算 C = AB，其中 A、B 和 C 都是 8×8 矩阵。那么我们可以将每个矩阵分割成四个 4×4 的子矩阵：
\[ \begin{bmatrix} C_{11} & C_{12} \\ C_{21} & C_{22} \end{bmatrix} = \begin{bmatrix} A_{11} & A_{12} \\ A_{21} & A_{22} \end{bmatrix} \begin{bmatrix} B_{11} & B_{12} \\ B_{21} & B_{22} \end{bmatrix} \]
其中
\[ C_{11} = A_{11}B_{11} + A_{12}B_{21} \]
\[ C_{12} = A_{11}B_{12} + A_{12}B_{22} \]
\[ C_{21} = A_{21}B_{11} + A_{22}B_{21} \]
\[ C_{22} = A_{21}B_{12} + A_{22}B_{22} \]

图 1 展示了一个分块矩阵乘法的版本，我们称之为 bijk 版本。这段代码背后的基本思想是将 A 和 C 分割成 1×bsize 的行片段，并将 B 分割成 bsize×bsize 的块。最内层的 (j, k) 循环对 A 的一个片段和 B 的一个块进行相乘，并将结果累加到 C 的一个片段中。i 循环遍历 A 和 C 的 n 个行片段，使用 B 中的相同块。

图 2 提供了图 1 中分块代码的图形解释。*关键思想是它将 B 的一个块加载到缓存中，使用完后再丢弃它。*
对 A 的引用具有良好的空间局部性，因为每个片段都是以步长 1 进行访问的。也有良好的时间局部性，因为整个片段会连续被引用 bsize 次。对 B 的引用具有良好的时间局部性，因为整个 bsize×bsize 块会连续被访问 n 次。最后，对 C 的引用具有良好的空间局部性，因为片段中的每个元素都是连续写入的。注意，对 C 的引用没有良好的时间局部性，因为每个片段只被访问一次。

分块可以使代码更难阅读，但它也可以带来巨大的性能提升。图 3 展示了在 Pentium III Xeon 系统上（bsize = 25）两个版本的分块矩阵乘法的性能。注意到分块使运行时间比最佳非分块版本快了大约两倍，从大约每次迭代 20 个周期降低到大约每次迭代 10 个周期。关于分块的另一个有趣之处是，随着数组大小的增加，每次迭代的时间几乎保持不变。对于小数组大小，分块版本的额外开销导致它比非分块版本运行得更慢。在大约 n = 100 时，存在一个交叉点，之后分块版本运行得更快。

我们提醒，分块矩阵乘法并不是在所有系统上都能提高性能。例如，在 Core i7 系统上，存在与最佳分块版本性能相同的非分块矩阵乘法版本。

~~~c
1 void bijk(array A, array B, array C, int n, int bsize)
2 {
3     int i, j, k, kk, jj;
4     double sum;
5     int en = bsize * (n/bsize); /* 计算块大小能整除的部分 */

6     // 初始化结果矩阵C
7     for (i = 0; i < n; i++)
8         for (j = 0; j < n; j++)
9             C[i][j] = 0.0;

10     // 遍历A和B矩阵的块
11     for (kk = 0; kk < en; kk += bsize) {
12         for (jj = 0; jj < en; jj += bsize) {
13             // 对每个块进行矩阵乘法
14             for (i = 0; i < n; i++) {
15                 for (j = jj; j < jj + bsize; j++) {
16                     sum = C[i][j];
17                     // 计算当前块的乘积并累加到sum
18                     for (k = kk; k < kk + bsize; k++) {
19                         sum += A[i][k]*B[k][j];
20                     }
21                     // 更新C矩阵的当前元素
22                     C[i][j] = sum;
23                 }
24             }
25         }
26     }
27 }
~~~

Figure 1: Blocked matrix multiply. A simple version that assumes that the array size (n) is an integral multiple of the block size (bsize)

