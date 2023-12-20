# The Attack Lab: Understanding Buffer Overflow Bugs

# Introduction

This assignment involves generating a total of five attacks on two programs having different security vulnerabilities. Outcomes you will gain from this lab include:

 - You will learn different ways that attackers can exploit security vulnerabilities when programs do not safeguard themselves well enough against buffer overflows.

 - Through this, you will get a better understanding of how to write programs that are more secure, as well as some of the features provided by compilers and operating systems to make programs less vulnerable

 - You will gain a deeper understanding of the stack and parameter-passing mechanisms of x86-64 machine code

 - You will gain a deeper understanding of how x86-64 instructions are encoded.

 - You will gain more experience with debugging tools such as GDB and OBJDUMP.

Note: In this lab, you will gain firsthand experience with methods used to exploit security weaknesses in operating systems and network servers. 

Our purpose is to help you learn about the runtime operation of programs and to understand the nature of these security weaknesses so that you can avoid them when you write system code. 

We do not condone the use of any other form of attack to gain unauthorized access to any system resources.

*You will want to study Sections 3.10.3 and 3.10.4 of the CS:APP3e book as reference material for this lab.*


# 2 Logistics

As usual, this is an individual project. You will generate attacks for target programs that are custom generated for you

# 2.1 Getting Files

You can obtain your files by pointing your Web browser at:

http://$Attacklab::SERVER_NAME:15513/

INSTRUCTOR: $Attacklab::SERVER_NAME is the machine that runs the attacklab servers. You define it in attacklab/Attacklab.pm and in attacklab/src/build/driverhdrs.h

The server will build your files and return them to your browser in a tar file called targetk.tar, where k is the unique number of your target programs.

Note: It takes a few seconds to build and download your target, so please be patient.

Save the targetk.tar file in a (protected) Linux directory in which you plan to do your work. Then give the command: `tar -xvf targetk.tar`. This will extract a directory targetk containing the files described below.

You should only download one set of files. If for some reason you download multiple targets, choose one target to work on and delete the rest.

Warning: If you expand your `targetk.tar` on a PC, by using a utility such as Winzip, or letting your browser do the extraction, you’ll risk resetting permission bits on the executable files

The files in targetk include:
 - `README.txt`: A file describing the contents of the directory
 - `ctarget`: An executable program vulnerable to code-injection attacks
 - `rtarget`: An executable program vulnerable to return-oriented-programming attacks
 - `cookie.txt`: An 8-digit hex code that you will use as a unique identifier in your attacks.
 - `farm.c`: The source code of your target’s “gadget farm,” which you will use in generating return-oriented programming attacks.
 - `hex2raw`: A utility to generate attack strings

In the following instructions, we will assume that you have copied the files to a protected local directory, and that you are executing the programs in that local directory.

# 2.2 Important Points

Here is a summary of some important rules regarding valid solutions for this lab. These points will not make much sense when you read this document for the first time. They are presented here as a central reference of rules once you get started.

 - You must do the assignment on a machine that is similar to the one that generated your targets.

 - Your solutions may not use attacks to circumvent the validation code in the programs. Specifically, any address you incorporate into an attack string for use by a ret instruction should be to one of the following destinations
   * The addresses for functions touch1, touch2, or touch3.
   * The address of your injected code
   * The address of one of your gadgets from the gadget farm

 - You may only construct gadgets from file rtarget with addresses ranging between those for functions start_farm and end_farm.

 - 你必须在一台与生成目标的机器类似的机器上完成作业。

 - 你的解决方案不得使用攻击手段来规避程序中的验证代码。具体来说，你在攻击字符串中加入的任何地址，用于 ret 指令，应该是以下目的地之一：
   * 函数 touch1、touch2 或 touch3 的地址。
   * 你注入代码的地址。
   * 你的小工具库中的一个小工具的地址。

 - 你只能使用文件 rtarget 中的地址构造小工具，这些地址的范围应该在函数 start_farm 和 end_farm 之间。

# 3 Target Programs

Both `CTARGET` and `RTARGET` read strings from standard input. They do so with the function getbuf defined below:

~~~c
unsigned getbuf()
{
    char buf[BUFFER_SIZE];
    Gets(buf);
    return 1;
}
~~~

The function `Gets` is similar to the standard library function `gets` it reads a string from standard input (terminated by ‘`\n`’ or `end-of-file`) and stores it (along with a null terminator) at the specified destination. In this code, you can see that the destination is an array `buf`, declared as having `BUFFER_SIZE` bytes. At the time your targets were generated, `BUFFER_SIZE` was a compile-time constant specific to your version of the programs.

Functions Gets() and gets() have no way to determine whether their destination buffers are large enough to store the string they read. They simply copy sequences of bytes, possibly overrunning the bounds of the storage allocated at the destinations.

