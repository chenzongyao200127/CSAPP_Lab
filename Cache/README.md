#####################################################################
# CS:APP Cache Lab
# Directions to Instructors
#
# Copyright (c) 2013, R. Bryant and D. O'Hallaron, All rights reserved.
######################################################################

This directory contains the files that you will need to run the CS:APP
cache lab, which develops the student's understanding of caches.

************
1. Overview
************

In this lab, the student works on two C files called csim.c and
trans.c.  There are two parts: Part (a) involves implementing a cache
simulator in csim.c. Part (b) involves writing a function that
computes the transpose of a given matrix in trans.c, with the goal of
minimizing the number misses on a simulated cache.

Each time a student with login "foo" compiles their work, the Makefile
automatically generates a handin file, called foo-handin.tar, that
contains the csim.c and trans.c file. Students hand this tar file in
to the instructor.

The driver program (driver.py) evaluates the correctness of the cache
simulator in csim.c, and the performance and correctness of the
transpose functions in trans.c. See the writeup for details.

Requirements:
- The lab must be done on a 64-bit x86-64 system. 
- The driver requires a version of Valgrind (http://valgrind.org) that
supports the "--tool=lackey" option.


********
2. Files
********
Makefile              Builds the entire lab
README                This file
grade/                Autograding scripts for the instructor
src/                  Source code of this lab
cachelab-handout/     Handout directory that goes to the students. This directory
                      is completely generated from files in the src directory
                      Never put any hard state in this directory.
cachelab-handout.tar  Tar file of handout directory that goes out to the students.
writeup/              Latex writeup about the lab. Modify to reflect your 
                      environment.

*******************
3. Building the Lab
*******************

To build the default version of the lab, modify the Latex lab writeup
in ./writeup/cachelab.tex for your environment. Then type the following
in the current directory:

        unix> make clean
        unix> make

This will build the cachelab-handout/ directory and its
cachelab-handout.tar archive that you can handout to students.
The command:

	unix> make dist DEST=<DIR>

will copy the tarfile and copies of the writeup to directory <DIR>,
where the students can access it.

*********************
4. Handing in the Lab
*********************

Each time a student with login "foo" compiles their work, the Makefile
automatically generates a handin file called foo-handin.tar. If you
want to autograde the handins, collect all of the student handin tar
files in the ./grade/handin directory.

**********************
5. Autograding the Lab
**********************

You can run the driver program manually on each student's handin, or
you can use the autograding scripts in ./grade to automatically grade
each of the .tar files in the handin/ directory.

See ./grade/README for instructions.


# 实验简介

CMU 用该实验替代 Performance Lab。学生编写一个通用缓存模拟器，然后优化一个小矩阵转置内核，从而最小化模拟缓存上的未命中次数。
这个实验室使用 Valgrind 工具来生成地址踪迹（address traces）。

注意：该实验室必须运行在 64位 x86-64 系统上。

1. **通用缓存模拟器**: 缓存是计算机存储体系结构中的一个重要组件，用于临时存储频繁访问的数据以提高访问速度。缓存模拟器是一个程序，用于模拟计算机硬件中缓存的工作方式。通过这样的模拟器，学生可以了解缓存如何处理数据，特别是如何加载和存储数据，以及缓存未命中（当所需数据不在缓存中时）的情况。

2. **小矩阵转置内核的优化**: 矩阵转置是一种基本的数学操作，涉及将矩阵的行转换为列。在计算机编程中，这个操作可以用于测试和优化数据的内存访问模式。在这个实验中，学生需要编写代码来转置一个小矩阵，并优化这个过程，以减少在模拟缓存上的未命中次数。这通常涉及到优化数据访问模式，使得数据访问更符合缓存的工作原理。

3. **Valgrind 工具**: Valgrind 是一个编程工具套件，用于内存调试、内存泄漏检测以及性能分析。在这个实验中，Valgrind 被用来生成地址踪迹，即程序运行过程中访问内存的地址记录。这些地址踪迹对于理解程序如何与内存交互非常有帮助，特别是在分析缓存行为时。

4. **64位 x86-64 系统**: x86-64 是一种广泛使用的计算机架构，支持64位处理。64位系统可以使用更大的内存地址空间，允许程序访问更多的内存。这个实验要求在64位 x86-64 系统上运行，可能是因为它需要处理较大的内存地址空间或者是为了兼容特定的性能分析工具。

综上所述，这个实验室活动旨在通过实践方式教授学生缓存原理、内存访问模式优化、程序性能分析，以及64位系统架构的相关知识。
通过构建缓存模拟器和优化矩阵转置操作，学生能够深入理解和应用这些重要的计算机科学概念。

(base) ╭─czy@czy-ThinkCentre ~/new_space/CSAPP_Lab/Cache ‹master› 
╰─$ uname -m
x86_64


这个目录包含了运行 CS:APP 缓存实验所需的文件，该实验旨在帮助学生理解缓存。

************
1. 概览
************

在这个实验中，学生需要处理两个名为 csim.c 和 trans.c 的 C 语言文件。
实验分为两部分：
1. 第一部分 (a) 要求在 csim.c 中实现一个缓存模拟器。
2. 第二部分 (b) 要求在 trans.c 中编写一个计算给定矩阵转置的函数，目标是最小化模拟缓存上的未命中次数。

每当学生（例如登录名为 "foo"）编译他们的工作时，Makefile 会自动生成一个交付文件，名为 foo-handin.tar，其中包含 csim.c 和 trans.c 文件。学生将这个 tar 文件交给教师。

驱动程序（driver.py）评估 csim.c 中缓存模拟器的正确性，以及 trans.c 中转置函数的性能和正确性。详情请参阅实验报告。

要求：
- 实验必须在 64 位 x86-64 系统上完成。
- 驱动程序需要一个支持 "--tool=lackey" 选项的 Valgrind 版本（http://valgrind.org）。

********
2. 文件
********
Makefile              构建整个实验的文件
README                本文件
grade/                教师用的自动评分脚本
src/                  本实验的源代码
cachelab-handout/     分发给学生的手册目录。这个目录完全由 src 目录中的文件生成。
                      切勿在此目录中放置任何固定状态。
cachelab-handout.tar  分发给学生的手册目录的 Tar 文件。
writeup/              实验的 LaTeX 文档。根据您的环境进行修改。

*******************
3. 构建实验
*******************

要构建实验的默认版本，请根据您的环境修改 ./writeup/cachelab.tex 中的 LaTeX 实验报告。然后在当前目录下输入以下命令：

        unix> make clean
        unix> make

这将构建 cachelab-handout/ 目录及其可以分发给学生的 cachelab-handout.tar 存档。
命令：

	unix> make dist DEST=<DIR>

会将 tar 文件和实验报告的副本复制到学生可以访问的目录 <DIR> 中。

*********************
4. 上交实验
*********************

每当学生（例如登录名为 "foo"）编译他们的工作时，Makefile 会自动生成名为 foo-handin.tar 的交付文件。如果您想自动评分，将所有学生的交付 tar 文件收集到 ./grade/handin 目录中。

**********************
5. 自动评分实验
**********************

您可以手动在每个学生的交付物上运行驱动程序，或者您可以使用 ./grade 中的自动评分脚本自动评分 handin/ 目录中的每个 .tar 文件。

有关说明，请参见 ./grade/README。