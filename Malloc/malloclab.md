# Malloc Lab: Writing a Dynamic Storage Allocator

# 1. Introduction

In this lab you will be writing a dynamic storage allocator for C programs, i.e., your own version of the `malloc`, `free` and `realloc` routines. You are encouraged to explore the design space creatively and implement an allocator that is correct, efficient and fast.

在这个实验中，你将为 C 程序编写一个动态存储分配器，即你自己的版本的 `malloc`、`free` 和 `realloc` 函数。
我们鼓励你创造性地探索设计空间，并实现一个正确、高效和快速的分配器。

# 2. Logistics

You may work in a group of up to two people. Any clarifications and revisions to the assignment will be posted on the course Web page.

# 3. Hand Out Instructions

[ITE-SPECIFIC: Insert a paragraph here that explains how students should download
the `malloclab-handout.tar` file]

Start by copying `malloclab-handout.tar` to a protected directory in which you plan to do your work. Then give the command: `tar xvf malloclab-handout.tar`. This will cause a number of files to be unpacked into the directory. 
The only file you will be modifying and handing in is `mm.c`. 
The `mdriver.c` program is a driver program that allows you to evaluate the performance of your solution. 
Use the command make to generate the driver code and run it with the command `./mdriver -V`. (The `-V` flag displays helpful summary information.)

Looking at the file `mm.c` you’ll notice a C structure `team` into which you should insert the requested identifying information about the one or two individuals comprising your programming team. 
*Do this right away so you don’t forget.*

When you have completed the lab, you will hand in only one file (`mm.c`), which contains your solution.


首先，将 `malloclab-handout.tar` 复制到你计划工作的保护目录中。然后执行命令：`tar xvf malloclab-handout.tar`。这将导致许多文件解压到目录中。
你唯一需要修改并提交的文件是 `mm.c`。

`mdriver.c` 程序是一个驱动程序，允许你评估你的解决方案的性能。
使用 make 命令生成驱动代码，并使用命令 `./mdriver -V` 运行它。（`-V` 标志显示有用的总结信息。）

当你完成实验时，你只需提交一个包含你解决方案的文件（`mm.c`）。

# 4. How to Work on the Lab

Your dynamic storage allocator will consist of the following four functions, which are declared in `mm.h` and defined in `mm.c`.

~~~c
int mm_init(void);
void *mm_malloc(size_t size);
void mm_free(void *ptr);
void *mm_realloc(void *ptr, size_t size);
~~~

The `mm.c` file we have given you implements the simplest but still functionally correct malloc package that we could think of. 
Using this as a starting place, modify these functions (and possibly define other private static functions), so that they obey the following semantics:

你的动态存储分配器将由在 `mm.h` 中声明并在 `mm.c` 中定义的以下四个函数组成。

~~~c
int mm_init(void);
void *mm_malloc(size_t size);
void mm_free(void *ptr);
void *mm_realloc(void *ptr, size_t size);
~~~

我们提供给你的 `mm.c` 文件实现了我们能想到的最简单但仍然功能正确的 malloc 包。
使用这个作为起点，修改这些函数（可能还要定义其他私有静态函数），使它们遵守以下语义：

~~~c
int mm_init(void);
~~~
- `mm_init`: Before calling `mm_malloc` or `mm_free`, the application program (i.e., the trace-driven driver program that you will use to evaluate your implementation) calls `mm_init` to perform any necessary initializations, such as allocating the initial heap area. The return value should be `-1` if there was a problem in performing the initialization, `0` otherwise.

- 在调用 `mm_malloc` 或 `mm_free` 之前，应用程序（即，你将用来评估你的实现的基于跟踪的驱动程序）调用 `mm_init` 来执行任何必要的初始化，例如分配初始堆区域。如果在执行初始化过程中出现问题，返回值应为 `-1`，否则为 `0`。


~~~c
void *mm_malloc(size_t size);
~~~
- `mm_malloc`: The `mm_malloc` routine returns a pointer to an allocated block payload of at least size bytes. The entire allocated block should lie within the heap region and should not overlap with any other allocated chunk.

   We will comparing your implementation to the version of `malloc` supplied in the standard C library (`libc`). Since the `libc` malloc always returns payload pointers that are aligned to 8 bytes, your malloc implementation should do likewise and always return 8-byte aligned pointers

函数返回一个指向至少 `size` 字节的已分配块有效负载的指针。整个已分配的块应位于堆区域内，并且不应与任何其他已分配块重叠。
我们将把你的实现与标准 C 库（`libc`）中提供的 `malloc` 版本进行比较。
由于 `libc` malloc 始终返回对齐到 8 字节的有效负载指针，你的 malloc 实现也应如此，并始终返回 8 字节对齐的指针。


