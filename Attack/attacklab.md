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
 • 回想一下，函数的第一个参数是通过 `%rdi` 寄存器传递的。
 • 您注入的代码应该设置寄存器为您的 cookie，然后使用 ret 指令将控制权转移给 touch2 中的第一条指令。
 • 不要尝试在您的攻击代码中使用 jmp 或 call 指令。这些指令的目的地址编码很难构造。即使您不是从调用中返回，也要使用 ret 指令进行所有控制权的转移。
 • 参见附录 B 中的讨论，了解如何使用工具生成指令序列的字节级表示。

变量	相对于ebp	相对于esp
函数调用者的变量	[ebp + 16]	[esp + 24]
arg2	[ebp + 12]	[esp + 20]
arg1	[ebp + 8]	[esp + 16]
返回地址	[ebp + 4]	[esp + 12]
保存的ebp	[ebp]	[esp + 8]
local1	[ebp - 4]	[esp + 4]
local2	[ebp - 8]	[esp]

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


# A 使用 HEX2RAW

HEX2RAW 接受十六进制格式字符串作为输入。在这种格式中，每个字节值由两个十六进制数字表示。例如，字符串 “012345” 可以以十六进制格式输入为 “30 31 32 33 34 35 00。”（回想一下，十进制数字 x 的 ASCII 码是 0x3x，并且字符串的结尾由空字节表示。）

传递给 HEX2RAW 的十六进制字符应由空白（空格或换行符）分隔。

我们建议在你处理攻击字符串时，使用换行符分隔不同的部分。

HEX2RAW 支持 C 风格的块注释，所以你可以标记攻击字符串的各个部分。例如：

48 c7 c1 f0 11 40 00 /* mov $0x40011f0,%rcx */

确保在开始和结束注释字符串（“/*”, “*/”）周围留有空间，这样注释就会被正确忽略。

如果你在文件 exploit.txt 中生成了一个十六进制格式的攻击字符串，可以通过几种不同的方式将原始字符串应用到 CTARGET 或 RTARGET：

1. 你可以设置一系列管道来通过 HEX2RAW 传递字符串。
unix> cat exploit.txt | ./hex2raw | ./ctarget

2. 你可以将原始字符串存储在文件中，并使用 I/O 重定向：
unix> ./hex2raw < exploit.txt > exploit-raw.txt
unix> ./ctarget < exploit-raw.txt
这种方法也可以在 GDB 中运行时使用：
unix> gdb ctarget
(gdb) run < exploit-raw.txt

3. 你可以将原始字符串存储在文件中，并将文件名作为命令行参数提供：
unix> ./hex2raw < exploit.txt > exploit-raw.txt
unix> ./ctarget -i exploit-raw.txt
这种方法在 GDB 中运行时也可以使用。


# 4.3 Level 3
Phase 3 also involves a code injection attack, but passing a string as argument.

Within the file `ctarget` there is code for functions hexmatch and touch3 having the following C representations:

~~~c
1 /* Compare string to hex represention of unsigned value */
2 int hexmatch(unsigned val, char *sval)
3 {
4     char cbuf[110];
5     /* Make position of check string unpredictable */
6     char *s = cbuf + random() % 100;
7     sprintf(s, "%.8x", val);
8     return strncmp(sval, s, 9) == 0;
9 }
10
11 void touch3(char *sval)
12 {
13    vlevel = 3; /* Part of validation protocol */
14    if (hexmatch(cookie, sval)) {
15    printf("Touch3!: You called touch3(\"%s\")\n", sval);
16    validate(3);
17 } else {
18    printf("Misfire: You called touch3(\"%s\")\n", sval);
19    fail(3);
20 }
21    exit(0);
22 }
~~~

Your task is to get CTARGET to execute the code for touch3 rather than returning to test. You must make it appear to touch3 as if you have passed a string representation of your cookie as its argument.

Some Advice:
 • You will need to include a string representation of your cookie in your exploit string. The string should consist of the eight hexadecimal digits (ordered from most to least significant) without a leading “0x.”
 • Recall that a string is represented in C as a sequence of bytes followed by a byte with value 0. Type “man ascii” on any Linux machine to see the byte representations of the characters you need.
 • Your injected code should set register %rdi to the address of this string.
 • When functions hexmatch and strncmp are called, they push data onto the stack, overwriting portions of memory that held the buffer used by getbuf. As a result, you will need to be careful where you place the string representation of your cookie.

 为了完成这个任务，你需要构造一个利用字符串的代码注入攻击，使得 `ctarget` 程序执行 `touch3` 函数。这个过程需要确保 `touch3` 函数接收到的参数看起来像是你的 cookie 的字符串表示。

