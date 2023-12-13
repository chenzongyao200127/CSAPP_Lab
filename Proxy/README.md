#####################################################################
# CS:APP Proxy Lab
# Directions to Instructors
#
# Copyright (c) 2004-2016, R. Bryant and D. O'Hallaron, All rights reserved.
######################################################################

************
1. Overview
************

In this lab, students build a simple concurrent caching Web proxy that
accepts HTTP 1.0 requests from clients, forwards them to end servers,
and then sends the replies back to the clients.  The proxy caches
objects returned by the end servers, and attempts to satisfy requests
from clients from the cache before forwarding the requests to the end
servers.

********
2. Files
********

Makefile           Builds the entire lab
README             This file
proxylab-handout/  Handout directory that goes to the students. Regenerated from src/
src/               Source code for the solution and the driver
writeup/           Latex lab writeup
grade/             Grading directory where instructors/TAs run the autograder

*******************
3. Building the Lab
*******************

Step 1: Modify the Latex writeup in writeup to reflect the handout and
handin instructions for your site.

Step 2: Type "make clean; make" to format the writeup in writeup/proxylab.pdf
and to build the proxylab-handout.tar file.

Step 3: Distribute proxylab.pdf and proxylab-handout.tar to the students. 

******************
4. Grading the Lab
******************

Autograding scripts are available in the grade/ directory. See grade/README for details.

When we offer this lab at CMU, we do some additional manual grading:
- Manually test on four real sites (see writeup/proxylab.tex for details)
- Manually grade for Style
- Manually check for errors that the autograder doesn't check for,
  such as race conditions on shared data structures.

************
5. Solutions
************
The reference solution is in ./src/proxy.[ch] and ./src/cache.[ch]

Sure, here's the translation of the provided text:

====================================================================================================
#####################################################################
# CS:APP 代理实验室
# 教师指导说明
#
# 版权所有 2004-2016, R. Bryant 和 D. O'Hallaron, 保留所有权利。
######################################################################

************
1. 概述
************

在这个实验室中，学生们将构建一个简单的并发缓存Web代理。该代理接受来自客户端的HTTP 1.0请求，将它们转发给终端服务器，然后将回复发送回客户端。
代理缓存来自终端服务器的对象，并尝试在将请求转发给终端服务器之前从缓存中满足客户端的请求。

********
2. 文件
********

Makefile           构建整个实验室
README             本文件
proxylab-handout/  分发给学生的讲义目录，从src/重新生成
src/               解决方案和驱动程序的源代码
writeup/           Latex实验报告
grade/             教师/助教在该目录下运行自动评分程序的评分目录

*******************
3. 构建实验室
*******************

步骤1：修改writeup中的Latex报告，以反映您的站点的分发和提交指南。

步骤2：键入"make clean; make"以格式化writeup/proxylab.pdf中的报告，并构建proxylab-handout.tar文件。

步骤3：将proxylab.pdf和proxylab-handout.tar分发给学生。

******************
4. 评分实验室
******************

自动评分脚本可在grade/目录中找到。详情见grade/README。

当我们在CMU提供这个实验室时，我们会进行一些额外的手动评分：
- 在四个真实网站上手动测试（详情见writeup/proxylab.tex）
- 手动评分风格
- 手动检查自动评分程序未检查的错误，例如共享数据结构上的竞态条件。

************
5. 解决方案
************
参考解决方案在./src/proxy.[ch] 和 ./src/cache.[ch]


====================================================================================================

实验简介
学生实现一个并发缓存（concurrent caching）的 Web 代理，该代理位于学生的浏览器和其他万维网之间。
该实验让学生接触到有趣的网络编程世界，并将课程中的许多概念联系在一起，例如字节序（byte ordering）、缓存、进程控制、信号、信号处理、文件 I/O、并发和同步。