~~~c
void mm_free(void *ptr);
~~~
- `mm_free`: The `mm_free` routine frees the block pointed to by `ptr`. It returns nothing. This routine is only guaranteed to work when the passed pointer (`ptr`) was returned by an earlier call to `mm_malloc` or `mm_realloc` and has not yet been freed.

`mm_free` 函数释放 `ptr` 指向的块。它不返回任何内容。只有当传递的指针（`ptr`）是之前通过 `mm_malloc` 或 `mm_realloc` 调用返回的，并且尚未被释放时，此例程才保证工作。

~~~c
void *mm_realloc(void *ptr, size_t size);
~~~
- `mm_realloc`: The `mm_realloc` routine returns a pointer to an allocated region of at least `size` bytes with the following constraints.
   1. if `ptr` is NULL, the call is equivalent to `mm_malloc(size)`;
   2. if `size` is equal to zero, the call is equivalent to `mm_free(ptr)`;
   3. if `ptr` is not NULL, it must have been returned by an earlier call to `mm_malloc` or `mm_realloc`. The call to `mm_realloc` changes the size of the memory block pointed to by `ptr` (the *old block*) to `size` bytes and returns the address of the new block. Notice that the address of the new block might be the same as the old block, or it might be different, depending on your implementation, the amount of internal fragmentation in the old block, and the `size` of the `realloc` request.
   The contents of the new block are the same as those of the old `ptr` block, up to the minimum of the old and new sizes. Everything else is uninitialized. For example, if the old block is 8 bytes and the new block is 12 bytes, then the first 8 bytes of the new block are identical to the first 8 bytes of the old block and the last 4 bytes are uninitialized. Similarly, if the old block is 8 bytes and the new block is 4 bytes, then the contents of the new block are identical to the first 4 bytes of the old block

- `mm_realloc`：`mm_realloc` 函数返回一个指向至少 `size` 字节的已分配区域的指针，其约束条件如下。
   1. 如果 `ptr` 为空，则调用等同于 `mm_malloc(size)`；
   2. 如果 `size` 等于零，则调用等同于 `mm_free(ptr)`；
   3. 如果 `ptr` 不为空，它必须是之前通过 `mm_malloc` 或 `mm_realloc` 调用返回的。`mm_realloc` 调用更改 `ptr` 指向的旧块的大小（旧块）为 `size` 字节，并返回新块的地址。新块的地址可能与旧块相同，也可能不同，这取决于你的实现、旧块中的内部碎片以及 `realloc` 请求的 `size`。
   新块的内容与旧的 `ptr` 块相同，直到旧的和新的大小中的较小者为止。其他一切都是未初始化的。
   例如，如果旧块是 8 字节，新块是 12 字节，那么新块的前 8 字节与旧块的前 8 字节相同，最后 4 字节未初始化。同样，如果旧块是 8 字节，新块是 4 字节，那么新块的内容与旧块的前 4 字节相同。


These semantics match the the semantics of the corresponding `libc malloc`, `realloc`, and `free` routines. Type man malloc to the shell for complete documentation

这些语义与相应的 `libc malloc`、`realloc` 和 `free` 例程的语义相匹配。输入 man malloc 到 shell 中以获取完整文档。

# 5 Heap Consistency Checker

Dynamic memory allocators are notoriously tricky beasts to program correctly and efficiently. They are difficult to program correctly because they involve a lot of untyped pointer manipulation. 

You will find it very helpful to write a heap checker that scans the heap and checks it for consistency

Some examples of what a heap checker might check are:

- Is every block in the free list marked as free?
- Are there any contiguous free blocks that somehow escaped coalescing?
- Is every free block actually in the free list?
- Do the pointers in the free list point to valid free blocks?
- Do any allocated blocks overlap?
- Do the pointers in a heap block point to valid heap addresses?

Your heap checker will consist of the function int `mm_check(void)` in `mm.c`. It will check any invariants or consistency conditions you consider prudent. It returns a nonzero value if and only if your heap is consistent. You are not limited to the listed suggestions nor are you required to check all of them. You are encouraged to print out error messages when `mm_check` fails.

This consistency checker is for your own debugging during development. When you submit `mm.c`, make sure to remove any calls to `mm_check` as they will slow down your throughput. Style points will be given for your mm_check function. Make sure to put in comments and document what you are checking.

# 6. Support Routines
The memlib.c package simulates the memory system for your dynamic memory allocator. You can invoke the following functions in `memlib.c`:

- `void *mem sbrk(int incr)` Expands the heap by incr bytes, where incr is a positive non-zero integer and returns a generic pointer to the first byte of the newly allocated heap area. The semantics are identical to the Unix `sbrk` function, except that `mem_sbrk` accepts only a positive non-zero integer argument.

- `void *mem_heap_lo(void)` Returns a generic pointer to the first byte in the heap.

- `void *mem_heap_hi(void)` Returns a generic pointer to the last byte in the heap.

