# Lab Assignment L5: Writing Your Own Unix Shell

# Introduction
The purpose of this assignment is to become more familiar with the concepts of process control and signalling. 
You’ll do this by writing a simple Unix shell program that supports job control.

# Logistics
You may work in a group of up to two people in solving the problems for this assignment. The only “handin” will be electronic. Any clarifications and revisions to the assignment will be posted on the course Web page.

Start by copying the file shlab-handout.tar to the protected directory (the lab directory) in which you plan to do your work. Then do the following:
 - Type the command tar xvf shlab-handout.tar to expand the tarfile.
 - Type the command make to compile and link some test routines.
 - Type your team member names and Andrew IDs in the header comment at the top of `tsh.c`.
  
Looking at the tsh.c (tiny shell) file, you will see that it contains a functional skeleton of a simple Unix shell. 
To help you get started, we have already implemented the less interesting functions. 
Your assignment is to complete the remaining empty functions listed below. 
As a sanity check for you, we’ve listed the approximate number of lines of code for each of these functions in our reference solution (which includes lots of comments)

 - `eval`: Main routine that parses and interprets the command line. [70 lines]
  
 - `builtin cmd`: Recognizes and interprets the built-in commands: quit, fg, bg, and jobs. [25 lines]
  
 - `do_bgfg`: Implements the bg and fg built-in commands. [50 lines]
  
 - `waitfg`: Waits for a foreground job to complete. [20 lines]
  
 - `sigchld_handler`: Catches SIGCHILD signals. [80 lines]
  
 - `sigint_handler`: Catches SIGINT (*ctrl-c*) signals. [15 lines]
  
 - `sigtstp_handler`: Catches SIGTSTP (*ctrl-z*) signals. [15 lines]

Each time you modify your tsh.c file, type make to recompile it. To run your shell, type tsh to the command line:

~~~shell
unix> ./tsh
tsh> [type commands to your shell here]
~~~

# General Overview of Unix Shells

A shell is an interactive command-line interpreter that runs programs on behalf of the user. A shell repeatedly prints a prompt, waits for a command line on stdin, and then carries out some action, as directed by the contents of the command line.

The command line is a sequence of ASCII text words delimited by whitespace. The first word in the command line is either the name of a built-in command or the pathname of an executable file. The remaining words are command-line arguments. If the first word is a built-in command, the shell immediately executes the command in the current process. Otherwise, the word is assumed to be the pathname of an executable program. In this case, the shell forks a child process, then loads and runs the program in the context of the child. The child processes created as a result of interpreting a single command line are known collectively as a job. In general, a job can consist of multiple child processes connected by Unix pipes.

If the command line ends with an ampersand ”&”, then the job runs in the background, which means that the shell does not wait for the job to terminate before printing the prompt and awaiting the next command line. Otherwise, the job runs in the foreground, which means that the shell waits for the job to terminate before awaiting the next command line. Thus, at any point in time, at most one job can be running in the foreground. However, an arbitrary number of jobs can run in the background. 

For example, typing the command line

~~~shell
tsh> jobs
~~~

causes the shell to execute the built-in jobs command. Typing the command line

~~~shell
tsh> /bin/ls -l -d
~~~

runs the ls program in the foreground. 
By convention, the shell ensures that when the program begins executing its main routine

~~~c
int main(int argc, char *argv[])
~~~

the argc and argv arguments have the following values:
 • argc == 3,
 • argv[0] == ‘‘`/bin/ls`’’,
 • argv[1]== ‘‘`-l`’’,
 • argv[2]== ‘‘`-d`’’.
Alternatively, typing the command line

~~~shell
tsh> /bin/ls -l -d &
~~~

runs the ls program in the background.

Unix shells support the notion of job control, which allows users to move jobs back and forth between background and foreground, and to change the process state (running, stopped, or terminated) of the processes in a job. 

Typing ctrl-c causes a SIGINT signal to be delivered to each process in the foreground job. The default action for SIGINT is to terminate the process. 

Similarly, typing ctrl-z causes a SIGTSTP signal to be delivered to each process in the foreground job. The default action for SIGTSTP is to place a process in the stopped state, where it remains until it is awakened by the receipt of a SIGCONT signal. Unix shells also provide various built-in commands that support job control. 