### 1. 构造 Cookie 字符串
首先，你需要将你的 cookie 值转换为一个没有前导“0x”的8位十六进制数字字符串。例如，如果你的 cookie 是 `0x1234ABCD`，则字符串应该是 `"1234ABCD"`。此外，记得在字符串末尾添加一个 null 字节（`\0`）以符合 C 语言字符串的标准。

### 2. 确定字符串的放置位置
由于 `getbuf` 的栈空间将被 `hexmatch` 和 `strncmp` 函数调用中使用的数据覆盖，你需要在栈上找到一个安全的位置来存放这个字符串。
通常，这意味着将字符串放置在攻击代码之后或在栈的某个其他位置。

### 3. 编写注入代码
你的注入代码需要执行以下操作：
- 将你的 cookie 字符串的地址加载到 `%rdi` 寄存器中（在 x86-64 架构中，函数的第一个参数通过 `%rdi` 传递）。
- 使用 `ret` 指令跳转到 `touch3` 函数的起始地址。

### 4. 构造攻击字符串
攻击字符串应该包含以下部分：
- 一些填充字节，用于覆盖 `getbuf` 函数的局部变量和保存的帧指针。
- 你的注入代码的机器码。
- 你的 cookie 的字符串表示（可能还包括一些附加的填充，以确保它位于一个安全的位置）。
- `touch3` 函数的地址，用于覆盖 `getbuf` 的返回地址。

### 5. 调试和调整
这个过程可能需要一些调试和调整，以确保所有的元素都放置在正确的位置，并且执行流按预期进行。

当然，以下是提供的文本翻译成中文的版本：

---

# 4.3 第三阶段
第三阶段也涉及代码注入攻击，但这次需要传递一个字符串作为参数。

在文件 `ctarget` 中，有名为 hexmatch 和 touch3 的函数，它们在 C 语言中的定义如下：

```c
1 /* 比较字符串与无符号值的十六进制表示 */
2 int hexmatch(unsigned val, char *sval)
3 {
4     char cbuf[110];
5     /* 使检查字符串的位置不可预测 */
6     char *s = cbuf + random() % 100;
7     sprintf(s, "%.8x", val);
8     return strncmp(sval, s, 9) == 0;
9 }
10
11 void touch3(char *sval)
12 {
13    vlevel = 3; /* 验证协议的一部分 */
14    if (hexmatch(cookie, sval)) {
15        printf("Touch3!：你调用了 touch3(\"%s\")\n", sval);
16        validate(3);
17    } else {
18        printf("未命中：你调用了 touch3(\"%s\")\n", sval);
19        fail(3);
20    }
21    exit(0);
22 }
```

你的任务是操作 CTARGET 以执行 touch3 的代码，而不是返回到测试函数。你必须让它看起来像是你已经将你的 cookie 的字符串表示作为其参数传递了。

一些建议：
 • 你需要在你的利用字符串中包含你的 cookie 的字符串表示。字符串应该由八个十六进制数字组成（从最重要到最不重要的顺序排列），前面没有“0x”。
 • 回想一下，C 语言中的字符串表示为一系列字节，后面跟着一个值为 0 的字节。在任何 Linux 机器上输入 “man ascii” 可以查看你需要的字符的字节表示。
 • 你注入的代码应该将 %rdi 寄存器设置为这个字符串的地址。
 • 当调用 hexmatch 和 strncmp 函数时，它们会将数据压入栈中，覆盖了 getbuf 使用的缓冲区所在的内存部分。因此，你需要小心地放置你的 cookie 的字符串表示。
---

~~~shell
$rax        Return value
$rbx        Callee saved
$rcx        4th argument
$rdx        3rd argument
$rsi        2nd argument
$rdi        1st argument
$rbp        Callee saved
$rsp        Stack Pointer
$r8         5th argument
$r9         6th argument
$r10        Callee saved
$r11        Used fot linking
$r12        Unused for C
$r13        Callee saved
$r14        Callee saved
$r15        Callee saved 
~~~


# 5 Part II: Return-Oriented Programming

Performing code-injection attacks on program `RTARGET` is much more difficult than it is for CTARGET, because it uses two techniques to thwart such attacks:

 - It uses randomization so that the stack positions differ from one run to another. This makes it impossible to determine where your injected code will be located.

 - It marks the section of memory holding the stack as nonexecutable, so even if you could set the program counter to the start of your injected code, the program would fail with a segmentation fault.

Fortunately, clever people have devised strategies for getting useful things done in a program by executing existing code, rather than injecting new code. 

The most general form of this is referred to as return-oriented programming (ROP) [1, 2]. The strategy with ROP is to identify byte sequences within an existing program that consist of one or more instructions followed by the instruction ret.

Such a segment is referred to as a gadget. Figure 2 illustrates how the stack can be set up to execute a sequence of n gadgets. 

