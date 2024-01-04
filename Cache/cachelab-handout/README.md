This is the handout directory for the CS:APP Cache Lab. 

************************
Running the autograders:
************************

Before running the autograders, compile your code:
    linux> make

Check the correctness of your simulator:
    linux> ./test-csim

Check the correctness and performance of your transpose functions:
    linux> ./test-trans -M 32 -N 32
    linux> ./test-trans -M 64 -N 64
    linux> ./test-trans -M 61 -N 67

Check everything at once (this is the program that your instructor runs):
    linux> ./driver.py    

******
Files:
******

# You will modifying and handing in these two files
csim.c       Your cache simulator
trans.c      Your transpose function

# Tools for evaluating your simulator and transpose function
Makefile     Builds the simulator and tools
README       This file
driver.py*   The driver program, runs test-csim and test-trans
cachelab.c   Required helper functions
cachelab.h   Required header file
csim-ref*    The executable reference cache simulator
test-csim*   Tests your cache simulator
test-trans.c Tests your transpose function
tracegen.c   Helper program used by test-trans
traces/      Trace files used by test-csim.c

这是 CS:APP 缓存实验的发放目录。

************************
运行自动评分程序：
************************

在运行自动评分程序之前，编译你的代码：
    linux> make

检查你的模拟器的正确性：
    linux> ./test-csim

检查你的转置函数的正确性和性能：
    linux> ./test-trans -M 32 -N 32
    linux> ./test-trans -M 64 -N 64
    linux> ./test-trans -M 61 -N 67

一次性检查所有内容（这是你的教师运行的程序）：
    linux> ./driver.py    

******
文件：
******

# 你将修改并提交这两个文件
csim.c       你的缓存模拟器
trans.c      你的转置函数

# 用于评估你的模拟器和转置函数的工具
Makefile     构建模拟器和工具
README       本文件
driver.py*   驱动程序，运行 test-csim 和 test-trans
cachelab.c   需要的辅助函数
cachelab.h   需要的头文件
csim-ref*    可执行的参考缓存模拟器
test-csim*   测试你的缓存模拟器
test-trans.c 测试你的转置函数
tracegen.c   test-trans 使用的辅助程序
traces/      test-csim.c 使用的跟踪文件