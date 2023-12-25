# Boob Lab Solution

## GDB
1. https://solar1s.t0nkov.site/posts/summary_of_gdb_commands_for_x86-64_systems/
2. https://csapp.cs.cmu.edu/2e/docs/gdbnotes-x86-64.pdf
3. https://evilpan.com/2020/09/13/gdb-tips/#%E9%99%84%E5%BD%95-gdb%E5%91%BD%E4%BB%A4%E8%A1%A8 
4. gef: `$ bash -c "$(curl -fsSL https://gef.blah.cat/sh)"`

## Objdump
1.  查看反汇编代码
    - **命令**：`objdump -d <binary>`
    - **说明**：此命令用于查看二进制文件的反汇编代码。它会显示程序中每个函数的汇编代码。
2. 查看符号表
    - **命令**：`objdump -t <binary>`
    - **说明**：此命令用于显示目标文件的符号表，包括函数、变量等的地址和信息。 

## 函数调用（System V AMD64 ABI调用约定）
~~~txt
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

## boom.c
phase_1 ~ phase_6
~~~c
    /* Hmm...  Six phases must be more secure than one phase! */
    input = read_line(); /* Get input                   */
    phase_1(input);      /* Run the phase               */
    phase_defused();     /* Drat!  They figured it out!
                          * Let me know how they did it. */
    printf("Phase 1 defused. How about the next one?\n");
~~~

## phase_1
~~~shell
gef➤  disas phase_1
Dump of assembler code for function phase_1:
=> 0x0000000000400ee0 <+0>:     sub    rsp,0x8  
   0x0000000000400ee4 <+4>:     mov    esi,0x402400
   0x0000000000400ee9 <+9>:     call   0x401338 <strings_not_equal>
   0x0000000000400eee <+14>:    test   eax,eax
   0x0000000000400ef0 <+16>:    je     0x400ef7 <phase_1+23>
   0x0000000000400ef2 <+18>:    call   0x40143a <explode_bomb>
   0x0000000000400ef7 <+23>:    add    rsp,0x8
   0x0000000000400efb <+27>:    ret    
End of assembler dump.
~~~

~~~shell
gef➤  x /s $rsi
0x402400:       "Border relations with Canada have never been better."
~~~

~~~c
__int64 __fastcall phase_1(_BYTE *a1)
{
  __int64 result; // rax

  result = strings_not_equal(a1, "Border relations with Canada have never been better.");
  if ( (_DWORD)result )
    explode_bomb();
  return result;
}
~~~

## phase_2
~~~shell
gef➤  disas phase_2
Dump of assembler code for function phase_2:
=> 0x0000000000400efc <+0>:     push   rbp
   0x0000000000400efd <+1>:     push   rbx
   0x0000000000400efe <+2>:     sub    rsp,0x28
   0x0000000000400f02 <+6>:     mov    rsi,rsp
   0x0000000000400f05 <+9>:     call   0x40145c <read_six_numbers>
   0x0000000000400f0a <+14>:    cmp    DWORD PTR [rsp],0x1
   0x0000000000400f0e <+18>:    je     0x400f30 <phase_2+52>
   0x0000000000400f10 <+20>:    call   0x40143a <explode_bomb>
   0x0000000000400f15 <+25>:    jmp    0x400f30 <phase_2+52>
   0x0000000000400f17 <+27>:    mov    eax,DWORD PTR [rbx-0x4]
   0x0000000000400f1a <+30>:    add    eax,eax
   0x0000000000400f1c <+32>:    cmp    DWORD PTR [rbx],eax
   0x0000000000400f1e <+34>:    je     0x400f25 <phase_2+41>
   0x0000000000400f20 <+36>:    call   0x40143a <explode_bomb>
   0x0000000000400f25 <+41>:    add    rbx,0x4
   0x0000000000400f29 <+45>:    cmp    rbx,rbp
   0x0000000000400f2c <+48>:    jne    0x400f17 <phase_2+27>
   0x0000000000400f2e <+50>:    jmp    0x400f3c <phase_2+64>
   0x0000000000400f30 <+52>:    lea    rbx,[rsp+0x4]
   0x0000000000400f35 <+57>:    lea    rbp,[rsp+0x18]
   0x0000000000400f3a <+62>:    jmp    0x400f17 <phase_2+27>
   0x0000000000400f3c <+64>:    add    rsp,0x28
   0x0000000000400f40 <+68>:    pop    rbx
   0x0000000000400f41 <+69>:    pop    rbp
   0x0000000000400f42 <+70>:    ret    
End of assembler dump.
~~~