In this figure, the stack contains a sequence of gadget addresses. Each gadget consists of a series of instruction bytes, with the final one being 0xc3, encoding the ret instruction. When the program executes a ret instruction starting with this configuration, it will initiate a chain of gadget executions, with the ret instruction at the end of each gadget causing the program to jump to the beginning of the next.

A gadget can make use of code corresponding to assembly-language statements generated by the compiler, especially ones at the ends of functions. In practice, there may be some useful gadgets of this form, but not enough to implement many important operations. For example, it is highly unlikely that a compiled function would have popq %rdi as its last instruction before ret. Fortunately, with a byte-oriented instruction set, such as x86-64, a gadget can often be found by extracting patterns from other parts of the instruction byte sequence.

在这个图中，栈包含一系列小工具地址。每个小工具由一系列指令字节组成，最后一个是 0xc3，编码了 ret 指令。当程序以这种配置执行 ret 指令时，它将启动一连串的小工具执行，每个小工具末尾的 ret 指令将使程序跳转到下一个小工具的开始。

小工具可以利用编译器生成的汇编语言语句对应的代码，特别是函数末尾的那些。实际上，可能有一些这样形式的有用小工具，但不足以实现许多重要操作。例如，编译后的函数在 ret 之前以 popq %rdi 作为其最后一条指令的可能性极低。幸运的是，在字节导向的指令集中，例如 x86-64，经常可以通过从其他部分的指令字节序列中提取模式来找到小工具。

当然，我可以帮助您更深入地理解“面向返回的编程（ROP）”以及为什么它在某些安全场景中特别重要。这种技术主要用于绕过某些安全措施，特别是在那些阻止直接代码注入的环境中。让我逐步解释：

1. **栈随机化**：一些系统通过在每次程序运行时改变栈的内存位置来防止攻击者预测代码的确切位置。这使得直接代码注入变得困难，因为攻击者无法确定他们的代码将被放置在哪里。

2. **非可执行栈**：另一种安全措施是将栈标记为不可执行，这意味着即使攻击者能够在栈上注入代码，程序也无法执行这些代码，因为操作系统阻止在这部分内存中执行任何代码。

### 面向返回的编程（ROP）
面向返回的编程是一种绕过这些安全措施的技术。它不是尝试注入新的代码，而是巧妙地利用程序中已经存在的代码片段来执行攻击者想要的操作。这些代码片段被称为“小工具（gadgets）”。

1. **小工具的定义**：在ROP中，一个小工具通常是由一系列指令组成，以 `ret` 指令结束。每个小工具执行一小部分操作，比如移动数据、进行算术运算或调整寄存器的值。

2. **小工具的作用**：通过精心挑选和排列这些小工具，攻击者可以构造一个小工具链，以执行更复杂的操作序列。这是通过在栈上排列小工具的地址来实现的。当程序执行一个小工具的 `ret` 指令时，它会跳转到下一个小工具的地址，从而形成一个连续的执行链。

3. **绕过安全措施**：ROP的关键在于，它不需要在栈上注入新的代码，而是利用了程序本身的代码。即使栈是非可执行的，攻击者仍然可以执行这些小工具，因为它们是程序有效代码的一部分。

4. **小工具的来源**：小工具可以来自程序的任何部分，尤其是函数的末尾（因为它们通常以 `ret` 指令结束）。有时，甚至可以在指令序列的中间找到小工具，通过在字节级别解析指令来实现。

### 实践中的挑战
ROP是一种高级的攻击技术，需要对目标程序的内部结构和汇编语言有深入了解。构造有效的ROP链需要精确地识别有用的小工具，并精心安排它们以达成所需的结果。这项技术在安全研究中非常重要，因为它展示了即使在采取了强化措施的情况下，系统也可能是脆弱的。

您提出了一个非常重要的问题，它涉及到面向返回编程（ROP）中的一个关键挑战：确保栈上的小工具地址在执行过程中不被覆盖或改变。确实，ROP 中的小工具在执行 `ret` 指令之前可能会执行一些操作，这些操作可能改变栈的状态。为了成功构建 ROP 链，必须仔细考虑这些因素：

1. **栈的操作**：一些小工具可能包括诸如 `pop`、`add` 或 `sub` 等会影响栈指针（`rsp`）的指令。这些操作可能会改变栈的内容，包括存储的返回地址。

2. **小工具的选择**：在构建 ROP 链时，攻击者需要选择那些不会意外破坏栈结构的小工具，或者至少能够预测其对栈的影响。理想情况下，每个小工具执行完毕后，栈顶应该指向下一个小工具的地址。

