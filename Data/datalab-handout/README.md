***********************
The CS:APP Data Lab
Directions to Students
***********************

Your goal is to modify your copy of bits.c so that it passes all the
tests in btest without violating any of the coding guidelines.


*********
0. Files:
*********

Makefile	- Makes btest, fshow, and ishow
README		- This file
bits.c		- The file you will be modifying and handing in
bits.h		- Header file
btest.c		- The main btest program
  btest.h	- Used to build btest
  decl.c	- Used to build btest
  tests.c       - Used to build btest
  tests-header.c- Used to build btest
dlc*		- Rule checking compiler binary (data lab compiler)	 
driver.pl*	- Driver program that uses btest and dlc to autograde bits.c
Driverhdrs.pm   - Header file for optional "Beat the Prof" contest
fshow.c		- Utility for examining floating-point representations
ishow.c		- Utility for examining integer representations

***********************************************************
1. Modifying bits.c and checking it for compliance with dlc
***********************************************************

IMPORTANT: Carefully read the instructions in the bits.c file before
you start. These give the coding rules that you will need to follow if
you want full credit.

Use the dlc compiler (./dlc) to automatically check your version of
bits.c for compliance with the coding guidelines:

       unix> ./dlc bits.c

dlc returns silently if there are no problems with your code.
Otherwise it prints messages that flag any problems.  Running dlc with
the -e switch:

    	unix> ./dlc -e bits.c  

causes dlc to print counts of the number of operators used by each function.

Once you have a legal solution, you can test it for correctness using
the ./btest program.

*********************
2. Testing with btest
*********************

The Makefile in this directory compiles your version of bits.c with
additional code to create a program (or test harness) named btest.

To compile and run the btest program, type:

    unix> make btest
    unix> ./btest [optional cmd line args]

You will need to recompile btest each time you change your bits.c
program. When moving from one platform to another, you will want to
get rid of the old version of btest and generate a new one.  Use the
commands:

    unix> make clean
    unix> make btest

Btest tests your code for correctness by running millions of test
cases on each function.  It tests wide swaths around well known corner
cases such as Tmin and zero for integer puzzles, and zero, inf, and
the boundary between denormalized and normalized numbers for floating
point puzzles. When btest detects an error in one of your functions,
it prints out the test that failed, the incorrect result, and the
expected result, and then terminates the testing for that function.

Here are the command line options for btest:

  unix> ./btest -h
  Usage: ./btest [-hg] [-r <n>] [-f <name> [-1|-2|-3 <val>]*] [-T <time limit>]
    -1 <val>  Specify first function argument
    -2 <val>  Specify second function argument
    -3 <val>  Specify third function argument
    -f <name> Test only the named function
    -g        Format output for autograding with no error messages
    -h        Print this message
    -r <n>    Give uniform weight of n for all problems
    -T <lim>  Set timeout limit to lim

Examples:

  Test all functions for correctness and print out error messages:
  unix> ./btest

  Test all functions in a compact form with no error messages:
  unix> ./btest -g

  Test function foo for correctness:
  unix> ./btest -f foo

  Test function foo for correctness with specific arguments:
  unix> ./btest -f foo -1 27 -2 0xf

Btest does not check your code for compliance with the coding
guidelines.  Use dlc to do that.

*******************
3. Helper Programs
*******************

We have included the ishow and fshow programs to help you decipher
integer and floating point representations respectively. Each takes a
single decimal or hex number as an argument. To build them type:

    unix> make

Example usages:

    unix> ./ishow 0x27
    Hex = 0x00000027,	Signed = 39,	Unsigned = 39

    unix> ./ishow 27
    Hex = 0x0000001b,	Signed = 27,	Unsigned = 27

    unix> ./fshow 0x15213243
    Floating point value 3.255334057e-26
    Bit Representation 0x15213243, sign = 0, exponent = 0x2a, fraction = 0x213243
    Normalized.  +1.2593463659 X 2^(-85)

    linux> ./fshow 15213243
    Floating point value 2.131829405e-38
    Bit Representation 0x00e822bb, sign = 0, exponent = 0x01, fraction = 0x6822bb
    Normalized.  +1.8135598898 X 2^(-126)



----------------------------------------------------------------------------------------------------


CS:APP 数据实验室
给学生的指导
***********************

你的目标是修改你的 `bits.c` 副本，使其通过 `btest` 中的所有测试，同时不违反任何编码指南。

*********
0. 文件：
*********

`Makefile` - 制作 btest、fshow 和 ishow
`README` - 本文件
`bits.c` - 你将要修改并提交的文件
`bits.h` - 头文件
`btest.c` - 主要的 btest 程序
  `btest.h` - 用于构建 btest
  `decl.c` - 用于构建 btest
  `tests.c` - 用于构建 btest
  `tests-header.c` - 用于构建 btest
`dlc*` - 规则检查编译器二进制文件（数据实验室编译器）
`driver.pl*` - 使用 btest 和 dlc 自动评分 bits.c 的驱动程序
`Driverhdrs.pm` - 可选的 "Beat the Prof" 比赛的头文件
`fshow.c` - 用于检查浮点表示的工具
`ishow.c` - 用于检查整数表示的工具