For example:
 • `jobs`: List the running and stopped background jobs.
 • `bg <job>`: Change a stopped background job to a running background job.
 • `fg <job>`: Change a stopped or running background job to a running in the foreground.
 • `kill <job>`: Terminate a job.

# The tsh Specification

Your tsh shell should have the following features:

1. The prompt should be the string “`tsh>` ”.
2. The command line typed by the user should consist of a name and zero or more arguments, all separated by one or more spaces. If name is a built-in command, then tsh should handle it immediately and wait for the next command line. Otherwise, tsh should assume that name is the path of an executable file, which it loads and runs in the context of an initial child process (In this context, the term job refers to this initial child process)
3. tsh need not support pipes (|) or I/O redirection (< and >)
4. Typing ctrl-c (ctrl-z) should cause a SIGINT (SIGTSTP) signal to be sent to the current foreground job, as well as any descendents of that job (e.g., any child processes that it forked). If there is no foreground job, then the signal should have no effect
5. If the command line ends with an ampersand &, then tsh should run the job in the background. Otherwise, it should run the job in the foreground.
6. Each job can be identified by either a process ID (PID) or a job ID (JID), which is a positive integer assigned by tsh. JIDs should be denoted on the command line by the prefix ’%’. For example, “%5” denotes JID 5, and “5” denotes PID 5. (We have provided you with all of the routines you need for manipulating the job list.)
7. tsh should support the following built-in commands:
   1. The quit command terminates the shell.
   2. The jobs command lists all background jobs.
   3. The bg <job> command restarts <job> by sending it a SIGCONT signal, and then runs it in the background. The <job> argument can be either a PID or a JID.
   4. The fg <job> command restarts <job> by sending it a SIGCONT signal, and then runs it in the foreground. The <job> argument can be either a PID or a JID.
8. tsh should reap all of its zombie children. If any job terminates because it receives a signal that it didn’t catch, then tsh should recognize this event and print a message with the job’s PID and a description of the offending signal.

Translation:

1. 提示符应该是字符串 "`tsh>`"。
   
2. 用户键入的命令行应由一个名称和零个或多个参数组成，所有这些都由一个或多个空格分隔。如果名称是内置命令，那么tsh应该立即处理它并等待下一个命令行。否则，tsh应该假设名称是可执行文件的路径，它加载并在初始子进程的上下文中运行该文件（在这个上下文中，“作业”一词指的是这个初始子进程）。
   
3. tsh不需要支持管道（|）或I/O重定向（< 和 >）。
   
4. 输入ctrl-c（ctrl-z）应该导致SIGINT（SIGTSTP）信号发送到当前前台作业，以及该作业的任何后代（例如，它所fork的任何子进程）。如果没有前台作业，那么该信号应该没有效果。
   
5. 如果命令行以和号&结尾，则tsh应该在后台运行作业。否则，它应该在前台运行作业。
   
6. 每个作业都可以通过进程ID（PID）或作业ID（JID）来识别，后者是tsh分配的一个正整数。JIDs应该在命令行中用前缀'%'表示。例如，“%5”表示JID 5，“5”表示PID 5。（我们已经为您提供了操纵作业列表所需的所有程序。）
   
7. tsh应该支持以下内置命令：
   1. quit命令终止shell。
   2. jobs命令列出所有后台作业。
   3. bg <job>命令通过发送SIGCONT信号来重启<job>，然后在后台运行它。 <job>参数可以是PID或JID。
   4. fg <job>命令通过发送SIGCONT信号来重启<job>，然后在前台运行它。 <job>参数可以是PID或JID。

8. tsh应该清理掉所有的僵尸子进程。如果任何作业因为接收到它没有捕获的信号而终止，那么tsh应该识别这一事件并打印一条消息，包含作业的PID和有关信号的描述。

# Checking Your Work
We have provided some tools to help you check your work.

*Reference solution*. The Linux executable tshref is the reference solution for the shell. Run this program to resolve any questions you have about how your shell should behave. Your shell should emit output that is identical to the reference solution (except for PIDs, of course, which change from run to run).

