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