***********************************************************
1. 修改 bits.c 并检查其是否符合 dlc
***********************************************************

重要提示：在开始之前仔细阅读 bits.c 文件中的指令。这些指令给出了你需要遵循的编码规则，以获得全部学分。

使用 dlc 编译器 (./dlc) 自动检查你的 bits.c 版本是否符合编码指南：

       unix> ./dlc bits.c

如果你的代码没有问题，dlc 会静默返回。否则，它会打印出标记任何问题的消息。使用 -e 开关运行 dlc：

    	unix> ./dlc -e bits.c  

会使 dlc 打印出每个函数使用的操作符数量。

一旦你有了合法的解决方案，你可以使用 ./btest 程序测试其正确性。

*********************
2. 使用 btest 测试
*********************

此目录中的 Makefile 会将你的 bits.c 版本与其他代码编译在一起，创建一个名为 btest 的程序（或测试工具）。

要编译并运行 btest 程序，请输入：

    unix> make btest
    unix> ./btest [可选命令行参数]

每次更改 `bits.c` 程序时，你需要重新编译 btest。当从一个平台移动到另一个平台时，你会想要删除旧版本的 btest 并生成一个新版本。使用以下命令：

    unix> make clean
    unix> make btest

Btest 通过对每个函数运行数百万个测试用例来测试你的代码的正确性。
它针对整数谜题的 Tmin 和零等众所周知的角落案例，以及浮点谜题的零、无穷大和非规格化与规格化数字之间的边界进行广泛测试。
当 btest 检测到你的函数中有错误时，它会打印出失败的测试、不正确的结果和期望的结果，然后终止该函数的测试。

以下是 btest 的命令行选项：

  unix> ./btest -h
  使用方法：./btest [-hg] [-r <n>] [-f <name> [-1|-2|-3 <val>]*] [-T <时间限制>]
    -1 <val>  指定第一个函数参数
    -2 <val>  指定第二个函数参数
    -3 <val>  指定第三个函数参数
    -f <name> 仅测试命名的函数
    -g        格式化输出以适用于无错误消息的自动评分
    -h        打印此消息
    -r <n>    为所有问题赋予 n 的统一权重
    -T <lim>  将超时限制设置为 lim

示例：

  测试所有函数的正确性并打印出错误消息：
  unix>

 ./btest

  以紧凑形式测试所有函数，不打印错误消息：
  unix> ./btest -g

  测试函数 foo 的正确性：
  unix> ./btest -f foo

  使用特定参数测试函数 foo 的正确性：
  unix> ./btest -f foo -1 27 -2 0xf

Btest 不检查你的代码是否符合编码指南。使用 dlc 进行检查。

*******************
3. 辅助程序
*******************

我们包含了 ishow 和 fshow 程序来帮助你破译整数和浮点数表示。每个程序都接受一个十进制或十六进制数作为参数。要构建它们，请输入：

    unix> make

示例用法：

    unix> ./ishow 0x27
    十六进制 = 0x00000027,	有符号 = 39,	无符号 = 39

    unix> ./ishow 27
    十六进制 = 0x0000001b,	有符号 = 27,	无符号 = 27

    unix> ./fshow 0x15213243
    浮点数值 3.255334057e-26
    位表示 0x15213243, 符号 = 0, 指数 = 0x2a, 小数 = 0x213243
    规格化。+1.2593463659 X 2^(-85)

    linux> ./fshow 15213243
    浮点数值 2.131829405e-38
    位表示 0x00e822bb, 符号 = 0, 指数 = 0x01, 小数 = 0x6822bb
    规格化。+1.8135598898 X 2^(-126)
~~~

学生可以实现简单的逻辑函数、二进制补码和浮点函数，但必须使用 C 语言的一个高度受限的子集。
例如，可能会要求仅用位级运算和直线代码（`straightline code`）来计算一个数的绝对值。
该实验帮助学生理解 C 语言数据类型的位级表示和数据操作的位级行为。

====================================================================================================
在计算机科学中，"直线代码"（straightline code）是指没有任何分支、循环或函数调用的一段代码。
在这种代码中，每条语句仅执行一次，并且按照它们在代码中的顺序执行。
直线代码通常非常简单且易于理解，因为它不包含任何控制流结构，如 `if` 语句、`while` 循环或 `for` 循环。

在你提到的情境中，要求学生使用 C 语言的受限子集来实现特定的逻辑、二进制补码和浮点函数，很可能是为了强化他们对位操作的理解和应用。
通过限制使用直线代码，学生被迫更加专注于每个操作的位级效果，以及如何仅通过位操作来实现复杂的行为。

例如，计算一个数的绝对值仅使用位级运算和直线代码，可能涉及检查符号位，然后条件性地反转所有位并增加一（这是二进制补码表示中的常见操作）。
这种练习有助于学生深入理解计算机底层是如何处理数据的，特别是在没有高级语言特性如循环、分支或函数调用的情况下。