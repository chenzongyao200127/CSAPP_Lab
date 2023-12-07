# Heap Checker 
- Part of the assignment is writing a heap checker
    1. This is here to help you.
    2. Write the heap checker as you go, don't think of it as something to doat the end
    3. A good heap checker will make debugging much, much easier
- Heap checker tips
    1. Heap checker should run silently until it finds an error
        - Otherwise you will get more output than is useful
        - You might find it useful to add a “verbose” flag, however
    2. Consider using a macro to turn the heap checker on and off
        - This way you don't have to edit all of the places you call it
    3. There is a built-in macro called `__LINE__` that gets replaced withthe line number it's on
        - You can use this to make the heap checker tell you where it failed

GPT-4:
Certainly! To complete the `mm_check` function, I'll provide a more detailed implementation with all the necessary checks based on a typical heap structure. Note that the exact implementation details may vary based on your specific memory management design, such as how you structure your heap blocks, maintain free lists, and manage heap boundaries.

### Example Implementation of `mm_check`

```c
#include <stdio.h>
#include <assert.h>

// Define your block structure and global variables here
// For example:
typedef struct block_t {
    size_t size;
    struct block_t *next_free;
    int is_free;
    // Other fields...
} block_t;

extern block_t *free_list_head; // Assuming you have a free list
extern block_t *heap_start; // Start of your heap
extern block_t *heap_end; // End of your heap

// Helper functions prototypes
// int is_in_free_list(block_t *block);
// block_t *get_next_block(block_t *block);
// int is_valid_block(block_t *block);
// int is_within_heap(void *ptr);

int mm_check(void) {
    int is_consistent = 1;

    // Check 1: Are all blocks in the free list marked as free?
    for (block_t *current = free_list_head; current != NULL; current = current->next_free) {
        if (!current->is_free) {
            fprintf(stderr, "Error: Block in free list not marked as free.\n");
            is_consistent = 0;
        }
    }

    // Check 2: Are there any contiguous free blocks that escaped coalescing?
    for (block_t *current = heap_start; current < heap_end; current = get_next_block(current)) {
        if (current->is_free) {
            block_t *next = get_next_block(current);
            if (next < heap_end && next->is_free) {
                fprintf(stderr, "Error: Contiguous free blocks not coalesced.\n");
                is_consistent = 0;
            }
        }
    }

    // Check 3: Is every free block actually in the free list?
    for (block_t *current = heap_start; current < heap_end; current = get_next_block(current)) {
        if (current->is_free && !is_in_free_list(current)) {
            fprintf(stderr, "Error: Free block not in free list.\n");
            is_consistent = 0;
        }
    }

    // Check 4: Do the pointers in the free list point to valid free blocks?
    for (block_t *current = free_list_head; current != NULL; current = current->next_free) {
        if (!is_valid_block(current) || !current->is_free) {
            fprintf(stderr, "Error: Invalid or non-free block in free list.\n");
            is_consistent = 0;
        }
    }

    // Check 5: Do any allocated blocks overlap?
    block_t *current = heap_start;
    while (current < heap_end) {
        block_t *next = get_next_block(current);
        if (!current->is_free && next < heap_end && !next->is_free && ((char*)current + current->size > (char*)next)) {
            fprintf(stderr, "Error: Overlapping allocated blocks.\n");
            is_consistent = 0;
        }
        current = next;
    }

    // Check 6: Do the pointers in a heap block point to valid heap addresses?
    for (current = heap_start; current < heap_end; current = get_next_block(current)) {
        if (!is_within_heap(current)) {
            fprintf(stderr, "Error: Block pointers not pointing to valid heap addresses.\n");
            is_consistent = 0;
        }
    }

    // Add more checks as per your heap implementation and invariants

    return is_consistent;
}
```

### Key Considerations

- **Customization**: Adapt the checks to align with your specific heap management techniques.
- **Helper Functions**: Functions like `is_in_free_list`, `get_next_block`, `is_valid_block`, and `is_within_heap` need to be defined based on your memory manager's logic.
- **Error Handling**: The error messages should be informative to aid in debugging.
- **Efficiency**: Consider the performance implications of these checks, especially if your heap becomes large.

This implementation assumes a certain structure for your heap and free blocks. You may need to adjust the checks and logic based on your actual implementation details.

# Implementation Hurdles
 - How do we know where the blocks are?
 - How do we know how big the blocks are?
 - How do we know which blocks are free?
 - Remember: can't buffer calls to malloc and free... must deal with them real-time.
 - Remember: calls to free only takes a pointer, not apointer and a size.
 - Solution: Need a data structure to store information onthe“blocks”
    1. Where do I keep this data structure?
    2. We can't allocate a space for it, that's what we are writing!

