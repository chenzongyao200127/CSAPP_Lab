#####################################################################
# CS:APP Malloc Lab
# Directions to Instructors
#
# Copyright (c) 2002, R. Bryant and D. O'Hallaron, All rights reserved.
# May not be used, modified, or copied without permission.
#
######################################################################

This directory contains the files that you will need to run the CS:APP
malloc lab, which develops a student's understanding of pointers,
system-level programming, and memory managment.

************
1. Overview
************

In this lab, students write their own storage allocator, in particular
implementations of the malloc, free, and realloc functions. A
trace-driven driver (mdriver) evaluates the quality of the student's
implementation in terms of space utilization and throughput.

********
2. Files
********

Makefile		Makefile that builds the Lab
README			This file
grade/			Autograding scripts
malloclab-handout/	The files handed out to the students
src/			The driver sources
traces/			The malloc/free/realloc trace files used by the driver
writeup/		The malloc lab writeup	

*******************
3. Building the Lab
*******************

Step 1: Configure the driver in src/ for your site. See src/README for
detailed information on the driver.

Step 2: Modify the Latex writeup in writeup/ to reflect the handout
and handin directions for your site. If you don't use Latex, use your
favorite document preparation system to prepare Postcript and PDF
versions of the writeup in malloclab.ps and malloclab.pdf.

Step 3: Modify the LABNAME and DEST variables in ./Makefile for your
site.

Step 4: Type "make" to build the $(LABNAME)-handout.tar file.

Step 5: Type "make dist" to copy the $(LABNAME)-handout.tar file
and the writeup to the distribution directory where the students
will retrieve the lab. 

******************
4. Grading the Lab
******************

There are autograding scripts for this lab. See grade/README for
details.

=====================================[Chinese Version]================================================

************
1. 概览
************

在这个实验室中，学生将编写自己的存储分配器，特别是malloc、free和realloc函数的实现。
一个基于跟踪的驱动程序（mdriver）评估学生实现的质量，从空间利用率和吞吐量方面。

********
2. 文件
********

Makefile		构建实验室的Makefile
README			本文件
grade/			自动评分脚本
malloclab-handout/	分发给学生的文件
src/			驱动程序源码
traces/			驱动程序使用的malloc/free/realloc跟踪文件
writeup/		malloc实验室报告

*******************
3. 构建实验室
*******************

步骤1：在src/中配置驱动程序以适应您的站点。详细信息请参阅src/README。

步骤2：修改writeup/中的Latex报告，以反映您站点的分发和提交指南。
如果您不使用Latex，使用您喜欢的文档制备系统，准备malloclab.ps和malloclab.pdf中的Postcript和PDF版本的报告。

步骤3：修改./Makefile中的LABNAME和DEST变量以适应您的站点。

步骤4：输入“make”以构建$(LABNAME)-handout.tar文件。

步骤5：输入“make dist”以将$(LABNAME)-handout.tar文件和报告复制到学生将检索实验室的分发目录。

******************
4. 评分实验室
******************

这个实验室有自动评分脚本。详细信息请参阅grade/README。