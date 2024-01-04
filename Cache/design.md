Valgrind 内存跟踪的格式如下：

I 0400d7d4,8
  M 0421c7f0,4
  L 04f6b868,8
  S 7ff0005c8,8

每个 “I” 前面从不有空格。每个 “M”、“L” 和 “S” 前面总是有一个空格。
地址字段指定了一个 64 位的十六进制内存地址。大小字段指定操作访问的字节数。  

Usage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>
• -h: Optional help flag that prints usage info
• -v: Optional verbose flag that displays trace info
• -s <s>: Number of set index bits (S = 2^s is the number of sets)
• -E <E>: Associativity (number of lines per set)
• -b <b>: Number of block bits (B = 2^b is the block size)
• -t <tracefile>: Name of the valgrind trace to replay

./csim-ref -v -s 4 -E 1 -b 4 -t traces/yi.trace
[s, E, b, m] m = 64 位机器
~~~shell
[ t 位 ] [ s 位 ] [ b 位 ]
~~~

1. 需要设计一个模块进行 Valgrind 内存跟踪输出指令的解析
2. 输入参数的解析 getopt模块
3. 模拟cache的数据结构（有效位，标记位，block）需要使用 malloc
4. 模拟组选择 + 行匹配 + 块抽取
5. 当选择要驱逐哪个缓存行时，需要使用最近最少使用（LRU）替换策略。如何模拟LRU？(hashkey 标记位 + 双向链表?)
6. 最终要统计 hits misses evictions
7. 在此实验中，假设内存访问是正确对齐的，因此单个内存访问永远不会跨越块边界。通过做出这个假设，可以忽略 valgrind 跟踪中的请求大小。
8. 写策略：write through + no write allocate
9. 由于不需要模拟具体的写入写出，高速缓存块的具体内容是不需要模拟的（只要利用有效位 + 标记 即可确定是否hit or miss）
10. 每个组是一个容量为E行的双向链表+hashmap (每行的struct需要运行时确定)，用于模拟LRU
11. 高速缓存大小由运行时分配的组数组组成
12. cache 初始化全为 0
13. 详细的错误处理+日志
   
具体模块：
1. 输入参数解析
2. cache 数据结构
3. LRU算法模拟
4. hits misses evictions verbose输出
5. Valgrind 内存跟踪输出指令解析


example
~~~shell
linux> ./csim-ref -v -s 4 -E 1 -b 4 -t traces/yi.trace
L 10,1 miss
M 20,1 miss hit
L 22,1 hit
S 18,1 hit
L 110,1 miss eviction
L 210,1 miss eviction
M 12,1 miss eviction hit
hits:4 misses:5 evictions:3
~~~

(base) czy-ThinkCentre Cache/cachelab-handout ‹master*› » ./test-csim
                        Your simulator     Reference simulator
Points (s,E,b)    Hits  Misses  Evicts    Hits  Misses  Evicts
     3 (1,1,1)       9       8       6       9       8       6  traces/yi2.trace
     3 (4,2,4)       4       5       2       4       5       2  traces/yi.trace
     3 (2,1,4)       2       3       1       2       3       1  traces/dave.trace
     3 (2,1,3)     167      71      67     167      71      67  traces/trans.trace
     3 (2,2,3)     201      37      29     201      37      29  traces/trans.trace
     3 (2,4,3)     212      26      10     212      26      10  traces/trans.trace
     3 (5,1,5)     231       7       0     231       7       0  traces/trans.trace
     6 (5,1,5)  265189   21775   21743  265189   21775   21743  traces/long.trace
    27

TEST_CSIM_RESULTS=27