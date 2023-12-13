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


~~~shell
# phase_1
0x007fffffffdc18│+0x0000: 0x00000000400e3f  →  <main+159> call 0x4015c4 <phase_defused>  ← $rsp
0x007fffffffdc20│+0x0008: 0x00000000402210  →  <__libc_csu_init+0> mov QWORD PTR [rsp-0x28], rbp
0x007fffffffdc28│+0x0010: 0x007ffff7de1083  →  <__libc_start_main+243> mov edi, eax
0x007fffffffdc30│+0x0018: 0x007ffff7ffc620  →  0x00050f7e00000000
0x007fffffffdc38│+0x0020: 0x007fffffffdd18  →  0x007fffffffe050  →  "/home/czy/new_space/CSAPP_Lab/Boom/bomb/bomb"
0x007fffffffdc40│+0x0028: 0x0000000100000000
0x007fffffffdc48│+0x0030: 0x00000000400da0  →  <main+0> push rbx
0x007fffffffdc50│+0x0038: 0x00000000402210  →  <__libc_csu_init+0> mov QWORD PTR [rsp-0x28], rbp
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── code:x86:64 ────
     0x400edd                  nop    
     0x400ede                  nop    
     0x400edf                  nop    
 →   0x400ee0 <phase_1+0>      sub    rsp, 0x8
     0x400ee4 <phase_1+4>      mov    esi, 0x402400
     0x400ee9 <phase_1+9>      call   0x401338 <strings_not_equal>
     0x400eee <phase_1+14>     test   eax, eax
     0x400ef0 <phase_1+16>     je     0x400ef7 <phase_1+23>
     0x400ef2 <phase_1+18>     call   0x40143a <explode_bomb>
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── threads ────
[#0] Id 1, Name: "bomb", stopped 0x400ee0 in phase_1 (), reason: BREAKPOINT
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── trace ────
[#0] 0x400ee0 → phase_1()
[#1] 0x400e3f → main(argc=<optimized out>, argv=<optimized out>)
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
gef➤  x /s  0x402400
0x402400:       "Border relations with Canada have never been better."
~~~


~~~
0000000000400efc <phase_2>:
  400efc:	55                   	push   %rbp
  400efd:	53                   	push   %rbx
  400efe:	48 83 ec 28          	sub    $0x28,%rsp
  400f02:	48 89 e6             	mov    %rsp,%rsi
  400f05:	e8 52 05 00 00       	callq  40145c <read_six_numbers>
  400f0a:	83 3c 24 01          	cmpl   $0x1,(%rsp)
  400f0e:	74 20                	je     400f30 <phase_2+0x34>
  400f10:	e8 25 05 00 00       	callq  40143a <explode_bomb>
  400f15:	eb 19                	jmp    400f30 <phase_2+0x34>
  400f17:	8b 43 fc             	mov    -0x4(%rbx),%eax
  400f1a:	01 c0                	add    %eax,%eax
  400f1c:	39 03                	cmp    %eax,(%rbx)
  400f1e:	74 05                	je     400f25 <phase_2+0x29>
  400f20:	e8 15 05 00 00       	callq  40143a <explode_bomb>
  400f25:	48 83 c3 04          	add    $0x4,%rbx
  400f29:	48 39 eb             	cmp    %rbp,%rbx
  400f2c:	75 e9                	jne    400f17 <phase_2+0x1b>
  400f2e:	eb 0c                	jmp    400f3c <phase_2+0x40>
  400f30:	48 8d 5c 24 04       	lea    0x4(%rsp),%rbx
  400f35:	48 8d 6c 24 18       	lea    0x18(%rsp),%rbp
  400f3a:	eb db                	jmp    400f17 <phase_2+0x1b>
  400f3c:	48 83 c4 28          	add    $0x28,%rsp
  400f40:	5b                   	pop    %rbx
  400f41:	5d                   	pop    %rbp
  400f42:	c3                   	retq  

这段汇编代码是一个具体功能的实现，它涉及到栈操作、函数调用、条件判断和跳转。下面是对这段代码的逐行解读和分析：

1. `400efc: 55` - `push %rbp`：将基指针（rbp）压入栈中，用于保存当前栈帧的基地址。
2. `400efd: 53` - `push %rbx`：将bx寄存器压栈，保存其值以便后续恢复。
3. `400efe: 48 83 ec 28` - `sub $0x28, %rsp`：栈指针（rsp）减去40（0x28），为局部变量分配栈空间。
4. `400f02: 48 89 e6` - `mov %rsp, %rsi`：将栈指针（rsp）的值移动到si寄存器，通常用于参数传递。

5. `400f05: e8 52 05 00 00` - `callq 40145c <read_six_numbers>`：调用`read_six_numbers`函数，该函数可能用于从某处读取六个数值并存储到栈上。

6. `400f0a: 83 3c 24 01` - `cmpl $0x1, (%rsp)`：比较栈顶的值与1，用于检查`read_six_numbers`函数的第一个返回值。
7. `400f0e: 74 20` - `je 400f30 <phase_2+0x34>`：如果相等（即第一个数值为1），则跳转到地址`400f30`。

8. `400f10: e8 25 05 00 00` - `callq 40143a <explode_bomb>`：如果第一个数值不为1，调用`explode_bomb`函数，这可能是一种错误处理或异常情况。
9. `400f15: eb 19` - `jmp 400f30 <phase_2+0x34>`：无条件跳转到地址`400f30`。

10. `400f17: 8b 43 fc` - `mov -0x4(%rbx), %eax`：将bx寄存器减去4的内存位置的值移动到ax寄存器。
11. `400f1a: 01 c0` - `add %eax, %eax`：将ax寄存器的值加到自身，即将其翻倍。
12. `400f1c: 39 03` - `cmp %eax, (%rbx)`：比较ax寄存器的值与bx指向的内存位置的值。
13. `400f1e: 74 05` - `je 400f25 <phase_2+0x29>`：如果相等，则跳转到地址`400f25`。
14. `400f20: e8 15 05 00 00` - `callq 40143a <explode_bomb>`：如果不相等，调用`explode_bomb`函数。

15. `400f25: 48 83 c3 04` - `add $0x4, %rbx`：将bx寄存器的值增加4，指向下一个整数。
16. `400f29: 48 39 eb` - `cmp %rbp, %rbx`：比较bp和bx寄存器的值。
17. `400f2c: 75 e9` - `jne 400f17 <phase_2+0x1b>`：如果不相等，跳转回`400f17`。

18. `400f30: 48 8d 5c 24 04` - `lea 0x4(%rsp), %rbx`：将栈指针加4的地址载入bx寄存器。
19. `400f35: 48 8d 6c 24 18` - `lea 0x18(%rsp), %rbp`：将栈指针加24的地址载入bp寄存器。
20. `400f3a: eb db` - `jmp
 400f17 <phase_2+0x1b>`：无条件跳转到地址`400f17`。

21. `400f3c: 48 83 c4 28` - `add $0x28, %rsp`：栈指针增加40，清理局部变量空间。
22. `400f40: 5b` - `pop %rbx`：从栈中弹出之前保存的bx寄存器的值。
23. `400f41: 5d` - `pop %rbp`：从栈中弹出之前保存的bp寄存器的值。
24. `400f42: c3` - `retq`：函数返回。

整体逻辑分析：
- 这个函数首先读取六个数值（可能是用户输入）。
- 检查第一个数值是否为1，不是则触发错误（`explode_bomb`）。
- 接着遍历这些数值，进行一系列检查（例如，每个数是否是前一个数的两倍），如果检查不通过，则再次触发错误。
- 如果所有检查都通过，函数正常结束。  
~~~

~~~s
+---------------------+
| Start: phase_2      |
+---------------------+
          |
          v
+---------------------+
| push %rbp           |
| push %rbx           |
| sub $0x28, %rsp     |
+---------------------+
          |
          v
+---------------------+
| call read_six_numbers|
+---------------------+
          |
          v
+---------------------+
| cmpl $0x1, (%rsp)   |
+--------+------------+
         |
         | No
         v
+---------------------+
| call explode_bomb   |
+---------------------+
         |
         v
+---------------------+
| jump to 400f30      |
+--------+------------+
         |
         | Yes
         v
+---------------------+
| lea 0x4(%rsp), %rbx |
| lea 0x18(%rsp), %rbp|
+---------------------+
          |
          v
+---------------------+
| Loop Start: 400f17  |
+---------------------+
          |
          v
+---------------------+
| mov -0x4(%rbx), %eax|
| add %eax, %eax      |
| cmp %eax, (%rbx)    |
+--------+------------+
         |
         | No
         v
+---------------------+
| call explode_bomb   |
+---------------------+
         |
         v
+---------------------+
| add $0x4, %rbx      |
| cmp %rbp, %rbx      |
+--------+------------+
         | 
         | Yes
         v
+---------------------+
| Loop End            |
+---------------------+
          |
          v
+---------------------+
| add $0x28, %rsp     |
| pop %rbx            |
| pop %rbp            |
| retq                |
+---------------------+
          |
          v
+---------------------+
| End of phase_2      |
+---------------------+

~~~


0000000000400f43 <phase_3>:
  400f43:	48 83 ec 18          	sub    $0x18,%rsp
  400f47:	48 8d 4c 24 0c       	lea    0xc(%rsp),%rcx
  400f4c:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx
  400f51:	be cf 25 40 00       	mov    $0x4025cf,%esi

这段汇编代码是一个函数（`phase_3`）的开始部分，涉及栈操作、函数调用、数据传递和条件跳转。以下是对这段代码的解析：

1. `400f43: 48 83 ec 18` - `sub $0x18, %rsp`：
   - 这是一个栈操作指令，用于为局部变量分配空间。它从栈指针（`%rsp`）中减去24（`0x18`），以此在栈上分配24字节的空间。

2. `400f47: 48 8d 4c 24 0c` - `lea 0xc(%rsp), %rcx`：
   - `lea`（Load Effective Address）指令用于计算地址并将结果存入寄存器。这里它将栈指针（`%rsp`）加上12字节的地址加载到`%rcx`寄存器。这个地址可能用于存储函数参数或临时数据。

3. `400f4c: 48 8d 54 24 08` - `lea 0x8(%rsp), %rdx`：
   - 同样是`lea`指令，将栈指针加上8字节的地址加载到`%rdx`寄存器。

4. `400f51: be cf 25 40 00` - `mov $0x4025cf, %esi`：
   - 将立即数`0x4025cf`（一个地址或数据）移动到`%esi`寄存器。这个值可能是一个字符串常量或内存地址。

  400f56:	b8 00 00 00 00       	mov    $0x0,%eax
  400f5b:	e8 90 fc ff ff       	callq  400bf0 <__isoc99_sscanf@plt>
  400f60:	83 f8 01             	cmp    $0x1,%eax
  400f63:	7f 05                	jg     400f6a <phase_3+0x27>
  400f65:	e8 d0 04 00 00       	callq  40143a <explode_bomb>

5. `400f56: b8 00 00 00 00` - `mov $0x0, %eax`：
   - 将0移动到`%eax`寄存器，通常在调用某些函数之前将`%eax`清零是一个标准做法，因为它通常用于存储函数返回值。
6. `400f5b: e8 90 fc ff ff` - `callq 400bf0 <__isoc99_sscanf@plt>`：
   - 调用`__isoc99_sscanf`函数。这个函数通常用于从字符串解析格式化的输入。这里的`%esi`，`%rdx`，和`%rcx`寄存器可能分别用于传递格式字符串，和两个用于存储解析结果的地址。
7. `400f60: 83 f8 01` - `cmp $0x1, %eax`：
   - 将`%eax`寄存器的值与1进行比较。`%eax`通常在调用函数后包含返回值，在这里可能是`__isoc99_sscanf`的返回值，表示成功解析的字段数量。
8. `400f63: 7f 05` - `jg 400f6a <phase_3+0x27>`：
   - `jg`（Jump if Greater）指令用于检查前一条指令的比较结果。如果`%eax`大于1，则跳转到`400f6a`处继续执行。
9. `400f65: e8 d0 04 00 00` - `callq 40143a <explode_bomb>`：
   - 如果`%eax`不大于1，即`__isoc99_sscanf`没有成功解析出足够的数据，程序调用`explode_bomb`函数，这可能是一种错误处理机制。

  400f6a:	83 7c 24 08 07       	cmpl   $0x7,0x8(%rsp)
  400f6f:	77 3c                	ja     400fad <phase_3+0x6a>

1. `400f6a: 83 7c 24 08 07` - `cmpl $0x7, 0x8(%rsp)`：
   - 这是一个比较指令。它将栈上`%rsp+0x8`地址处的值与7进行比较。这可能是检查某个变量或输入是否大于7。
2. `400f6f: 77 3c` - `ja 400fad <phase_3+0x6a>`：
   - `ja`（Jump if Above）指令用于检查前面的比较。如果`%rsp+0x8`的值大于7，则跳转到地址`400fad`。

  400f71:	8b 44 24 08          	mov    0x8(%rsp),%eax
  400f75:	ff 24 c5 70 24 40 00 	jmpq   *0x402470(,%rax,8)
  400f7c:	b8 cf 00 00 00       	mov    $0xcf,%eax
  400f81:	eb 3b                	jmp    400fbe <phase_3+0x7b>
  400f83:	b8 c3 02 00 00       	mov    $0x2c3,%eax
  400f88:	eb 34                	jmp    400fbe <phase_3+0x7b>
  400f8a:	b8 00 01 00 00       	mov    $0x100,%eax
  400f8f:	eb 2d                	jmp    400fbe <phase_3+0x7b>
  400f91:	b8 85 01 00 00       	mov    $0x185,%eax
  400f96:	eb 26                	jmp    400fbe <phase_3+0x7b>
  400f98:	b8 ce 00 00 00       	mov    $0xce,%eax
  400f9d:	eb 1f                	jmp    400fbe <phase_3+0x7b>
  400f9f:	b8 aa 02 00 00       	mov    $0x2aa,%eax
  400fa4:	eb 18                	jmp    400fbe <phase_3+0x7b>
  400fa6:	b8 47 01 00 00       	mov    $0x147,%eax
  400fab:	eb 11                	jmp    400fbe <phase_3+0x7b>

3. `400f71: 8b 44 24 08` - `mov 0x8(%rsp), %eax`：
   - 将栈上`%rsp+0x8`的值移动到`%eax`寄存器。这里，`%eax`可能用作后续操作的索引或值。
4. `400f75: ff 24 c5 70 24 40 00` - `jmpq *0x402470(,%rax,8)`：
   - 这是一个基于表的间接跳转。它将使用`%rax`作为索引，在地址`0x402470`处的跳转表中查找跳转目标。跳转表中的每个条目大小为8字节（因为是64位地址），故使用`%rax * 8`进行偏移。
5. 接下来的一系列指令都是设置`%eax`寄存器的值并跳转到`400fbe`：
   - 这些指令（`mov $值, %eax` 和 `jmp 400fbe`）设置不同的返回值到`%eax`，然后跳转到相同的代码点`400fbe`。这种模式通常用于在不同条件下设置不同的返回值。

例如：
   - `400f7c: b8 cf 00 00 00` - `mov $0xcf, %eax`：设置`%eax`为0xcf。
   - `400f81: eb 3b` - `jmp 400fbe`：跳转到`400fbe`。

6. 同样的模式重复：
   - 不同的`mov`指令设置不同的值到`%eax`。
   - 跟随的`jmp`指令跳转到同一个地址（`400fbe`）。

这段代码实现了一个基于条件的跳转逻辑。它首先检查一个栈上的值，然后根据这个值选择不同的代码路径执行。
间接跳转指令（`jmpq *0x402470(,%rax,8)`）说明这里可能有一个跳转表，用于根据不同的条件跳转到不同的地址。后续的一系列`mov`和`jmp`指令则为不同情况设置了不同的返回值。

  400fad:	e8 88 04 00 00       	callq  40143a <explode_bomb>
  400fb2:	b8 00 00 00 00       	mov    $0x0,%eax
  400fb7:	eb 05                	jmp    400fbe <phase_3+0x7b>
  400fb9:	b8 37 01 00 00       	mov    $0x137,%eax

  400fbe:	3b 44 24 0c          	cmp    0xc(%rsp),%eax
  400fc2:	74 05                	je     400fc9 <phase_3+0x86>
  400fc4:	e8 71 04 00 00       	callq  40143a <explode_bomb>
  400fc9:	48 83 c4 18          	add    $0x18,%rsp

  400fcd:	c3                   	retq   

1. `400fbe: 3b 44 24 0c` - `cmp 0xc(%rsp), %eax`：
   - 这行代码执行比较操作。它比较位于栈指针（`%rsp`）加上12（`0xc`）字节处的值与累加器（`%eax`）中的值。
   - `%rsp`通常指向当前的栈顶。`0xc(%rsp)`表示取栈顶地址上方12字节处的值。这可能是一个之前存储的局部变量或函数参数。
   - `%eax`是一个常用的寄存器，经常用来存储函数返回值或临时数据。
2. `400fc2: 74 05` - `je 400fc9 <phase_3+0x86>`：
   - 这是一个条件跳转指令。`je`（Jump if Equal）指令的作用是，如果前面的`cmp`指令比较结果显示两个值相等（即ZF标志位被设置），则跳转到标签`400fc9`处的代码继续执行。
   - 如果两个比较的值不相等，程序将继续顺序执行下一条指令。
3. `400fc4: e8 71 04 00 00` - `callq 40143a <explode_bomb>`：
   - 这是一个函数调用指令。`callq`用于调用另一个函数，这里是`explode_bomb`。
   - 这行代码通常在条件判断失败的情况下执行，如上面的`je`指令没有跳转，则执行这一行。`explode_bomb`函数的名字暗示它可能是在错误或不期望的路径发生时执行的代码，可能用于错误处理或者是一种警告机制。

综合来看，这段代码的逻辑是：比较栈上某个位置的值和`%eax`寄存器的值，如果相等则跳转到另一个代码块，如果不相等则调用`explode_bomb`函数。


~~~c
// IDA 大法
  if ( (int)__isoc99_sscanf(a1, (__int64)"%d %d", (__int64)v8, (__int64)v8 + 4, a5, a6, v7, v8[0]) <= 1 )
    explode_bomb();
  switch ( LODWORD(v8[0]) )
  {
    case 0:
      result = 207LL;
      break;
    case 1:
      result = 311LL;
      break;
    case 2:
      result = 707LL;
      break;
    case 3:
      result = 256LL;
      break;
    case 4:
      result = 389LL;
      break;
    case 5:
      result = 206LL;
      break;
    case 6:
      result = 682LL;
      break;
    case 7:
      result = 327LL;
      break;
    default:
      explode_bomb();
  }
  if ( (_DWORD)result != HIDWORD(v8[0]) )
    explode_bomb();
  return result;
~~~






~~~shell
gef➤  disas phase_4
Dump of assembler code for function phase_4:
=> 0x000000000040100c <+0>:     sub    rsp,0x18
   0x0000000000401010 <+4>:     lea    rcx,[rsp+0xc]
   0x0000000000401015 <+9>:     lea    rdx,[rsp+0x8]
   0x000000000040101a <+14>:    mov    esi,0x4025cf
   0x000000000040101f <+19>:    mov    eax,0x0
   0x0000000000401024 <+24>:    call   0x400bf0 <__isoc99_sscanf@plt>
   0x0000000000401029 <+29>:    cmp    eax,0x2
   0x000000000040102c <+32>:    jne    0x401035 <phase_4+41>
   0x000000000040102e <+34>:    cmp    DWORD PTR [rsp+0x8],0xe
   0x0000000000401033 <+39>:    jbe    0x40103a <phase_4+46>
   0x0000000000401035 <+41>:    call   0x40143a <explode_bomb>
   0x000000000040103a <+46>:    mov    edx,0xe
   0x000000000040103f <+51>:    mov    esi,0x0
   0x0000000000401044 <+56>:    mov    edi,DWORD PTR [rsp+0x8]
   0x0000000000401048 <+60>:    call   0x400fce <func4>
   0x000000000040104d <+65>:    test   eax,eax
   0x000000000040104f <+67>:    jne    0x401058 <phase_4+76>
   0x0000000000401051 <+69>:    cmp    DWORD PTR [rsp+0xc],0x0
   0x0000000000401056 <+74>:    je     0x40105d <phase_4+81>
   0x0000000000401058 <+76>:    call   0x40143a <explode_bomb>
   0x000000000040105d <+81>:    add    rsp,0x18
   0x0000000000401061 <+85>:    ret    
End of assembler dump.

gef➤  disas func4
Dump of assembler code for function func4:
   0x0000000000400fce <+0>:     sub    rsp,0x8
   0x0000000000400fd2 <+4>:     mov    eax,edx
   0x0000000000400fd4 <+6>:     sub    eax,esi
   0x0000000000400fd6 <+8>:     mov    ecx,eax
   0x0000000000400fd8 <+10>:    shr    ecx,0x1f
   0x0000000000400fdb <+13>:    add    eax,ecx
   0x0000000000400fdd <+15>:    sar    eax,1
   0x0000000000400fdf <+17>:    lea    ecx,[rax+rsi*1]
   0x0000000000400fe2 <+20>:    cmp    ecx,edi
   0x0000000000400fe4 <+22>:    jle    0x400ff2 <func4+36>
   0x0000000000400fe6 <+24>:    lea    edx,[rcx-0x1]
   0x0000000000400fe9 <+27>:    call   0x400fce <func4>
   0x0000000000400fee <+32>:    add    eax,eax
   0x0000000000400ff0 <+34>:    jmp    0x401007 <func4+57>
   0x0000000000400ff2 <+36>:    mov    eax,0x0
   0x0000000000400ff7 <+41>:    cmp    ecx,edi
   0x0000000000400ff9 <+43>:    jge    0x401007 <func4+57>
   0x0000000000400ffb <+45>:    lea    esi,[rcx+0x1]
   0x0000000000400ffe <+48>:    call   0x400fce <func4>
   0x0000000000401003 <+53>:    lea    eax,[rax+rax*1+0x1]
   0x0000000000401007 <+57>:    add    rsp,0x8
   0x000000000040100b <+61>:    ret    
End of assembler dump.
~~~


这段汇编代码展示了`phase_4`函数的逻辑。以下是每条指令的解释和整体逻辑的分析：
1. `sub rsp,0x18`：为局部变量分配24字节的栈空间。
2. `lea rcx,[rsp+0xc]`：将栈上`rsp+0xc`地址的值加载到`rcx`寄存器。
3. `lea rdx,[rsp+0x8]`：将栈上`rsp+0x8`地址的值加载到`rdx`寄存器。
4. `mov esi,0x4025cf`：将地址`0x4025cf`加载到`esi`寄存器，可能是`__isoc99_sscanf`函数的格式字符串。
5. `mov eax,0x0`：将`eax`寄存器清零。
6. `call 0x400bf0 <__isoc99_sscanf@plt>`：调用`__isoc99_sscanf`函数，可能用于解析输入。

7. `cmp eax,0x2`：比较`eax`（`__isoc99_sscanf`的返回值）与2，检查是否成功解析了两个输入值。
8. `jne 0x401035 <phase_4+41>`：如果不等于2，跳转到地址`0x401035`，可能触发错误处理。=> 💣

9. `cmp DWORD PTR [rsp+0x8],0xe`：比较栈上`rsp+0x8`处的值与14。
10. `jbe 0x40103a <phase_4+46>`：如果栈上的值小于等于14，跳转到地址`0x40103a`。
11. `call 0x40143a <explode_bomb>`：调用`explode_bomb`函数，可能是错误处理。=> 💣

12. `mov edx,0xe`：将14移动到`edx`寄存器。
13. `mov esi,0x0`：将0移动到`esi`寄存器。
14. `mov edi,DWORD PTR [rsp+0x8]`：将栈上`rsp+0x8`处的值移动到`edi`寄存器。

gef➤  p $edx
$16 = 0xe
gef➤  p $esi
$17 = 0x0
gef➤  p $edi
$18 = 0xe

15. `call 0x400fce <func4>`：调用`func4`函数。
16. `test eax,eax`：测试`eax`寄存器的值。
17. `jne 0x401058 <phase_4+76>`：如果`eax`非零，跳转到`0x401058`。=> 💣

18. `cmp DWORD PTR [rsp+0xc],0x0`：比较栈上`rsp+0xc`处的值与0。
19. `je 0x40105d <phase_4+81>`：如果等于0，跳转到`0x40105d`。=> 😊
20. `call 0x40143a <explode_bomb>`：再次调用`explode_bomb`函数。=> 💣

21. `add rsp,0x18`：清理栈空间。
22. `ret`：函数返回。

### 总结
`phase_4`函数似乎是一个输入验证函数，它首先尝试解析两个输入值。如果解析成功，它会检查第一个输入值是否在特定范围内，并且可能会基于这个值调用另一个函数`func4`。最后，它会检查第二个输入值是否符合某个条件。如果任何检查失败，`explode_bomb`函数会被调用，这可能意味着程序中的一种错误处理机制。

这段汇编代码是`func4`函数的实现，它似乎是一个递归函数。下面是对这段代码的解析：

1. `sub rsp,0x8`：为局部变量或调用过程减少8字节栈空间。
2. `mov eax,edx`：将`edx`寄存器的值移动到`eax`寄存器。
3. `sub eax,esi`：从`eax`中减去`esi`的值。
4. `mov ecx,eax`：将`eax`的值复制到`ecx`寄存器。
5. `shr ecx,0x1f`：逻辑右移`ecx` 31位，用于得到`eax`符号的副本。
6. `add eax,ecx`：将`ecx`的值加到`eax`上，与下一步结合用于进行算术右移。
7. `sar eax,1`：算术右移`eax`，即除以2。
8. `lea ecx,[rax+rsi*1]`：计算`rax + rsi`的值，并将其存储在`ecx`中。
9. `cmp ecx,edi`：比较`ecx`和`edi`的值。
10. `jle 0x400ff2 <func4+36>`：如果`ecx`小于等于`edi`，跳转到`0x400ff2`。
11. `lea edx,[rcx-0x1]`：将`ecx - 1`的结果存储在`edx`中。
12. `call 0x400fce <func4>`：递归调用`func4`。
13. `add eax,eax`：将`eax`的值翻倍。
14. `jmp 0x401007 <func4+57>`：无条件跳转到`0x401007`。
15. `mov eax,0x0`：将`eax`设置为0。
16. `cmp ecx,edi`：再次比较`ecx`和`edi`的值。
17. `jge 0x401007 <func4+57>`：如果`ecx`大于等于`edi`，跳转到`0x401007`。
18. `lea esi,[rcx+0x1]`：将`ecx + 1`的结果存储在`esi`中。
19. `call 0x400fce <func4>`：再次递归调用`func4`。
20. `lea eax,[rax+rax*1+0x1]`：计算`2 * rax + 1`，并将结果存储在`eax`中。
21. `add rsp,0x8`：清理之前分配的栈空间。
22. `ret`：函数返回。

为了创建`func4`的逻辑调用图并尝试还原其C语言代码，我们可以根据汇编指令的逻辑来分析。
首先，让我们来绘制一个简化的逻辑图，然后基于这个逻辑提供一个可能的C语言实现。



### 逻辑调用图

```
func4(edi, esi, edx):
    ecx = (edx - esi) / 2 + esi
    if (ecx <= edi):
        if (ecx >= edi):
            return 0
        else:
            return 2 * func4(edi, ecx + 1, edx) + 1
    else:
        return 2 * func4(edi, esi, ecx - 1)
```

这个图表示的是函数`func4`的递归逻辑。函数接受三个参数（这里用`edi`, `esi`, 和`edx`表示），计算一个中间值`ecx`，然后根据`ecx`与`edi`的比较结果决定递归的路径。

### C语言实现

基于上述逻辑，`func4`函数的可能C语言实现如下：

```c
int func4(int edi, int esi, int edx) {
    int ecx = (edx - esi) / 2 + esi;

    if (ecx <= edi) {
        if (ecx >= edi) {
            return 0;
        } else {
            return 2 * func4(edi, ecx + 1, edx) + 1;
        }
    } else {
        return 2 * func4(edi, esi, ecx - 1);
    }
}
```

~~~shell
gef➤  disas phase_5
Dump of assembler code for function phase_5:
=> 0x0000000000401062 <+0>:     push   rbx
   0x0000000000401063 <+1>:     sub    rsp,0x20

   0x0000000000401067 <+5>:     mov    rbx,rdi
   0x000000000040106a <+8>:     mov    rax,QWORD PTR fs:0x28
   0x0000000000401073 <+17>:    mov    QWORD PTR [rsp+0x18],rax
   0x0000000000401078 <+22>:    xor    eax,eax
   0x000000000040107a <+24>:    call   0x40131b <string_length>
   0x000000000040107f <+29>:    cmp    eax,0x6
   0x0000000000401082 <+32>:    je     0x4010d2 <phase_5+112>
   0x0000000000401084 <+34>:    call   0x40143a <explode_bomb>
   0x0000000000401089 <+39>:    jmp    0x4010d2 <phase_5+112>

   0x000000000040108b <+41>:    movzx  ecx,BYTE PTR [rbx+rax*1]
   0x000000000040108f <+45>:    mov    BYTE PTR [rsp],cl
   0x0000000000401092 <+48>:    mov    rdx,QWORD PTR [rsp]
   0x0000000000401096 <+52>:    and    edx,0xf
   0x0000000000401099 <+55>:    movzx  edx,BYTE PTR [rdx+0x4024b0]
   0x00000000004010a0 <+62>:    mov    BYTE PTR [rsp+rax*1+0x10],dl
   0x00000000004010a4 <+66>:    add    rax,0x1
   0x00000000004010a8 <+70>:    cmp    rax,0x6
   0x00000000004010ac <+74>:    jne    0x40108b <phase_5+41>

   0x00000000004010ae <+76>:    mov    BYTE PTR [rsp+0x16],0x0
   0x00000000004010b3 <+81>:    mov    esi,0x40245e
   0x00000000004010b8 <+86>:    lea    rdi,[rsp+0x10]
   0x00000000004010bd <+91>:    call   0x401338 <strings_not_equal>
   0x00000000004010c2 <+96>:    test   eax,eax
   0x00000000004010c4 <+98>:    je     0x4010d9 <phase_5+119>
   0x00000000004010c6 <+100>:   call   0x40143a <explode_bomb>

   0x00000000004010cb <+105>:   nop    DWORD PTR [rax+rax*1+0x0]
   0x00000000004010d0 <+110>:   jmp    0x4010d9 <phase_5+119>
   0x00000000004010d2 <+112>:   mov    eax,0x0
   0x00000000004010d7 <+117>:   jmp    0x40108b <phase_5+41>

   0x00000000004010d9 <+119>:   mov    rax,QWORD PTR [rsp+0x18]
   0x00000000004010de <+124>:   xor    rax,QWORD PTR fs:0x28
   0x00000000004010e7 <+133>:   je     0x4010ee <phase_5+140>
   0x00000000004010e9 <+135>:   call   0x400b30 <__stack_chk_fail@plt>

   0x00000000004010ee <+140>:   add    rsp,0x20
   0x00000000004010f2 <+144>:   pop    rbx
   0x00000000004010f3 <+145>:   ret    
End of assembler dump.
~~~

~~~shell
   0x000000000040108b <+41>:    movzx  ecx,BYTE PTR [rbx+rax*1]
~~~
这条汇编指令 `movzx ecx, BYTE PTR [rbx+rax*1]` 在执行上做了几件事情：

1. **地址计算**：
   - 计算 `rbx + rax`。这里 `rax` 乘以 1，实际上没有改变 `rax` 的值，所以它就是 `rbx` 和 `rax` 的简单相加。这个计算的结果是一个内存地址。
2. **内存访问**：
   - 从上面计算出的内存地址处读取一个字节（`BYTE PTR` 表示指针指向的是一个字节大小的数据）。
3. **数据扩展并移动到寄存器**：
   - `movzx` 指令是 Move with Zero-Extend 的缩写。它将读取的那个字节（8位）的值移动到 `ecx` 寄存器中，并且在移动过程中，将这个值扩展为 `ecx` 寄存器大小（在x86-64架构下是32位）。扩展是通过在高位添加零来实现的，这就是 "Zero-Extend"（零扩展）的含义。

### 举例说明
假设 `rbx` 存储的是数组的起始地址，`rax` 是数组的索引，那么这条指令实际上是从数组中读取一个字节的数据，然后把这个数据放到 `ecx` 寄存器中，同时保证 `ecx` 寄存器的高24位被清零。
在高级语言（如C）中，这条指令等价于下面的操作：
```c
ecx = (unsigned int)array[rax]; // 假设 array 是一个 byte 类型的数组
```
这条指令在处理小型数据（如字符或其他8位数据类型）时非常有用，特别是当这些数据需要在之后的计算或比较中被当作更大的数据类型来处理。通过零扩展，可以保证高位的准确性，防止不必要的符号扩展影响计算结果。


汇编指令 `mov BYTE PTR [rsp], cl` 执行以下操作：
1. **数据移动**：
   - 将 `cl` 寄存器中的值移动到由 `rsp` 寄存器指向的内存地址处。在这里，`cl` 是 `rcx` 寄存器的低8位。
2. **操作的精度**：
   - `BYTE PTR` 指示这个操作是针对一个字节（8位）的数据。因此，不管 `cl` 寄存器中的内容是什么，只有它的最低8位（一个字节）会被写入内存。
3. **栈操作**：
   - `rsp` 寄存器是栈指针，它指向当前栈帧的顶部。这条指令实际上是在栈顶写入一个字节的数据。


~~~shell
   0x000000000040108b <+41>:    movzx  ecx,BYTE PTR [rbx+rax*1]
   0x000000000040108f <+45>:    mov    BYTE PTR [rsp],cl
   0x0000000000401092 <+48>:    mov    rdx,QWORD PTR [rsp]
   0x0000000000401096 <+52>:    and    edx,0xf
   0x0000000000401099 <+55>:    movzx  edx,BYTE PTR [rdx+0x4024b0]
   0x00000000004010a0 <+62>:    mov    BYTE PTR [rsp+rax*1+0x10],dl
   0x00000000004010a4 <+66>:    add    rax,0x1
   0x00000000004010a8 <+70>:    cmp    rax,0x6
~~~
1. `movzx ecx, BYTE PTR [rbx+rax*1]`：
   - 将 `rbx` 和 `rax` 的和作为地址，从这个地址读取一个字节，并将其零扩展后存入 `ecx` 寄存器。
2. `mov BYTE PTR [rsp], cl`：
   - 将 `cl`（`ecx`的低8位）的值存储到栈顶（`rsp`指向的位置）。
3. `mov rdx, QWORD PTR [rsp]`：
   - 从栈顶（`rsp`指向的位置）读取8个字节（一个64位的数）到 `rdx` 寄存器。
4. `and edx, 0xf`：
   - 将 `edx`（`rdx`的低32位）与 `0xf`（十六进制数，等于十进制的15）进行位与操作，结果存回 `edx`。
5. `movzx edx, BYTE PTR [rdx+0x4024b0]`：
   - 将地址 `0x4024b0 + rdx` 处的一个字节读取出来，零扩展后存入 `edx` 寄存器。
6. `mov BYTE PTR [rsp+rax*1+0x10], dl`：
   - 将 `dl`（`rdx`的低8位）的值存储到地址 `rsp + rax + 0x10` 处。
7. `add rax, 0x1`：
   - 将 `rax` 寄存器的值增加1。
8. `cmp rax, 0x6`：
   - 将 `rax` 与6进行比较。
9. `jne 0x40108b <phase_5+41>`：
   - 如果 `rax` 不等于6，则跳转回地址 `0x40108b`，形成一个循环。

### 逻辑分析
这段代码似乎是在执行一个循环操作，可能是处理数组或字符串。它从一个地址（由 `rbx + rax` 确定）读取数据，进行一些处理，然后将结果存储在另一个地方。
循环的次数由 `rax` 控制，每次循环 `rax` 递增，直到达到6。这可能是对一个固定长度（如6个元素）的数组或字符串进行操作的典型循环结构。

~~~c
do {
    // 1. 读取并扩展字节
    ecx = read_byte_zero_extended(rbx + rax);

    // 2. 存储到栈顶
    *rsp = ecx & 0xFF;

    // 3. 从栈顶读取64位数
    rdx = *(QWORD *)rsp;

    // 4. 对rdx的低32位进行位与操作
    edx = rdx & 0xF;

    // 5. 读取并扩展另一个字节
    edx = read_byte_zero_extended(0x4024b0 + edx);

    // 6. 将结果存储到栈上特定位置
    *(BYTE *)(rsp + rax + 0x10) = edx & 0xFF;

    // 7. rax增加1
    rax += 1;

    // 8. 比较rax和6
} while (rax != 6);  // 9. 如果rax不等于6，则继续循环
~~~

~~~shell
   0x00000000004010d9 <+119>:   mov    rax,QWORD PTR [rsp+0x18]
   0x00000000004010de <+124>:   xor    rax,QWORD PTR fs:0x28
   0x00000000004010e7 <+133>:   je     0x4010ee <phase_5+140>
   0x00000000004010e9 <+135>:   call   0x400b30 <__stack_chk_fail@plt>
~~~

这一段汇编代码是用于栈保护的一部分，旨在防止缓冲区溢出和其他栈相关的安全漏洞。下面是每条指令的详细解释：
1. `mov rax, QWORD PTR [rsp+0x18]`：
   - 将栈上 `rsp + 0x18` 处的8字节（64位）数据加载到 `rax` 寄存器中。这通常是函数开始时存储的栈保护标记（或称为栈金丝雀值）。
2. `xor rax, QWORD PTR fs:0x28`：
   - 对 `rax` 寄存器的值和 `fs:0x28` 处的值（另一个栈保护标记）执行异或（XOR）操作。在函数开始时，相同的操作已经存储了一个栈保护标记，现在这里检查它是否被改变。`fs` 是一个特殊的段寄存器，用于指向特定的内存区域，这里用于存储栈保护相关的数据。
3. `je 0x4010ee <phase_5+140>`：
   - 如果 `xor` 操作的结果是0（意味着两个标记相同），则跳转到地址 `0x4010ee`。这表示栈未被破坏，程序可以安全地继续。
4. `call 0x400b30 <__stack_chk_fail@plt>`：
   - 如果 `xor` 操作的结果不为0，意味着栈保护标记被更改了（可能是由于缓冲区溢出或其他攻击），程序会调用 `__stack_chk_fail` 函数。这个函数通常会记录错误信息，并终止程序运行，以防止潜在的安全漏洞被利用。

### 总结
这段代码是一个运行时的安全检查，用来确认函数的栈空间没有被非法地修改。这是现代编译器用来增强程序安全性的一种常见机制。
通过检查在函数入口处保存的栈保护标记，确保在函数执行过程中这个值没有被篡改，从而防止栈溢出攻击。
如果检测到栈保护标记被改变，程序将执行安全相关的失败处理逻辑。


~~~shell
   0x0000000000401067 <+5>:     mov    rbx,rdi
   0x000000000040106a <+8>:     mov    rax,QWORD PTR fs:0x28
   0x0000000000401073 <+17>:    mov    QWORD PTR [rsp+0x18],rax
   0x0000000000401078 <+22>:    xor    eax,eax
   0x000000000040107a <+24>:    call   0x40131b <string_length>
   0x000000000040107f <+29>:    cmp    eax,0x6
   0x0000000000401082 <+32>:    je     0x4010d2 <phase_5+112>
   0x0000000000401084 <+34>:    call   0x40143a <explode_bomb>
   0x0000000000401089 <+39>:    jmp    0x4010d2 <phase_5+112>
~~~

1. `mov rbx, rdi`：
   - 将 `rdi` 寄存器的值复制到 `rbx` 寄存器。在x86-64架构中，`rdi` 通常用于传递函数的第一个参数，所以这可能是在保存传入的参数。
2. `mov rax, QWORD PTR fs:0x28`：
   - 从 `fs` 段寄存器指向的地址加上偏移 `0x28` 的位置读取8字节（64位）数据到 `rax`。这通常是读取当前的栈保护标记。
3. `mov QWORD PTR [rsp+0x18], rax`：
   - 将 `rax` 寄存器中的值（栈保护标记）存储到栈上 `rsp + 0x18` 的位置。
4. `xor eax, eax`：
   - 将 `eax` 寄存器清零。`xor` 操作一个寄存器与自身是一种常见的设置寄存器为0的方法。
5. `call 0x40131b <string_length>`：
   - 调用位于 `0x40131b` 的 `string_length` 函数。这个函数可能用于计算字符串的长度。
6. `cmp eax, 0x6`：
   - 比较 `eax` 寄存器的值（`string_length` 函数的返回值）和6。这可能是在检查字符串长度是否等于6。
7. `je 0x4010d2 <phase_5+112>`：
   - 如果 `eax` 等于6，则跳转到地址 `0x4010d2`。这表明只有在字符串长度等于6时才继续执行后续操作。
8. `call 0x40143a <explode_bomb>`：
   - 如果 `eax` 不等于6，调用 `explode_bomb` 函数。这个函数名暗示它可能用于处理错误或非法操作（如游戏中的“炸弹”）。
9. `jmp 0x4010d2 <phase_5+112>`：
   - 无条件跳转到地址 `0x4010d2`。

### 总结
这段代码的主要目的似乎是检查传入函数的字符串长度是否满足特定条件（长度等于6）。
如果不满足，程序将调用错误处理函数（如 `explode_bomb`）。这是一种常见的模式，用于验证输入数据是否符合预期的要求。

~~~shell
[ Legend: Modified register | Code | Heap | Stack | String ]
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── registers ────
$rax   : 0x6               
$rbx   : 0x000000006038c0  →  0x00363534333231 ("123456"?)
$rcx   : 0x36              
$rdx   : 0x72              
$rsp   : 0x007fffffffdbd8  →  0x000000004010c2  →  <phase_5+96> test eax, eax
$rbp   : 0x0               
$rsi   : 0x0000000040245e  →  0x00737265796c66 ("flyers"?)
$rdi   : 0x007fffffffdbf0  →  0x00726569756461 ("aduier"?)
$rip   : 0x00000000401338  →  <strings_not_equal+0> push r12
$r8    : 0x000000006038c0  →  0x00363534333231 ("123456"?)
$r9    : 0x7c              
$r10   : 0x0000000040069e  →  0x7300766e65746567 ("getenv"?)
$r11   : 0x246             
$r12   : 0x00000000400c90  →  <_start+0> xor ebp, ebp
$r13   : 0x007fffffffdd00  →  0x0000000000000002
$r14   : 0x0               
$r15   : 0x0               
$eflags: [ZERO carry PARITY adjust sign trap INTERRUPT direction overflow resume virtualx86 identification]
$cs: 0x33 $ss: 0x2b $ds: 0x00 $es: 0x00 $fs: 0x00 $gs: 0x00 
~~~

gef➤  x /s 0x4024b0
0x4024b0 <array.3449>:  "maduiersnfotvbylSo you think you can stop the bomb with ctrl-c, do you?"

这个程序比较复杂，重要语句注释写在上面的代码里，意思是读取一个长度为6的字符串，对于每个字符截取后四位数字，用来作为index，获取另一个字符串里对应的字符，并保存起来，产生一个新的长度为6的字符串，要求等于另一个字符串。

用来对比的字符串str2为“`flyers`”，我们发现flyers6个字符分别出现在str1的第9位，第15位，第14位，第5位，第6位，第7位，输入的字符串后四位的二进制只要分别表示9,15,14,5,6,7即可，翻查ASCII表，可以得到结果:
`ionefg`




~~~shell
gef➤  disas phase_6
Dump of assembler code for function phase_6:
=> 0x00000000004010f4 <+0>:     push   r14
   0x00000000004010f6 <+2>:     push   r13
   0x00000000004010f8 <+4>:     push   r12
   0x00000000004010fa <+6>:     push   rbp      
   0x00000000004010fb <+7>:     push   rbx

   0x00000000004010fc <+8>:     sub    rsp,0x50             # 从栈指针（rsp）减去80（0x50），为局部变量分配栈空间。
   # 参数传递 
   0x0000000000401100 <+12>:    mov    r13,rsp              # 这些指令将栈指针（rsp）的值复制到 r13 和 rsi 寄存器
   0x0000000000401103 <+15>:    mov    rsi,rsp              # rsi 通常用作函数的第二个参数，这表明 rsp 的值将被用作参数传递。

   0x0000000000401106 <+18>:    call   0x40145c <read_six_numbers>
   0x000000000040110b <+23>:    mov    r14,rsp              # 将栈指针的值复制到 r14 寄存器。(这可能是为了保存当前栈顶的位置？)
   0x000000000040110e <+26>:    mov    r12d,0x0             # 将 r12d（r12 寄存器的32位部分）清零。
   0x0000000000401114 <+32>:    mov    rbp,r13              
   0x0000000000401117 <+35>:    mov    eax,DWORD PTR [r13+0x0]  # 将 r13 寄存器加0的地址处的四字节数据加载到 eax 寄存器。(读取一个整数)
   0x000000000040111b <+39>:    sub    eax,0x1
   0x000000000040111e <+42>:    cmp    eax,0x5
   0x0000000000401121 <+45>:    jbe    0x401128 <phase_6+52>
   0x0000000000401123 <+47>:    call   0x40143a <explode_bomb> ====> 💣

   0x0000000000401128 <+52>:    add    r12d,0x1
   0x000000000040112c <+56>:    cmp    r12d,0x6
   0x0000000000401130 <+60>:    je     0x401153 <phase_6+95>
   0x0000000000401132 <+62>:    mov    ebx,r12d

   0x0000000000401135 <+65>:    movsxd rax,ebx
   0x0000000000401138 <+68>:    mov    eax,DWORD PTR [rsp+rax*4]
   0x000000000040113b <+71>:    cmp    DWORD PTR [rbp+0x0],eax
   0x000000000040113e <+74>:    jne    0x401145 <phase_6+81>
   0x0000000000401140 <+76>:    call   0x40143a <explode_bomb> ====> 💣
   0x0000000000401145 <+81>:    add    ebx,0x1
   0x0000000000401148 <+84>:    cmp    ebx,0x5
   0x000000000040114b <+87>:    jle    0x401135 <phase_6+65>

   0x000000000040114d <+89>:    add    r13,0x4
   0x0000000000401151 <+93>:    jmp    0x401114 <phase_6+32>
   
   0x0000000000401153 <+95>:    lea    rsi,[rsp+0x18]          # 使用 lea 指令加载栈指针 rsp 加 24（0x18）的地址到 rsi 寄存器
   0x0000000000401158 <+100>:   mov    rax,r14                 
   0x000000000040115b <+103>:   mov    ecx,0x7
   0x0000000000401160 <+108>:   mov    edx,ecx
   0x0000000000401162 <+110>:   sub    edx,DWORD PTR [rax]    # 从 rax 指向的地址读取一个四字节（DWORD）的值，从 edx(7)中减去这个值，然后将结果存储回 edx。
   0x0000000000401164 <+112>:   mov    DWORD PTR [rax],edx    # 将 edx 寄存器的值写回到 rax 指向的内存地址。
   0x0000000000401166 <+114>:   add    rax,0x4
   0x000000000040116a <+118>:   cmp    rax,rsi
   0x000000000040116d <+121>:   jne    0x401160 <phase_6+108>

   0x000000000040116f <+123>:   mov    esi,0x0
   0x0000000000401174 <+128>:   jmp    0x401197 <phase_6+163>
   0x0000000000401176 <+130>:   mov    rdx,QWORD PTR [rdx+0x8]
   0x000000000040117a <+134>:   add    eax,0x1
   0x000000000040117d <+137>:   cmp    eax,ecx
   0x000000000040117f <+139>:   jne    0x401176 <phase_6+130>
   0x0000000000401181 <+141>:   jmp    0x401188 <phase_6+148>

   0x0000000000401183 <+143>:   mov    edx,0x6032d0   #(node1)
   0x0000000000401188 <+148>:   mov    QWORD PTR [rsp+rsi*2+0x20],rdx
   0x000000000040118d <+153>:   add    rsi,0x4
   0x0000000000401191 <+157>:   cmp    rsi,0x18
   0x0000000000401195 <+161>:   je     0x4011ab <phase_6+183>

   0x0000000000401197 <+163>:   mov    ecx,DWORD PTR [rsp+rsi*1]
   0x000000000040119a <+166>:   cmp    ecx,0x1
   0x000000000040119d <+169>:   jle    0x401183 <phase_6+143>  #(ecx <= 1 then jump)
   0x000000000040119f <+171>:   mov    eax,0x1
   0x00000000004011a4 <+176>:   mov    edx,0x6032d0   # (node1)
   0x00000000004011a9 <+181>:   jmp    0x401176 <phase_6+130>
   0x00000000004011ab <+183>:   mov    rbx,QWORD PTR [rsp+0x20]
   0x00000000004011b0 <+188>:   lea    rax,[rsp+0x28]
   0x00000000004011b5 <+193>:   lea    rsi,[rsp+0x50]
   0x00000000004011ba <+198>:   mov    rcx,rbx
   0x00000000004011bd <+201>:   mov    rdx,QWORD PTR [rax]
   0x00000000004011c0 <+204>:   mov    QWORD PTR [rcx+0x8],rdx
   0x00000000004011c4 <+208>:   add    rax,0x8
   0x00000000004011c8 <+212>:   cmp    rax,rsi
   0x00000000004011cb <+215>:   je     0x4011d2 <phase_6+222>
   0x00000000004011cd <+217>:   mov    rcx,rdx
   0x00000000004011d0 <+220>:   jmp    0x4011bd <phase_6+201>
   0x00000000004011d2 <+222>:   mov    QWORD PTR [rdx+0x8],0x0
   0x00000000004011da <+230>:   mov    ebp,0x5
   0x00000000004011df <+235>:   mov    rax,QWORD PTR [rbx+0x8]
   0x00000000004011e3 <+239>:   mov    eax,DWORD PTR [rax]
   0x00000000004011e5 <+241>:   cmp    DWORD PTR [rbx],eax
   0x00000000004011e7 <+243>:   jge    0x4011ee <phase_6+250>
   0x00000000004011e9 <+245>:   call   0x40143a <explode_bomb>  ====> 💣
   0x00000000004011ee <+250>:   mov    rbx,QWORD PTR [rbx+0x8]
   0x00000000004011f2 <+254>:   sub    ebp,0x1
   0x00000000004011f5 <+257>:   jne    0x4011df <phase_6+235>
   
   0x00000000004011f7 <+259>:   add    rsp,0x50
   0x00000000004011fb <+263>:   pop    rbx
   0x00000000004011fc <+264>:   pop    rbp
   0x00000000004011fd <+265>:   pop    r12
   0x00000000004011ff <+267>:   pop    r13
   0x0000000000401201 <+269>:   pop    r14
   0x0000000000401203 <+271>:   ret    
End of assembler dump.
~~~

这个代码非常长，我们可以把它划分为几个部分，每个部分完成不同的功能，即不同的限制条件，第一部分是+23-+60，要求每个数小于等于6，第二部分+60-+95，是两个循环，用来判断数字两两之间不相等，第三部分是+108-128，将读来的6个数与7求差，再存入原来的地方，第四部分130-181，这个部分有一些复杂，相当于是在栈上存了些地址，可以发现+130 的代码为mov 0x8(%rdx),%rdx，这其实就是链表的形式，每次读取next地址，这段代码的意思是，用第三部分与7求出来的差值重新安排链表节点，1对应第一个节点，2对应第二个节点，依次类推；第五部分+183-257，用来判断重新构造出的链表里的数字满足严格递减。

`node 1` 0x14c
`node 2` 0xa8
`node 3` 0x39c
`node 4` 0x2bc
`node 5` 0x1dd
`node 6` 0x1bb
正确的递减顺序应该是 3 4 5 6 1 2 

我们利用gdb命令: x/24w 0x6032d0（每个节点占用了4个w）发现序号1-6的节点对应的数字为：332，168，924，691，477，413，为了使其严格递减，序号应该是：3，4，5，6，1，2，考虑到我们之前用7对传入的数字作差，为了得到这个序列，传入的数字应该是，4，3，2，1，6，5这就是我们最终的结果。

总结下，这个函数是传入一个互不相等的，由6个数组成的数组，然后每个数与7作差，根据作差的结果，每个数对应原来链表的第一个节点，重新排序一个链表，要求这个链表的数值降序排列

# 彩蛋求解
~~~c
86          /* I guess this is too easy so far.  Some more complex code will
87           * confuse people. */
88          input = read_line();
89          phase_3(input);
90          phase_defused();
91          printf("Halfway there!\n");
92
93          /* Oh yeah?  Well, how good is your math?  Try on this saucy problem! */
94          input = read_line();
95          phase_4(input);
96          phase_defused();
97          printf("So you got that one.  Try this one.\n");
98          
99          /* Round and 'round in memory we go, where we stop, the bomb blows! */
100         input = read_line();
(gdb) list 100,150
100         input = read_line();
101         phase_5(input);
102         phase_defused();
103         printf("Good work!  On to the next...\n");
104
105         /* This phase will never be used, since no one will get past the
106          * earlier ones.  But just in case, make this one extra hard. */
107         input = read_line();
108         phase_6(input);
109         phase_defused();
110
111         /* Wow, they got it!  But isn't something... missing?  Perhaps
112          * something they overlooked?  Mua ha ha ha ha! */
113         
114         return 0;
115     }
~~~

~~~shell
(gdb) disas secret_phase
Dump of assembler code for function secret_phase:
   0x0000000000401242 <+0>:     push   %rbx
   0x0000000000401243 <+1>:     callq  0x40149e <read_line>
   0x0000000000401248 <+6>:     mov    $0xa,%edx
   0x000000000040124d <+11>:    mov    $0x0,%esi
   0x0000000000401252 <+16>:    mov    %rax,%rdi
   0x0000000000401255 <+19>:    callq  0x400bd0 <strtol@plt>
   0x000000000040125a <+24>:    mov    %rax,%rbx
   0x000000000040125d <+27>:    lea    -0x1(%rax),%eax
   0x0000000000401260 <+30>:    cmp    $0x3e8,%eax
   0x0000000000401265 <+35>:    jbe    0x40126c <secret_phase+42>
   0x0000000000401267 <+37>:    callq  0x40143a <explode_bomb>
   0x000000000040126c <+42>:    mov    %ebx,%esi
   0x000000000040126e <+44>:    mov    $0x6030f0,%edi
   0x0000000000401273 <+49>:    callq  0x401204 <fun7>
   0x0000000000401278 <+54>:    cmp    $0x2,%eax
   0x000000000040127b <+57>:    je     0x401282 <secret_phase+64>
   0x000000000040127d <+59>:    callq  0x40143a <explode_bomb>
   0x0000000000401282 <+64>:    mov    $0x402438,%edi
   0x0000000000401287 <+69>:    callq  0x400b10 <puts@plt>
   0x000000000040128c <+74>:    callq  0x4015c4 <phase_defused>
   0x0000000000401291 <+79>:    pop    %rbx
   0x0000000000401292 <+80>:    retq   
End of assembler dump.
~~~

~~~shell
Dump of assembler code for function phase_defused:
   0x00000000004015c4 <+0>:     sub    $0x78,%rsp
   0x00000000004015c8 <+4>:     mov    %fs:0x28,%rax
   0x00000000004015d1 <+13>:    mov    %rax,0x68(%rsp)
   0x00000000004015d6 <+18>:    xor    %eax,%eax
   0x00000000004015d8 <+20>:    cmpl   $0x6,0x202181(%rip)        # 0x603760 <num_input_strings>
   0x00000000004015df <+27>:    jne    0x40163f <phase_defused+123>
   0x00000000004015e1 <+29>:    lea    0x10(%rsp),%r8
   0x00000000004015e6 <+34>:    lea    0xc(%rsp),%rcx
   0x00000000004015eb <+39>:    lea    0x8(%rsp),%rdx
   0x00000000004015f0 <+44>:    mov    $0x402619,%esi
   0x00000000004015f5 <+49>:    mov    $0x603870,%edi
   0x00000000004015fa <+54>:    callq  0x400bf0 <__isoc99_sscanf@plt>
   0x00000000004015ff <+59>:    cmp    $0x3,%eax
   0x0000000000401602 <+62>:    jne    0x401635 <phase_defused+113>
   0x0000000000401604 <+64>:    mov    $0x402622,%esi
   0x0000000000401609 <+69>:    lea    0x10(%rsp),%rdi
   0x000000000040160e <+74>:    callq  0x401338 <strings_not_equal>
   0x0000000000401613 <+79>:    test   %eax,%eax
   0x0000000000401615 <+81>:    jne    0x401635 <phase_defused+113>
   0x0000000000401617 <+83>:    mov    $0x4024f8,%edi
   0x000000000040161c <+88>:    callq  0x400b10 <puts@plt>
   0x0000000000401621 <+93>:    mov    $0x402520,%edi
   0x0000000000401626 <+98>:    callq  0x400b10 <puts@plt>
   0x000000000040162b <+103>:   mov    $0x0,%eax
   0x0000000000401630 <+108>:   callq  0x401242 <secret_phase>
   0x0000000000401635 <+113>:   mov    $0x402558,%edi
   0x000000000040163a <+118>:   callq  0x400b10 <puts@plt>
   0x000000000040163f <+123>:   mov    0x68(%rsp),%rax
   0x0000000000401644 <+128>:   xor    %fs:0x28,%rax
   0x000000000040164d <+137>:   je     0x401654 <phase_defused+144>
   0x000000000040164f <+139>:   callq  0x400b30 <__stack_chk_fail@plt>
   0x0000000000401654 <+144>:   add    $0x78,%rsp
   0x0000000000401658 <+148>:   retq   
End of assembler dump.
~~~

祭出IDA大法
~~~c
unsigned __int64 __fastcall phase_defused(__int64 a1, __int64 a2, __int64 a3, __int64 a4, __int64 a5, __int64 a6)
{
  __int64 v7; // [rsp+0h] [rbp-78h]
  __int64 v8; // [rsp+8h] [rbp-70h] BYREF
  char v9[88]; // [rsp+10h] [rbp-68h] BYREF
  unsigned __int64 v10; // [rsp+68h] [rbp-10h]

  v10 = __readfsqword(0x28u);
  if ( num_input_strings == 6 )
  {
    if ( (unsigned int)__isoc99_sscanf(
                         (__int64)&unk_603870,
                         (__int64)"%d %d %s",
                         (__int64)&v8,
                         (__int64)&v8 + 4,
                         (__int64)v9,
                         a6,
                         v7,
                         v8) == 3
      && !(unsigned int)strings_not_equal(v9, "DrEvil") )
    {
      puts("Curses, you've found the secret phase!");
      puts("But finding it and solving it are quite different...");
      secret_phase();
    }
    puts("Congratulations! You've defused the bomb!");
  }
  return __readfsqword(0x28u) ^ v10;
}

// fun7
__int64 __fastcall fun7(__int64 a1, __int64 a2)
{
  __int64 result; // rax

  if ( !a1 )
    return 0xFFFFFFFFLL;
  if ( *(_DWORD *)a1 > (int)a2 )
    return 2 * (unsigned int)fun7(*(_QWORD *)(a1 + 8), a2);
  result = 0LL;
  if ( *(_DWORD *)a1 != (_DWORD)a2 )
    return 2 * (unsigned int)fun7(*(_QWORD *)(a1 + 16), a2) + 1;
  return result;
}


__int64 secret_phase()
{
  const char *line; // rdi
  unsigned int v1; // ebx

  line = (const char *)read_line();
  v1 = strtol(line, 0LL, 10);
  if ( v1 - 1 > 0x3E8 )
    explode_bomb(line, 0LL);
  if ( (unsigned int)fun7(&n1, v1) != 2 )
    explode_bomb(&n1, v1);
  puts("Wow! You've defused the secret stage!");
  return phase_defused();
}
~~~

### 函数 `fun7`
`fun7` 是一个递归函数，它接受两个参数 `a1` 和 `a2`。这个函数的工作方式是：
1. **Base Case**:
   - 如果 `a1` 为空（`!a1`），函数返回 `-1`（在 C 语言中，`0xFFFFFFFFLL` 是 `unsigned long long` 类型的 `-1`）。
2. **Recursive Cases**:
   - 如果 `a1` 所指向的值大于 `a2`，则递归调用 `fun7`，传入 `a1 + 8`（可能是树的左子节点）和 `a2`，返回值乘以 2。
   - 如果 `a1` 所指向的值不等于 `a2`，则递归调用 `fun7`，传入 `a1 + 16`（可能是树的右子节点）和 `a2`，返回值乘以 2 并加 1。
3. **Return 0**:
   - 如果 `a1` 所指向的值等于 `a2`，则函数返回 `0`。

### 函数 `secret_phase`
`secret_phase` 函数是用来处理 "秘密阶段" 的，它的工作方式如下：
1. 读取一行输入，将其转换为一个整数（`strtol` 函数）。
2. 检查输入的整数减 1 是否大于 1000（`0x3E8`）。如果是，触发 `explode_bomb`。
3. 调用 `fun7` 函数，传入一个地址（`&n1`，可能是某种数据结构的起点，如一棵树的根节点）和读取的整数。如果 `fun7` 的返回值不是 `2`，触发 `explode_bomb`。
4. 如果没有触发炸弹，输出成功信息并调用 `phase_defused` 函数。

### 总结

这个代码片段看起来是一种挑战型程序的一部分，可能是用来测试用户逻辑和编程能力的。用户需要输入一个特定的数字，这个数字会被用在一个递归搜索算法中（`fun7`）。如果输入的数字不能使 `fun7` 函数返回 `2`，程序会触发一个“炸弹”。这意味着要成功通过这个阶段，用户需要理解 `fun7` 函数的工作原理，并找出能产生正确结果的输入值。

(master)⚡ [8] % ./bomb psol.txt                                                                       ~/new_space/CSAPP_Lab/Boom/bomb
Welcome to my fiendish little bomb. You have 6 phases with
which to blow yourself up. Have a nice day!
Phase 1 defused. How about the next one?
That's number 2.  Keep going!
Halfway there!
So you got that one.  Try this one.
Good work!  On to the next...
Curses, you've found the secret phase!
But finding it and solving it are quite different...
20
Wow! You've defused the secret stage!
Congratulations! You've defused the bomb!
(master)⚡ %                     


### 1. `phase_defused` Function:

```c
unsigned __int64 phase_defused() {
    __int64 tempVar1; // Temporary variable
    __int64 parsedValues; // Parsed values from the input
    char inputString[88]; // Buffer for the input string
    unsigned __int64 stackCanary = __readfsqword(0x28u);

    if (num_input_strings == 6) {
        if (__isoc99_sscanf(&inputSource, "%d %d %s", &parsedValues, &parsedValues + 4, inputString) == 3
            && !strings_not_equal(inputString, "DrEvil")) {
            puts("Curses, you've found the secret phase!");
            puts("But finding it and solving it are quite different...");
            secret_phase();
        }
        puts("Congratulations! You've defused the bomb!");
    }
    return __readfsqword(0x28u) ^ stackCanary;
}
```

### 2. `fun7` Function:

```c
__int64 fun7(__int64 nodePtr, __int64 value) {
    if (!nodePtr)
        return -1;
    
    if (*(int *)nodePtr > value)
        return 2 * fun7(*(long long *)(nodePtr + 8), value);
    
    if (*(int *)nodePtr != value)
        return 2 * fun7(*(long long *)(nodePtr + 16), value) + 1;

    return 0;
}
```

### 3. `secret_phase` Function:

```c
__int64 secret_phase() {
    const char *userInput;
    unsigned int num;

    userInput = read_line();
    num = strtol(userInput, NULL, 10);

    if (num - 1 > 1000)
        explode_bomb(userInput);

    if (fun7(&rootNode, num) != 2)
        explode_bomb(&rootNode, num);

    puts("Wow! You've defused the secret stage!");
    return phase_defused();
}
```

~~~shell
gef➤  x /140 0x000000006030f0
0x6030f0 <n1>:  36      0       6304016 0
0x603100 <n1+16>:       6304048 0       0       0
0x603110 <n21>: 8       0       6304144 0
0x603120 <n21+16>:      6304080 0       0       0
0x603130 <n22>: 50      0       6304112 0
0x603140 <n22+16>:      6304176 0       0       0
0x603150 <n32>: 22      0       6304368 0
0x603160 <n32+16>:      6304304 0       0       0
0x603170 <n33>: 45      0       6304208 0
0x603180 <n33+16>:      6304400 0       0       0
0x603190 <n31>: 6       0       6304240 0
0x6031a0 <n31+16>:      6304336 0       0       0
0x6031b0 <n34>: 107     0       6304272 0
0x6031c0 <n34+16>:      6304432 0       0       0
0x6031d0 <n45>: 40      0       0       0
0x6031e0 <n45+16>:      0       0       0       0
0x6031f0 <n41>: 1       0       0       0
0x603200 <n41+16>:      0       0       0       0
0x603210 <n47>: 99      0       0       0
0x603220 <n47+16>:      0       0       0       0
0x603230 <n44>: 35      0       0       0
0x603240 <n44+16>:      0       0       0       0
0x603250 <n42>: 7       0       0       0
0x603260 <n42+16>:      0       0       0       0
0x603270 <n43>: 20      0       0       0
0x603280 <n43+16>:      0       0       0       0
0x603290 <n46>: 47      0       0       0
0x6032a0 <n46+16>:      0       0       0       0
0x6032b0 <n48>: 1001    0       0       0
0x6032c0 <n48+16>:      0       0       0       0
~~~

这段输出显示的是一种树形数据结构，具体来说，很可能是一棵二叉搜索树（Binary Search Tree, BST）。
在这种结构中，每个节点通常包含几个元素：一个键值（key），指向左子节点的指针，和指向右子节点的指针。

从提供的内存转储中，我们可以看到以下模式：

- **节点的结构**：每个节点包含四个元素。以 `0x6030f0 <n1>` 为例，它的元素是 `36 0 6304016 0`。这里，`36` 是节点的键值，而 `6304016` 可能是指向另一个节点的指针。`0` 通常表示空指针或无效数据。

- **节点的布局**：每个节点似乎按照 `16` 字节的间隔排列，这表明每个节点可能由四个 `4` 字节的部分组成。前两个部分可能是键值和保留空间，后两个部分则是左右子节点的指针。

- **子节点指针**：在有些节点中，例如 `n1`，后两个数值中的一个（`6304016`）非零，这表示该节点有子节点。这个值应该是指向子节点的内存地址。

这个内存转储提供了树的物理布局，但没有直接告诉我们树的逻辑结构（即节点如何相互连接）。要理解树的逻辑结构，通常需要通过遍历树（编程中的递归或迭代方法）来确定。
结合你之前提供的 `fun7` 函数，该函数的行为进一步证实了这是一种二叉搜索树，其中 `fun7` 递归地查找给定值，并根据找到的路径返回特定的结果。