~~~shell
gef➤  disas read_six_numbers 
Dump of assembler code for function read_six_numbers:
   0x000000000040145c <+0>:     sub    rsp,0x18
   0x0000000000401460 <+4>:     mov    rdx,rsi
   0x0000000000401463 <+7>:     lea    rcx,[rsi+0x4]
   0x0000000000401467 <+11>:    lea    rax,[rsi+0x14]
   0x000000000040146b <+15>:    mov    QWORD PTR [rsp+0x8],rax
   0x0000000000401470 <+20>:    lea    rax,[rsi+0x10]
   0x0000000000401474 <+24>:    mov    QWORD PTR [rsp],rax
   0x0000000000401478 <+28>:    lea    r9,[rsi+0xc]
   0x000000000040147c <+32>:    lea    r8,[rsi+0x8]
   0x0000000000401480 <+36>:    mov    esi,0x4025c3
   0x0000000000401485 <+41>:    mov    eax,0x0
   0x000000000040148a <+46>:    call   0x400bf0 <__isoc99_sscanf@plt>
   0x000000000040148f <+51>:    cmp    eax,0x5
   0x0000000000401492 <+54>:    jg     0x401499 <read_six_numbers+61>
   0x0000000000401494 <+56>:    call   0x40143a <explode_bomb>
   0x0000000000401499 <+61>:    add    rsp,0x18
   0x000000000040149d <+65>:    ret    
End of assembler dump.
~~~

## phase_3

~~~shell
gef➤  disas phase_3
Dump of assembler code for function phase_3:
   0x0000000000400f43 <+0>:     sub    rsp,0x18
   0x0000000000400f47 <+4>:     lea    rcx,[rsp+0xc]
   0x0000000000400f4c <+9>:     lea    rdx,[rsp+0x8]
   0x0000000000400f51 <+14>:    mov    esi,0x4025cf
   0x0000000000400f56 <+19>:    mov    eax,0x0
   0x0000000000400f5b <+24>:    call   0x400bf0 <__isoc99_sscanf@plt>
   0x0000000000400f60 <+29>:    cmp    eax,0x1
   0x0000000000400f63 <+32>:    jg     0x400f6a <phase_3+39>
   0x0000000000400f65 <+34>:    call   0x40143a <explode_bomb>
   0x0000000000400f6a <+39>:    cmp    DWORD PTR [rsp+0x8],0x7
   0x0000000000400f6f <+44>:    ja     0x400fad <phase_3+106>
   0x0000000000400f71 <+46>:    mov    eax,DWORD PTR [rsp+0x8]
   0x0000000000400f75 <+50>:    jmp    QWORD PTR [rax*8+0x402470]
   0x0000000000400f7c <+57>:    mov    eax,0xcf
   0x0000000000400f81 <+62>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400f83 <+64>:    mov    eax,0x2c3
   0x0000000000400f88 <+69>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400f8a <+71>:    mov    eax,0x100
   0x0000000000400f8f <+76>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400f91 <+78>:    mov    eax,0x185
   0x0000000000400f96 <+83>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400f98 <+85>:    mov    eax,0xce
   0x0000000000400f9d <+90>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400f9f <+92>:    mov    eax,0x2aa
   0x0000000000400fa4 <+97>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400fa6 <+99>:    mov    eax,0x147
   0x0000000000400fab <+104>:   jmp    0x400fbe <phase_3+123>
   0x0000000000400fad <+106>:   call   0x40143a <explode_bomb>
   0x0000000000400fb2 <+111>:   mov    eax,0x0
   0x0000000000400fb7 <+116>:   jmp    0x400fbe <phase_3+123>
   0x0000000000400fb9 <+118>:   mov    eax,0x137
   0x0000000000400fbe <+123>:   cmp    eax,DWORD PTR [rsp+0xc]
   0x0000000000400fc2 <+127>:   je     0x400fc9 <phase_3+134>
   0x0000000000400fc4 <+129>:   call   0x40143a <explode_bomb>
   0x0000000000400fc9 <+134>:   add    rsp,0x18
   0x0000000000400fcd <+138>:   ret    
End of assembler dump.
~~~

## phase_4

~~~shell
gef➤  disas phase_4
Dump of assembler code for function phase_4:
   0x000000000040100c <+0>:     sub    rsp,0x18
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
~~~