3. **精确的栈布局**：ROP 攻击的成功在很大程度上依赖于栈布局的精确性。攻击者必须确保在每个小工具执行 `ret` 时，栈顶指向下一个小工具的地址。这可能需要在小工具之间插入适当数量的 `pop` 指令或其他操作，以确保栈指针移动到正确的位置。

4. **复杂的编排**：由于每个小工具可能以不同的方式影响栈，ROP 链的构建类似于解决一个复杂的拼图。攻击者需要仔细规划每一步，以确保整个链条顺利执行。

总的来说，ROP 攻击的关键在于精心设计小工具序列，以使得在执行每个小工具后，栈的顶部恰好指向下一个小工具的地址。这通常涉及到对目标程序的深入分析，以及对栈的操作有精确的理解和控制。ROP 是一种高级和复杂的攻击技术，它要求攻击者具备深入的底层系统和汇编语言知识。
---

For example, one version of rtarget contains code generated for the following C function:

~~~c
void setval_210(unsigned *p)
{
  *p = 3347663060U;
}
~~~

The chances of this function being useful for attacking a system seem pretty slim. But, the disassembled machine code for this function shows an interesting byte sequence

~~~shell
0000000000400f15 <setval_210>:
400f15: c7 07 d4 48 89 c7 movl $0xc78948d4,(%rdi)
400f1b: c3 retq
~~~

The byte sequence 48 89 c7 encodes the instruction movq %rax, %rdi. (See Figure 3A for the encodings of useful movq instructions.) This sequence is followed by byte value c3, which encodes the ret instruction. The function starts at address 0x400f15, and the sequence starts on the fourth byte of the function. Thus, this code contains a gadget, having a starting address of 0x400f18, that will copy the
64-bit value in register %rax to register %rdi

Your code for RTARGET contains a number of functions similar to the setval_210 function shown above in a region we refer to as the gadget farm. Your job will be to identify useful gadgets in the gadget farm and use these to perform attacks similar to those you did in Phases 2 and 3.

Important: The gadget farm is demarcated by functions start_farm and end_farm in your copy of rtarget. Do not attempt to construct gadgets from other portions of the program code.

字节序列 `48 89 c7` 编码了指令 `movq %rax, %rdi`。（参见图 3A，了解有用的 movq 指令的编码。）该序列后面跟着的字节值 `c3` 编码了 `ret` 指令。该函数从地址 `0x400f15` 开始，序列从函数的第四个字节开始。因此，这段代码包含了一个小工具，其起始地址为 `0x400f18`，该小工具将寄存器 `%rax` 中的 64 位值复制到寄存器 `%rdi` 中。

你的 RTARGET 代码包含了许多类似于上面展示的 `setval_210` 函数的函数，它们位于我们称为“小工具农场”的区域内。你的任务是在小工具农场中识别有用的小工具，并使用这些小工具执行类似于你在第二和第三阶段所做的攻击。

重要提示：小工具农场由你的 rtarget 副本中的 start_farm 和 end_farm 函数划定范围。不要试图从程序代码的其他部分构造小工具。
---

# ROP Level 2

For Phase 4, you will repeat the attack of Phase 2, but do so on program RTARGET using gadgets from your gadget farm. You can construct your solution using gadgets consisting of the following instruction types, and using only the first eight x86-64 registers (%rax–%rdi)

`movq` : The codes for these are shown in Figure 3A
`popq` : The codes for these are shown in Figure 3B
`ret` : This instruction is encoded by the single byte 0xc3.
`nop` : This instruction (pronounced “no op,” which is short for “no operation”) is encoded by the single byte 0x90. Its only effect is to cause the program counter to be incremented by 1.

Some Advice:
 • All the gadgets you need can be found in the region of the code for rtarget demarcated by the functions start_farm and mid_farm.
 • You can do this attack with just two gadgets.
 • When a gadget uses a popq instruction, it will pop data from the stack. As a result, your exploit string will contain a combination of gadget addresses and data.

在第四阶段，你将重复第二阶段的攻击，但这次是在 RTARGET 程序上使用你的小工具农场中的小工具进行。
你可以使用以下指令类型的小工具来构造你的解决方案，并且只使用前八个 x86-64 寄存器（%rax–%rdi）：

`movq`：这些的代码如图 3A 所示。
`popq`：这些的代码如图 3B 所示。
`ret`：这个指令由单个字节 0xc3 编码。
`nop`：这个指令（发音为“no op”，缩写为“no operation”）由单个字节 0x90 编码。它唯一的作用是使程序计数器增加 1。

一些建议：
 • 你需要的所有小工具都可以在 rtarget 代码的 start_farm 和 mid_farm 函数划定的区域内找到。
 • 你可以只使用两个小工具来完成这次攻击。
 • 当一个小工具使用 popq 指令时，它会从栈中弹出数据。因此，你的利用字符串将包含小工具地址和数据的组合。
---