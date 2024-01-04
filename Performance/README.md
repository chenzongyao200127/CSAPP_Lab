#####################################################################
# CS:APP Performance Lab
# Directions to Instructors
#
# Copyright (c) 2002, R. Bryant and D. O'Hallaron, All rights reserved.
# May not be used, modified, or copied without permission.
#
######################################################################

This directory contains the files that you will need to run the CS:APP
Performance Lab, which develops the students's understanding of the
performance impact of caches and code optimizations such as blocking
and loop unrolling.

***********
1. Overview
***********

In this lab, students work on a C file, called kernels.c, that
contains some image processing kernels. The students are asked to use
the concepts they have learned in class to make these kernels run as
fast as possible.

Students evaluate the performance of their kernels by linking
kernels.c into a driver program called driver.c. Students can register
different versions of their kernel functions with the driver. When the
driver runs, it will execute, measure, and report on the performance
of each registered function. This is a very useful versioning feature
that allows students try out different approaches and do side by side
comparisons.

You should strongly encourage your students to take advantage of this
versioning feature in the driver.

********
2. Files
********

Makefile		Builds the entire lab
README			This file
src/			Driver code that runs and measures solutions
perflab-handout/	Handout directory that goes to the students
grade/			Autograder code
writeup/		Sample Latex lab writeup

*******************
3. Building the Lab
*******************

To build the lab, modify the Latex lab writeup in
./writeup/perflab.tex for your environment. Then type the following in
the current directory:

	unix> make clean
	unix> make 

The Makefile generates the driver code, formats the lab writeup, and
then copies the driver code to the perflab-handout directory.
Finally, it builds a tarfile of the perflab-handout directory (in
perflab-handout.tar) which you can distribute to students.
The command:

	unix> make dist DEST=<DIR>

will copy the tarfile and copies of the writeup to directory <DIR>,
where the students can access it.

**********************
4. Autograding the Lab
**********************

There is an autograding script that automatically grades the lab.  See
./grade/README for instructions.



这个目录包含运行CS:APP性能实验室所需的文件，该实验室旨在提高学生对缓存和代码优化（如分块和循环展开）的性能影响的理解。

***********
1. 概述
***********

在这个实验室中，学生将处理一个名为kernels.c的C文件，其中包含一些图像处理核心功能。要求学生运用他们在课堂上学到的概念，使这些核心功能运行得尽可能快。

学生通过将kernels.c链接到一个名为driver.c的驱动程序中来评估他们的核心功能的性能。学生可以向驱动程序注册不同版本的核心函数。当驱动程序运行时，它会执行、测量并报告每个注册函数的性能。这是一个非常有用的版本功能，允许学生尝试不同的方法并进行并行比较。

你应该强烈鼓励学生利用驱动程序中的这个版本功能。

********
2. 文件
********

Makefile		构建整个实验室
README			此文件
src/			运行和测量解决方案的驱动代码
perflab-handout/	分发给学生的讲义目录
grade/			自动评分代码
writeup/		样本Latex实验室写作

*******************
3. 构建实验室
*******************

要构建实验室，请在./writeup/perflab.tex中修改Latex实验室写作以适应您的环境。然后在当前目录中输入以下命令：

	unix> make clean
	unix> make 

Makefile生成驱动代码，格式化实验室写作，并将驱动代码复制到perflab-handout目录中。
最后，它会构建一个包含perflab-handout目录的tar文件（在perflab-handout.tar中），您可以将其分发给学生。
命令：

	unix> make dist DEST=<DIR>

将会把tar文件和写作的副本复制到目录<DIR>，学生可以在那里访问它们。

**********************
4. 自动评分实验室
**********************

有一个自动评分脚本可以自动评分实验室。参见./grade/README以获取指导。