If the string typed by the user and read by getbuf is sufficiently short, it is clear that getbuf will return 1, as shown by the following execution examples:

~~~shell
Attack/target1❯ ./rtarget -q
Cookie: 0x59b997fa
Type string:Keep it short!
No exploit.  Getbuf returned 0x1
Normal return
~~~

Typically an error occurs if you type a long string:

~~~shell
target1 [master●] ./ctarget -q
Cookie: 0x59b997fa
Type string:This is not a very interesting string, but it has the property    
Ouch!: You caused a segmentation fault!
Better luck next time
FAIL: Would have posted the following:
        user id bovik
        course  15213-f15
        lab     attacklab
        result  1:FAIL:0xffffffff:ctarget:0:54 68 69 73 20 69 73 20 6E 6F 74 20 61 20 76 65 72 79 20 69 6E 74 65 72 65 73 74 69 6E 67 20 73 74 72 69 6E 67 2C 20 62 75 74 20 69 74 20 68 61 73 20 74 68 65 20 70 72 6F 70 65 72 74 79
~~~


# 4 Part I: Code Injection Attacks
For the first three phases, your exploit strings will attack CTARGET. This program is set up in a way that the stack positions will be consistent from one run to the next and so that data on the stack can be treated as executable code. These features make the program vulnerable to attacks where the exploit strings contain the byte encodings of executable code

# 4.1 Level 1
For Phase 1, you will not inject new code. Instead, your exploit string will redirect the program to execute an existing procedure.
Function getbuf is called within CTARGET by a function test having the following 
~~~c
1 void test()
2 {
3   int val;
4   val = getbuf();
5   printf("No exploit. Getbuf returned 0x%x\n", val);
6 }
~~~

When getbuf executes its return statement (line 5 of getbuf), the program ordinarily resumes execution within function test (at line 5 of this function). We want to change this behavior. Within the file ctarget, there is code for a function touch1 having the following C representation:

~~~c
1 void touch1()
2 {
3   vlevel = 1; /* Part of validation protocol */
4   printf("Touch1!: You called touch1()\n");
5   validate(1);
6   exit(0);
7 }
~~~

Your task is to get CTARGET to execute the code for touch1 when getbuf executes its return statement, rather than returning to test. Note that your exploit string may also corrupt parts of the stack not directly related to this stage, but this will not cause a problem, since touch1 causes the program to exit directly.

Some Advice:
 • All the information you need to devise your exploit string for this level can be determined by examining a disassembled version of CTARGET. Use objdump -d to get this dissembled version.
 • The idea is to position a byte representation of the starting address for touch1 so that the ret instruction at the end of the code for getbuf will transfer control to touch1.
 • Be careful about byte ordering.
 • You might want to use GDB to step the program through the last few instructions of getbuf to make sure it is doing the right thing.
 • The placement of buf within the stack frame for getbuf depends on the value of compile-time constant BUFFER_SIZE, as well the allocation strategy used by GCC. You will need to examine the disassembled code to determine its position.

# 4.1 第一级
在第一阶段，你不会注入新代码。相反，你的攻击字符串将重定向程序以执行一个已存在的过程。
在 CTARGET 中，有一个函数 test 调用了函数 getbuf，如下所示：

~~~c
1 void test()
2 {
3   int val;
4   val = getbuf();
5   printf("没有利用漏洞。Getbuf 返回 0x%x\n", val);
6 }
~~~

当 getbuf 执行其返回语句（getbuf 的第5行）时，程序通常会在函数 test（在这个函数的第5行）中恢复执行。
我们想改变这种行为。在文件 ctarget 中，有一个名为 touch1 的函数代码，其 C 表示如下：

~~~c
1 void touch1()
2 {
3   vlevel = 1; /* 验证协议的一部分 */
4   printf("Touch1!: 你调用了 touch1()\n");
5   validate(1);
6   exit(0);
7 }
~~~

你的任务是在 getbuf 执行其返回语句时让 CTARGET 执行 touch1 的代码，而不是返回到 test。
请注意，你的攻击字符串可能也会破坏与这个阶段直接无关的栈的部分，但这不会造成问题，因为 touch1 会直接导致程序退出。

一些建议：
 • 制定这一级的攻击字符串所需的所有信息可以通过检查 CTARGET 的反汇编版本确定。使用 objdump -d 来获取这个反汇编版本。
 • 目的是将 touch1 起始地址的字节表示定位到一个位置，使得 getbuf 代码末尾的 ret 指令将控制权转移给 touch1。
 • 注意字节顺序。
 • 你可能想要使用 GDB 来逐步执行 getbuf 的最后几条指令，以确保它正常运行。
 • buf 在 getbuf 的栈帧中的位置取决于编译时常量 BUFFER_SIZE 的值，以及 GCC 使用的分配策略。你需要检查反汇编代码以确定其位置。