# My Block Struct
~~~shell
            31 ......  3  2 1 0
            [   块大小  ][ 0 0 a] (a = 1: 已分配的， a = 0: 空闲的)
   malloc ->[     pred         ](祖先)    ---+
            [     succ         ](后继)       |
            [      有效载荷     ]             原来的有效载荷
            [   只包括已分配的块 ]             |
            [                  ]         ---+    
            [------------------]    
            [-----填充(可选)----]   
            [------------------]
            [   块大小  ][ a / f]
PS: 块大小包括所有的有效载荷（payload）、任何必要的填充（padding）、以及头部（header）和脚部标记（footer）
~~~

设计思路 显式空闲链表 + LIFO + 首次适配 + 边界标记：
- **显式数据结构**：在这种方法中，空闲块被组织成一个显式的数据结构，如双向链表。这意味着每个空闲块都显式地包含了指向其他空闲块的指针，而不是隐式地通过块的大小或其他方式来确定。

- **LIFO**: 在这种方法中，当一个内存块被释放时，它被放置在空闲链表的开始处。这意味着最后被释放的块将是第一个被考虑用于新的分配请求的块。由于新释放的块可能仍然在缓存中，这可能提高了内存访问的效率。

- **First Fit** 这是一个常见的内存分配策略，指的是分配器在分配内存时会选择它遇到的第一个足够大的空闲块。结合LIFO顺序，这意味着分配器将首先检查最近释放的块，这些块更可能仍然在CPU的缓存中，从而提高访问速度。

- **释放块的常数时间操作**：在LIFO策略下，释放一个块仅涉及将其放置在空闲链表的头部。这是一个常数时间操作，因为它不依赖于堆的大小或空闲链表的长度。

- **使用边界标记的合并**：边界标记是一种内存管理技术，它在每个内存块的开始和结束处存储信息，如块的大小和分配状态。当一个块被释放时，分配器可以检查相邻的块，如果它们也是空闲的，则可以迅速将它们合并成一个更大的块。这种合并操作同样是常数时间内完成的，因为它仅涉及对几个边界标记的更新，而不需要遍历整个堆或链表。

一些必须的操作：
`1. 操作空闲链表的基本常数和宏`
`2. 创建初始空闲链表`
3. 分配块
4. 双字对齐
5. 堆扩展
6. First Fit
7. 释放块
8. 放置块
9. 合并空闲块
10. `heap checker`
11. `realloc()`


# `__LINE__` Macro Usage

The `__LINE__` macro is a built-in preprocessor macro in C and C++ that expands to the current line number in the source code file. It's a part of standard predefined macros that are available in all C and C++ compilers. Here's how you can use it:

### Basic Usage

The `__LINE__` macro is commonly used for debugging purposes, where you want to include the line number in log messages or error reports. For example:

```c
printf("Error occurred at line number: %d\n", __LINE__);
```

This will print the line number where this `printf` statement appears in your source code.

### With `assert`

The `__LINE__` macro can be especially useful in conjunction with the `assert` macro for better diagnostics:

```c
assert(some_condition && "Failure on line " __LINE__);
```

### Logging

For logging purposes, you might define a macro that includes `__LINE__`:

```c
#define LOG(msg) printf("[%s:%d] %s\n", __FILE__, __LINE__, msg)

// Usage
LOG("This is a log message.");
```

This macro will print the file name and line number along with the log message.

### Error Handling

In error handling, `__LINE__` can be used to provide more detailed information about where an error occurred:

```c
if (error_condition) {
    fprintf(stderr, "Error at line %d\n", __LINE__);
    // Additional error handling
}
```

### Limitations and Considerations

- The `__LINE__` macro is replaced by the line number during preprocessing. It cannot be used to determine the line number dynamically at runtime.
- It's a part of the source code, so if your code gets moved around or line numbers change due to code modification, `__LINE__` will always represent the current line number in the source file.
- Overusing `__LINE__` (and other debug macros like `__FILE__`) can make the code harder to read and maintain, so it's best used judiciously, particularly in parts of code where tracking location is crucial (like error handling or logging).


# `size_t` Usage
`size_t` 是一种在 C 和 C++ 程序设计语言中广泛使用的数据类型。它是一个无符号的整型，通常用于表示大小或索引，比如数组的长度、字符串的长度或者用于循环计数器。`size_t` 的确切大小取决于程序运行的平台（操作系统和处理器架构），但它设计得足够大，可以容纳任何数组的大小。

在大多数现代平台上，`size_t` 通常与最常见的指针大小相同。例如，在 32 位系统上，`size_t` 通常是一个 32 位无符号整数，而在 64 位系统上，它通常是一个 64 位无符号整数。这样做的目的是确保 `size_t` 能够表示系统上任何可能的内存大小。使用 `size_t` 作为数组索引和大小的类型有助于提高代码的可移植性和安全性。