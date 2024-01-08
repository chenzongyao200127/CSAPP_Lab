# TODO
 - `eval`: Main routine that parses and interprets the command line. [70 lines]
 - `builtin cmd`: Recognizes and interprets the built-in commands: quit, fg, bg, and jobs. [25 lines]
 - `do_bgfg`: Implements the bg and fg built-in commands. [50 lines]
 - `waitfg`: Waits for a foreground job to complete. [20 lines]
 - `sigchld_handler`: Catches SIGCHILD signals. [80 lines]
 - `sigint_handler`: Catches SIGINT (*ctrl-c*) signals. [15 lines]
 - `sigtstp_handler`: Catches SIGTSTP (*ctrl-z*) signals. [15 lines]
  
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


- “仔细阅读教科书第8章（异常控制流）。”

- “使用跟踪文件指导shell的开发。从trace01.txt开始，确保你的shell产生与参考shell相同的输出。然后转到trace02.txt，以此类推。”

- “`waitpid`、`kill`、`fork`、`execve`、`setpgid`和`sigprocmask`函数将非常有用。`waitpid`的`WUNTRACED`和`WNOHANG`选项也很有用。”

- “在实现信号处理程序时，一定要向整个前台进程组发送SIGINT和SIGTSTP信号，使用'-pid'而不是'pid'作为kill函数的参数。sdriver.pl程序会检测这个错误。”

- “作业的一个棘手部分是决定waitfg和sigchld处理程序之间的工作分配。我们建议以下方法：
  - 在waitfg中，使用sleep函数的忙循环。
  - 在sigchld处理程序中，只使用一次waitpid调用。
  虽然其他解决方案也是可能的，例如在waitfg和sigchld处理程序中都调用waitpid，但这可能会非常混乱。在处理程序中进行所有的回收操作更为简单。”

- “在eval中，父进程必须在fork子进程之前使用sigprocmask阻塞SIGCHLD信号，然后在通过调用addjob将子进程添加到作业列表后再次使用sigprocmask解除这些信号的阻塞。由于子进程会继承其父进程的阻塞向量，因此子进程必须确保在执行新程序之前解除SIGCHLD信号的阻塞。父进程需要这样阻塞SIGCHLD信号，以避免出现竞争条件，即在父进程调用addjob之前，子进程已被sigchld处理程序回收（并因此从作业列表中移除）。”

- “像more、less、vi和emacs这样的程序会对终端设置做一些奇怪的事情。不要从你的shell中运行这些程序。坚持使用简单的基于文本的程序，如/bin/ls、/bin/ps和/bin/echo。”

- “当你从标准Unix shell运行你的shell时，你的shell运行在前台进程组中。如果你的shell然后创建了一个子进程，那么默认情况下，该子进程也将是前台进程组的成员。由于输入ctrl-c会向前台组中的每个进程发送SIGINT，因此输入ctrl-c会将SIGINT发送给你的shell，以及你的shell创建的每个进程，这显然是不正确的。”

- “这里是一个解决办法：在fork之后，但在execve之前，子进程应该调用setpgid(0, 0)，将子进程放在一个新的进程组中，该组的组ID与子进程的PID相同。这确保只有一个进程，即你的shell，在前台进程组中。当你输入ctrl-c时，shell应该捕获结果的SIGINT，然后将其转发给适当的前台作业（或更准确地说，包含前台作业的进程组）。”

这些建议适用于开发一个shell程序，特别强调了进程管理、信号处理和进程间通信的重要性。

1. **“仔细阅读教科书第8章（异常控制流）。”**
   - 这个提示建议彻底研究第8章，该章节涉及异常控制流，是理解如何处理信号和进程间通信的关键。这对于编写正确处理信号和异常情况的shell程序至关重要。

2. **“使用跟踪文件指导shell的开发。”**
   - 这建议使用预先定义的跟踪文件（如trace01.txt, trace02.txt等）来测试和指导shell的开发。这些文件包含预期的输出，可以用来验证你的shell是否正确实现了所需功能。