# 4.2 Level 2

Phase 2 involves injecting a small amount of code as part of your exploit string.

Within the file ctarget there is code for a function touch2 having the following C representation:
~~~c
1 void touch2(unsigned val)
2 {
3   vlevel = 2; /* Part of validation protocol */
4   if (val == cookie) {
5     printf("Touch2!: You called touch2(0x%.8x)\n", val);
6     validate(2);
7   } else {
8     printf("Misfire: You called touch2(0x%.8x)\n", val);
9     fail(2);
10  }
11  exit(0);
12 }
~~~

Your task is to get CTARGET to execute the code for touch2 rather than returning to test. 
In this case, however, you must make it appear to touch2 as if you have passed your cookie as its argument.
Some Advice:
 • You will want to position a byte representation of the address of your injected code in such a way that ret instruction at the end of the code for getbuf will transfer control to it.
• Recall that the first argument to a function is passed in register %rdi.
• Your injected code should set the register to your cookie, and then use a ret instruction to transfer control to the first instruction in touch2.
• Do not attempt to use jmp or call instructions in your exploit code. The encodings of destination addresses for these instructions are difficult to formulate. Use ret instructions for all transfers of control, even when you are not returning from a call.
• See the discussion in Appendix B on how to use tools to generate the byte-level representations of instruction sequences.


# 4.2 第二级

第二阶段涉及到在您的攻击字符串中注入少量代码。

在文件 ctarget 中，有一个名为 touch2 的函数，其 C 语言表示如下：
~~~c
1 void touch2(unsigned val)
2 {
3   vlevel = 2; /* 验证协议的一部分 */
4   if (val == cookie) {
5     printf("Touch2!：你调用了 touch2(0x%.8x)\n", val);
6     validate(2);
7   } else {
8     printf("未命中：你调用了 touch2(0x%.8x)\n", val);
9     fail(2);
10  }
11  exit(0);
12 }
~~~

您的任务是让 CTARGET 执行 touch2 的代码，而不是返回到 test 函数。
但在这种情况下，您必须让 touch2 看起来好像您已经将您的 cookie 作为其参数传递了。
一些建议：
 • 您需要将注入代码的地址的字节表示定位在一个位置，使得 getbuf 代码末尾的 ret 指令将控制权转移给它。
 • 回想一下，函数的第一个参数是通过 %rdi 寄存器传递的。
 • 您注入的代码应该设置寄存器为您的 cookie，然后使用 ret 指令将控制权转移给 touch2 中的第一条指令。
 • 不要尝试在您的攻击代码中使用 jmp 或 call 指令。这些指令的目的地址编码很难构造。即使您不是从调用中返回，也要使用 ret 指令进行所有控制权的转移。
 • 参见附录 B 中的讨论，了解如何使用工具生成指令序列的字节级表示。


# B 生成字节码

使用 GCC 作为汇编器和 OBJDUMP 作为反汇编器，可以方便地生成指令序列的字节码。例如，假设你编写了一个包含以下汇编代码的文件 example.s：

# 手动生成的汇编代码示例

~~~shell
pushq $0xabcdef # 将值压入栈
addq $17,%rax # 向 %rax 加 17
movl %eax,%edx # 将低 32 位复制到 %edx
~~~

代码可以包含指令和数据的混合。‘#’字符右侧的任何内容都是注释。
现在你可以汇编和反汇编这个文件：

~~~shell
unix> gcc -c example.s
unix> objdump -d example.o > example.d
~~~

生成的文件 example.d 包含以下内容：
example.o: 文件格式 elf64-x86-64
反汇编 .text 段：

~~~shell
0000000000000000 <.text>:
0: 68 ef cd ab 00 pushq $0xabcdef
5: 48 83 c0 11 add $0x11,%rax
9: 89 c2 mov %eax,%edx
~~~

底部的行显示了从汇编语言指令生成的机器码。每行左侧有一个十六进制数字，表示指令的起始地址（从0开始），而‘:’字符后的十六进制数字表示指令的字节码。
因此，我们可以看到指令 `push $0xABCDEF` 的十六进制格式字节码是 `68 ef cd ab 00`。
从这个文件中，你可以得到代码的字节序列：

`68 ef cd ab 00 48 83 c0 11 89 c2`

这个字符串然后可以通过 HEX2RAW 转换成目标程序的输入字符串。
或者，你可以编辑 example.d 文件，省略不必要的值，并包含 C 风格的注释以提高可读性，得到：

~~~shell
68 ef cd ab 00 /* pushq $0xabcdef */
48 83 c0 11 /* add $0x11,%rax */
89 c2 /* mov %eax,%edx */
~~~

这也是一个有效的输入，你可以在发送到目标程序之前通过 HEX2RAW 传递。