~~~shell
gef➤  disas func4
Dump of assembler code for function func4:
   0x0000000000400fce <+0>:     sub    rsp,0x8
   0x0000000000400fd2 <+4>:     mov    eax,edx
   0x0000000000400fd4 <+6>:     sub    eax,esi
   0x0000000000400fd6 <+8>:     mov    ecx,eax
   0x0000000000400fd8 <+10>:    shr    ecx,0x1f # 逻辑右移31位，用于确定正负号
   0x0000000000400fdb <+13>:    add    eax,ecx
   0x0000000000400fdd <+15>:    sar    eax,1    # 算术右移1位，对前一步骤的结果 / 2
   0x0000000000400fdf <+17>:    lea    ecx,[rax+rsi*1]
   0x0000000000400fe2 <+20>:    cmp    ecx,edi
   0x0000000000400fe4 <+22>:    jle    0x400ff2 <func4+36> # $ecx <= $edi 则跳转 
   0x0000000000400fe6 <+24>:    lea    edx,[rcx-0x1]
   0x0000000000400fe9 <+27>:    call   0x400fce <func4> # 递归调用 func4
   0x0000000000400fee <+32>:    add    eax,eax # eax 的值加倍
   0x0000000000400ff0 <+34>:    jmp    0x401007 <func4+57> 
   0x0000000000400ff2 <+36>:    mov    eax,0x0
   0x0000000000400ff7 <+41>:    cmp    ecx,edi
   0x0000000000400ff9 <+43>:    jge    0x401007 <func4+57>
   0x0000000000400ffb <+45>:    lea    esi,[rcx+0x1]
   0x0000000000400ffe <+48>:    call   0x400fce <func4> # 再次递归调用 func4
   0x0000000000401003 <+53>:    lea    eax,[rax+rax*1+0x1]
   0x0000000000401007 <+57>:    add    rsp,0x8          # 恢复栈指针 + 返回
   0x000000000040100b <+61>:    ret    
End of assembler dump.
~~~

## phase_5

~~~shell
gef➤  disas phase_5
Dump of assembler code for function phase_5:
   0x0000000000401062 <+0>:     push   rbx
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

## phase_6

~~~shell
gef➤  disas phase_6
Dump of assembler code for function phase_6:
   0x00000000004010f4 <+0>:     push   r14
   0x00000000004010f6 <+2>:     push   r13
   0x00000000004010f8 <+4>:     push   r12
   0x00000000004010fa <+6>:     push   rbp
   0x00000000004010fb <+7>:     push   rbx
   0x00000000004010fc <+8>:     sub    rsp,0x50
   0x0000000000401100 <+12>:    mov    r13,rsp
   0x0000000000401103 <+15>:    mov    rsi,rsp
   0x0000000000401106 <+18>:    call   0x40145c <read_six_numbers>
   0x000000000040110b <+23>:    mov    r14,rsp
   0x000000000040110e <+26>:    mov    r12d,0x0
   0x0000000000401114 <+32>:    mov    rbp,r13
   0x0000000000401117 <+35>:    mov    eax,DWORD PTR [r13+0x0]
   0x000000000040111b <+39>:    sub    eax,0x1
   0x000000000040111e <+42>:    cmp    eax,0x5
   0x0000000000401121 <+45>:    jbe    0x401128 <phase_6+52>
   0x0000000000401123 <+47>:    call   0x40143a <explode_bomb>
   0x0000000000401128 <+52>:    add    r12d,0x1
   0x000000000040112c <+56>:    cmp    r12d,0x6
   0x0000000000401130 <+60>:    je     0x401153 <phase_6+95>
   0x0000000000401132 <+62>:    mov    ebx,r12d
   0x0000000000401135 <+65>:    movsxd rax,ebx
   0x0000000000401138 <+68>:    mov    eax,DWORD PTR [rsp+rax*4]
   0x000000000040113b <+71>:    cmp    DWORD PTR [rbp+0x0],eax
   0x000000000040113e <+74>:    jne    0x401145 <phase_6+81>
   0x0000000000401140 <+76>:    call   0x40143a <explode_bomb>
   0x0000000000401145 <+81>:    add    ebx,0x1
   0x0000000000401148 <+84>:    cmp    ebx,0x5
   0x000000000040114b <+87>:    jle    0x401135 <phase_6+65>
   0x000000000040114d <+89>:    add    r13,0x4
   0x0000000000401151 <+93>:    jmp    0x401114 <phase_6+32>
   0x0000000000401153 <+95>:    lea    rsi,[rsp+0x18]
   0x0000000000401158 <+100>:   mov    rax,r14
   0x000000000040115b <+103>:   mov    ecx,0x7
   0x0000000000401160 <+108>:   mov    edx,ecx
   0x0000000000401162 <+110>:   sub    edx,DWORD PTR [rax]
   0x0000000000401164 <+112>:   mov    DWORD PTR [rax],edx
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
   0x0000000000401183 <+143>:   mov    edx,0x6032d0
   0x0000000000401188 <+148>:   mov    QWORD PTR [rsp+rsi*2+0x20],rdx
   0x000000000040118d <+153>:   add    rsi,0x4
   0x0000000000401191 <+157>:   cmp    rsi,0x18
   0x0000000000401195 <+161>:   je     0x4011ab <phase_6+183>
   0x0000000000401197 <+163>:   mov    ecx,DWORD PTR [rsp+rsi*1]
   0x000000000040119a <+166>:   cmp    ecx,0x1
   0x000000000040119d <+169>:   jle    0x401183 <phase_6+143>
   0x000000000040119f <+171>:   mov    eax,0x1
   0x00000000004011a4 <+176>:   mov    edx,0x6032d0
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
   0x00000000004011e9 <+245>:   call   0x40143a <explode_bomb>
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


# Reference

1. https://hansimov.gitbook.io/csapp/labs/bomb-lab
2. 