- `size_t mem_heapsize(void)` Returns the current size of the heap in bytes.

- `size_t mem_pagesize(void)` Returns the system’s page size in bytes (4K on Linux systems).

# 7. The Trace-driven Driver Program

The driver program `mdriver.c` in the `malloclab-handout.tar` distribution tests your `mm.c` pack-age for correctness, space utilization, and throughput. 

The driver program is controlled by a set of `trace files` that are included in the `malloclab-handout.tar` distribution. Each trace file contains a sequence of allocate, reallocate, and free directions that instruct the driver to call your `mm_malloc`, `mm_realloc`, and `mm_free` routines in some sequence. 

The driver and the trace files are the same ones we will use when we grade your handin `mm.c` file

The driver `mdriver.c` accepts the following command line arguments:

- `-t <tracedir>`: Look for the default trace files in directory `tracedir` instead of the default directory defined in `config.h`

- `-f <tracefile>`: Use one particular `tracefile` for testing instead of the default set of trace-files.

- `-h` Print a summary of the command line arguments

- `-l` Run and measure `libc` malloc in addition to the student’s malloc package.

- `-v` Verbose output. Print a performance breakdown for each tracefile in a compact table.

- `-V` More verbose output. Prints additional diagnostic information as each trace file is processed. Useful during debugging for determining which trace file is causing your malloc package to fail.

`malloclab-handout.tar` 分发中的驱动程序 `mdriver.c` 测试你的 `mm.c` 包的正确性、空间利用率和吞吐量。

驱动程序由一组包含在 `malloclab-handout.tar` 分发中的 `trace files` 控制。每个跟踪文件包含一系列分配、重新分配和释放指令，指示驱动程序按某种顺序调用你的 `mm_malloc`、`mm_realloc` 和 `mm_free` 例程。

评估你的提交的 `mm.c` 文件时，我们将使用相同的驱动程序和跟踪文件。

驱动程序 `mdriver.c` 接受以下命令行参数：

- `-t <tracedir>`：在 `tracedir` 目录中查找默认跟踪文件，而不是 `config.h` 中定义的默认目录。

- `-f <tracefile>`：使用一个特定的 `tracefile` 进行测试，而不是默认的一组跟踪文件。

- `-h`：打印命令行参数的概要。

- `-l`：运行并测量 `libc` malloc 以及学生的 malloc 包。

- `-v`：详细输出。以紧凑表格形式打印每个跟踪文件的性能分解。

- `-V`：更详细的输出。在处理每个跟踪文件时打印额外的诊断信息。在调试过程中非常有用，用于确定哪个跟踪文件导致你的 malloc 包失败。

# 8. Programming Rules

- You should not change any of the interfaces in mm.c.

- You should not invoke any memory-management related library calls or system calls. This excludes the use of `malloc`, `calloc`, `free`, `realloc`, `sbrk`, `brk` or any variants of these calls in your code

- You are not allowed to define any `global` or `static` compound data structures such as arrays, structs, trees, or lists in your `mm.c` program. However, you are allowed to declare global scalar variables such as integers, floats, and pointers in `mm.c`.

- For consistency with the `libc malloc` package, which returns blocks aligned on 8-byte boundaries, your allocator must always return pointers that are aligned to 8-byte boundaries. The driver will enforce this requirement for you.

# 9. Evaluation

1. Correctness
2. Performance
3. Style

# 10. Handin Instructions

# 11. Hints
Development Tips for Efficient Memory Allocator Implementation:

1. **Initial Development with Tiny Trace Files:**
   - Utilize the `mdriver -f` option.
   - Begin with tiny trace files to simplify debugging and testing, such as `short1,2-bal.rep`.

2. **Verbose Output for Debugging:**
   - Use `mdriver -v` for a detailed summary of each trace file.
   - `mdriver -V` indicates when each trace file is read, aiding in error isolation.

3. **Debugging with GCC and Debugger:**
   - Compile using `gcc -g`.
   - Employ a debugger to identify out-of-bounds memory references.

4. **Understanding Malloc Implementation:**
   - Thoroughly study the malloc implementation in the textbook.
   - The textbook provides an example of a simple allocator based on an implicit free list.

5. **Pointer Arithmetic Encapsulation:**
   - Encapsulate pointer arithmetic in C preprocessor macros to reduce complexity.
   - Refer to the textbook for macro examples.

6. **Implementation in Stages:**
   - Start with implementing malloc and free, working efficiently on the first 9 traces.
   - Then focus on realloc implementation, initially building on malloc and free.

7. **Performance Optimization with Profiler:**
   - Utilize tools like `gprof` for optimizing performance.

8. **Start Early and Be Persistent:**
   - Writing an efficient malloc package is challenging but rewarding.
   - Begin early and persevere for success.

Good luck!