*Shell driver*. The sdriver.plprogram executes a shell as a child process, sends it commands and signals as directed by a trace file, and captures and displays the output from the shell. Use the -h argument to find out the usage of sdriver.pl:

~~~shell
unix> ./sdriver.pl -h
Usage: sdriver.pl [-hv] -t <trace> -s <shellprog> -a <args>
Options:
-h Print this message
-v Be more verbose
-t <trace> Trace file
-s <shell> Shell program to test
-a <args> Shell arguments
-g Generate output for autograder
~~~

We have also provided 16 trace files (trace{01-16}.txt) that you will use in conjunction with the shell driver to test the correctness of your shell. The lower-numbered trace files do very simple tests, and the higher-numbered tests do more complicated tests

You can run the shell driver on your shell using trace file trace01.txt (for instance) by typing:

~~~shell
unix> ./sdriver.pl -t trace01.txt -s ./tsh -a "-p"
~~~

(the -a "-p" argument tells your shell not to emit a prompt), or 

~~~shell
unix> make test01
~~~

Similarly, to compare your result with the reference shell, you can run the trace driver on the reference shell by typing:

~~~shell
unix> ./sdriver.pl -t trace01.txt -s ./tshref -a "-p"
~~~

or

~~~shell
unix> make rtest01
~~~

For your reference, tshref.out gives the output of the reference solution on all races. This might be more convenient for you than manually running the shell driver on all trace files.

The neat thing about the trace files is that they generate the same output you would have gotten had you run your shell interactively (except for an initial comment that identifies the trace). 

For example:

~~~shell
bass> make test15
./sdriver.pl -t trace15.txt -s ./tsh -a "-p"
#
# trace15.txt - Putting it all together
#
tsh> ./bogus
./bogus: Command not found.
tsh> ./myspin 10
Job (9721) terminated by signal 2
tsh> ./myspin 3 &
[1] (9723) ./myspin 3 &
tsh> ./myspin 4 &
[2] (9725) ./myspin 4 &
tsh> jobs
[1] (9723) Running ./myspin 3 &
[2] (9725) Running ./myspin 4 &
tsh> fg %1
Job [1] (9723) stopped by signal 20
tsh> jobs
[1] (9723) Stopped ./myspin 3 &
[2] (9725) Running ./myspin 4 &
tsh> bg %3
%3: No such job
tsh> bg %1
[1] (9723) ./myspin 3 &
tsh> jobs
[1] (9723) Running ./myspin 3 &
[2] (9725) Running ./myspin 4 &
tsh> fg %1
tsh> quit
bass>
~~~


# Hints
 - Read every word of Chapter 8 (Exceptional Control Flow) in your textbook.
 - Use the trace files to guide the development of your shell. Starting with trace01.txt, make sure that your shell produces the identical output as the reference shell. Then move on to trace file trace02.txt, and so on.
 - The `waitpid`, `kill`, `fork`, `execve`, `setpgid`, and `sigprocmask` functions will come in very handy. The `WUNTRACED` and `WNOHANG` options to `waitpid` will also be useful.
 - When you implement your signal handlers, be sure to send SIGINT and SIGTSTP signals to the entire foreground process group, using ”-pid” instead of ”pid” in the argument to the kill function. The sdriver.pl program tests for this error.
 - One of the tricky parts of the assignment is deciding on the allocation of work between the waitfg and sigchld handler functions. We recommend the following approach:
   - In waitfg, use a busy loop around the sleep function.
   - In sigchld handler, use exactly one call to waitpid.
 While other solutions are possible, such as calling waitpidin both waitfgand sigchld handler, these can be very confusing. It is simpler to do all reaping in the handler.
 - In eval, the parent must use sigprocmask to block SIGCHLD signals before it forks the child, and then unblock these signals, again using sigprocmask after it adds the child to the job list by calling addjob. Since children inherit the blocked vectors of their parents, the child must be sure to then unblock SIGCHLD signals before it execs the new program. The parent needs to block the SIGCHLD signals in this way in order to avoid the race condition where the child is reaped by sigchld handler (and thus removed from the job list) before the parent calls addjob
 - Programs such as more, less, vi, and emacs do strange things with the terminal settings. Don’t run these programs from your shell. Stick with simple text-based programs such as /bin/ls, /bin/ps, and /bin/echo
 - When you run your shell from the standard Unix shell, your shell is running in the foreground process group. If your shell then creates a child process, by default that child will also be a member of the foreground process group. Since typing ctrl-c sends a SIGINT to every process in the foreground group, typing ctrl-c will send a SIGINT to your shell, as well as to every process that your shell created, which obviously isn’t correct.
 - Here is the workaround: After the fork, but before the execve, the child process should call setpgid(0, 0), which puts the child in a new process group whose group ID is identical to the child’s PID. This ensures that there will be only one process, your shell, in the foreground process group. When you type ctrl-c, the shell should catch the resulting SIGINT and then forward it to the appropriate foreground job (or more precisely, the process group that contains the foreground job).

