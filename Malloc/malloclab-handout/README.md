#####################################################################
# CS:APP Malloc Lab
# Handout files for students
#
# Copyright (c) 2002, R. Bryant and D. O'Hallaron, All rights reserved.
# May not be used, modified, or copied without permission.
#
######################################################################

***********
Main Files:
***********

mm.{c,h}	
	Your solution malloc package. mm.c is the file that you
	will be handing in, and is the only file you should modify.

mdriver.c	
	The malloc driver that tests your mm.c file

short{1,2}-bal.rep
	Two tiny tracefiles to help you get started. 

Makefile	
	Builds the driver

**********************************
Other support files for the driver
**********************************

config.h	Configures the malloc lab driver
fsecs.{c,h}	Wrapper function for the different timer packages
clock.{c,h}	Routines for accessing the Pentium and Alpha cycle counters
fcyc.{c,h}	Timer functions based on cycle counters
ftimer.{c,h}	Timer functions based on interval timers and gettimeofday()
memlib.{c,h}	Models the heap and sbrk function

*******************************
Building and running the driver
*******************************
To build the driver, type "make" to the shell.

To run the driver on a tiny test trace:

	unix> mdriver -V -f short1-bal.rep

The -V option prints out helpful tracing and summary information.

To get a list of the driver flags:

	unix> mdriver -h

====================================================================================================

#####################################################################
# CS:APP Malloc 实验
# 学生手册文件
#
# 版权所有 2002, R. Bryant 和 D. O'Hallaron, 保留所有权利。
# 未经允许，不得使用、修改或复制。
#
######################################################################

***********
主要文件:
***********

`mm.{c,h}`
	你的解决方案 malloc 包。mm.c 是你将要提交的文件，
	也是你唯一需要修改的文件。

`mdriver.c`
	测试你的 mm.c 文件的 malloc 驱动程序

`short{1,2}-bal.rep`
	两个小型追踪文件，帮助你开始。

`Makefile`
	构建驱动程序

**********************************
驱动程序的其他支持文件
**********************************

`config.h`	配置 malloc 实验驱动程序
`fsecs.{c,h}`	不同计时包的封装函数
`clock.{c,h}`	访问 Pentium 和 Alpha 周期计数器的例程
`fcyc.{c,h}`	基于周期计数器的计时函数
`ftimer.{c,h}`	基于间隔计时器和 gettimeofday() 的计时函数
`memlib.{c,h}`	模拟堆和 sbrk 函数

*******************************
构建和运行驱动程序
*******************************
要构建驱动程序，请在 shell 中输入“make”。

~~~shell
# 要在一个小型测试追踪上运行驱动程序：

	unix> mdriver -V -f short1-bal.rep

# -V 选项打印出有用的追踪和总结信息。

# 要获取驱动程序标志列表：

	unix> mdriver -h
~~~