3. **`waitpid`、`kill`、`fork`、`execve`、`setpgid`和`sigprocmask`函数将非常有用。**
   - 这些建议强调了几个关键的UNIX系统调用，它们对于进程创建、信号发送、进程组管理和信号屏蔽至关重要。了解它们的用法对于编写功能齐全的shell程序是必须的。

4. **“在实现信号处理程序时，确保向整个前台进程组发送SIGINT和SIGTSTP信号。”**
   - 这是关于信号处理的具体建议。当shell的前台进程需要被中断或停止时，应该使用`kill`函数向整个进程组发送SIGINT或SIGTSTP信号。这意味着你需要使用负的进程ID作为`kill`函数的参数。

5. **关于`waitfg`和`sigchld`处理程序的工作分配。**
   - 这部分建议涉及进程同步和状态管理。在`waitfg`中使用忙循环来等待前台进程的结束，而在`sigchld`处理程序中使用`waitpid`来处理子进程的退出。这样做可以简化代码逻辑，并减少错误。

6. **“在eval中，父进程必须在fork子进程之前和之后使用sigprocmask。”**
   - 这是关于如何防止竞争条件的建议。在创建子进程（通过`fork`）前后正确地阻塞和解除阻塞SIGCHLD信号，确保父进程有机会将子进程加入作业列表，避免子进程结束过快而导致的竞争条件。

7. **“不要从你的shell中运行如more、less、vi和emacs等程序。”**
   - 这个建议是因为某些程序（如文本编辑器或分页器）会更改终端的设置，可能导致与shell的交互出现问题。建议在开发shell时使用简单的、基于文本的程序，如ls、ps或echo。

8. **“当你从标准Unix shell运行你的shell时，处理ctrl-c的问题。”**
   - 这是关于进程组和信号处理的建议。确保你的shell进程是前台进程组的唯一成员，这样当你按ctrl-c时，只有shell（而不是它创建的任何子进程）接收到SIGINT信号。这可以通过在子进程中调用`setpgid(0, 0)`来实现，从而将子进程放入一个新的进程组。

通过理解并实现这些细节，你将能够构建一个健壮、功能丰富的shell程序，能够有效地管理进程和信号，处理用户输入和外部程序的执行。

- 用户输入的命令行由一个 name、零个或者多个参数组成，它们都由一个或者多个空格分隔开。如果 name 是一个内置命令，那么 shell 就立即处理它，并等待下一个命令行。否则，shell 就假设 name 是一个可执行文件，在一个初始的子进程（作业）的上下文中加载并运行它。作业的进程组 ID 与子进程的 PID 相同。
- 
- 每个作业是由一个进程 ID（PID）或者一个作业 ID（JID）来标识的，它是由一个 shell 分配的任意的小正整数。JID 在命令行上用前缀 “%” 来表示。比如，“％5” 表示 JID 5，而 “5” 表示 PID 5。
- 
- 如果命令行以 & 来结束，那么 shell 就在后台运行这个作业。否则，shell 就在前台运行这个作业。
- 
- 输入 Ctrl+C（Ctrl+Z），使得内核发送一个 SIGINT（SIGTSTP）信号给 shell，shell 再转发给前台进程组中的每个进程。✦
- 
- 内置命令 jobs 列出所有的后台作业。
- 
- 内置命令 bg job 通过发送一个 SIGCONT 信号重启 job，然后在后台运行它。job 参数可以是一个 PID，也可以是一个 JID。
- 
- 内置命令 fg job 通过发送一个 SIGCONT 信号重启 job，然后在前台运行它。
- 
- shell 回收它所有的僵死子进程。如果任何作业因为收到一个未捕获的信号而终止，那么 shell 就输出一条消息到终端，消息中包含该作业的 PID 和对该信号的描述。