Translation:

 - 仔细阅读教科书第8章（异常控制流）的每一个字。
  
 - 使用追踪文件来指导你的shell开发。从trace01.txt开始，确保你的shell产生的输出与参考shell完全相同。然后继续处理追踪文件trace02.txt，依此类推。
  
 - `waitpid`、`kill`、`fork`、`execve`、`setpgid`和`sigprocmask`函数将非常有用。`waitpid`的`WUNTRACED`和`WNOHANG`选项也将是有益的。
  
 - 当你实现信号处理程序时，确保向整个前台进程组发送SIGINT和SIGTSTP信号，使用"-pid"而不是"pid"作为kill函数的参数。sdriver.pl程序会测试这个错误。
  
 - 这项任务的难点之一是决定waitfg和sigchld处理程序功能之间的工作分配。我们推荐以下方法：
   - 在waitfg中，使用围绕sleep函数的忙循环。
   - 在sigchld处理程序中，确保只调用一次waitpid。
 虽然其他解决方案也是可能的，比如在waitfg和sigchld处理程序中都调用waitpid，但这些可能会非常混乱。在处理程序中进行所有回收操作会更简单。
 
 - 在eval中，父进程必须在fork子进程之前使用sigprocmask来阻塞SIGCHLD信号，然后在通过调用addjob将子进程添加到作业列表后，再次使用sigprocmask解除这些信号的阻塞。由于子进程继承了它们父进程的阻塞向量，子进程必须确保在exec新程序之前解除对SIGCHLD信号的阻塞。父进程需要以这种方式阻塞SIGCHLD信号，以避免发生竞争条件，即在父进程调用addjob之前，子进程已经被sigchld处理程序回收（并因此从作业列表中移除）。 
  
 - 像more、less、vi和emacs这样的程序会对终端设置做一些奇怪的事情。不要从你的shell中运行这些程序。坚持使用简单的基于文本的程序，如/bin/ls、/bin/ps和/bin/echo。
  
 - 当你从标准Unix shell运行你的shell时，你的shell运行在前台进程组中。如果你的shell创建了一个子进程，那么默认情况下，该子进程也将是前台进程组的成员。由于输入ctrl-c会向前台组的每个进程发送SIGINT，因此输入ctrl-c将向你的shell以及你的shell创建的每个进程发送SIGINT，这显然是不正确的。这里是解决办法：在fork之后，但在execve之前，子进程应该调用setpgid(0, 0)，这将把子进程放在一个新的进程组中，其组ID与子进程的PID相同。这确保只有一个进程，即你的shell，在前台进程组中。当你输入ctrl-c时，shell应该捕获结果SIGINT，然后将其转发给适当的前台作业（或更准确地说，包含前台作业的进程组）。

# Evaluation
Your score will be computed out of a maximum of 90 points based on the following distribution:

 - 80 Correctness: 16 trace files at 5 points each.
 - 10 Style points. We expect you to have good comments (5 pts) and to check the return value of EVERY system call (5 pts).
  
Your solution shell will be tested for correctness on a Linux machine, using the same shell driver and trace files that were included in your lab directory. Your shell should produce identical output on these traces as the reference shell, with only two exceptions:
 • The PIDs can (and will) be different.
 • The output of the /bin/ps commands in trace11.txt, trace12.txt, and trace13.txt will be different from run to run. However, the running states of any mysplit processes in the output of the /bin/ps command should be identical.

