# 编写信号处理程序

信号处理是 Linux 系统编程最棘手的一个问题。处理程序有几个属性使得它们很难推理分析：
1. 处理程序与主程序并发运行，共享同样的全局变量，因此可能与主程序和其他处理程序互相干扰；
2. 如何以及何时接收信号的规则常常有违人的直觉；
3. 不同的系统有不同的信号处理语义。

在本节中，我们将讲述这些问题，介绍编写安全、正确和可移植的信号处理程序的一些基本规则。

## 安全的信号处理

信号处理程序很麻烦是因为它们和主程序以及其他信号处理程序并发地运行，正如我们在图 8-31 中看到的那样。
如果处理程序和主程序并发地访问同样的全局数据结构，那么结果可能就不可预知，而且经常是致命的。

~~~c

// 编写一个叫做 snooze 的程序，它有一个命令行参数，用这个参数调用练习题 8.5 中的 snooze 函数，然后终止。编写程序，使得用户可以通过在键盘上输入 Ctrl+C 中断 snooze 函数。比如：
//
// linux> ./snooze 5
// CTRL+C                     // User hits Crtl+C after 3 seconds
// Slept for 3 of 5 secs.
// linux>


// 只要休眠进程收到一个未被忽略的信号，sleep 函数就会提前返回。
// 但是，因为收到一个 SIGINT 信号的默认行为就是终止进程（图 8-26），我们必须设置一个 SIGINT 处理程序来允许 sleep 函数返回。
// 处理程序简单地捕获 SIGNAL，并将控制返回给 sleep 函数，该函数会立即返回。
#include "csapp.h"

/* SIGINT handler */
void handler(int sig)
{
    return; /* Catch the signal and return */
}

unsigned int snooze(unsigned int secs) {
    unsigned int rc = sleep(secs);

    printf("Slept for %d of %d secs.\n", secs - rc, secs);

    return rc;
}

int main(int argc, char **argv) {
    
    if (argc != 2) {
        fprintf(stderr, "usage: %s <secs>\n", argv[0]);
        exit(0);
    }

    if (signal(SIGINT, handler) == SIG_ERR) /* Install SIGINT */
        unix_error("signal error\n"); /* handler */
    (void)snooze(atoi(argv[1]));
    
    exit(0);
}
~~~

我们会在第 12 章详细讲述并发编程。这里我们的目标是给你一些保守的编写处理程序的原则，使得这些处理程序能安全地并发运行。
如果你忽视这些原则，就可能有引入细微的并发错误的风险。如果有这些错误，程序可能在绝大部分时候都能正确工作。
然而当它出错的时候，就会错得不可预测和不可重复，这样是很难调试的。一定要防患于未然！

 -  **G0. 处理程序要尽可能简单** 避免麻烦的最好方法是保持处理程序尽可能小和简单。例如，处理程序可能只是简单地设置全局标志并立即返回；所有与接收信号相关的处理都由主程序执行，它周期性地检查（并重置）这个标志。
  
 -  **G1. 在处理程序中只调用异步信号安全的函数**。所谓异步信号安全的函数（或简称安全的函数）能够被信号处理程序安全地调用，原因有二：要么它是可重入的（例如只访问局部变量，见 12.7.2 节），要么它不能被信号处理程序中断。图 8-33 列出了 Linux 保证安全的系统级函数。注意，许多常见的函数（例如 printf、sprintf、malloc 和 exit）都不在此列。

    > 所谓“可重入”的函数指的是一种特别设计的函数，它能够在任何时刻被中断，并且之后能够安全地恢复执行，而不会出现数据损坏、状态混乱或不确定的行为。这在信号处理程序中尤为重要，因为信号处理程序可能在程序执行的任何时刻被调用。
    > 可重入函数的主要特点包括：

    1. **不依赖全局数据或静态数据**：全局或静态数据可能被多次调用的函数共享，如果函数在执行中被中断并再次调用，这些数据可能会处于不一致的状态。
    2. **仅使用局部变量**：局部变量存储在栈上，每次函数调用都会有自己的一套局部变量，这就避免了共享数据带来的问题。
    3. **没有副作用**：如修改全局变量、调用非可重入函数等。
    4. **不执行非原子操作**：如分配内存、I/O 操作等，这些操作可能会被中断，并在再次进入函数时处于不确定状态。

    > 在多线程和并发编程中，可重入性是一个重要的考量因素，因为它确保了函数在异步执行环境中的稳定性和可靠性。信号处理程序尤其需要考虑这一点，因为信号可以在程序的任何执行点发生。

 - 信号处理程序中产生输出唯一安全的方法是使用 `write` 函数（见 10.1 节）。特别地，调用 printf 或 sprintf 是不安全的。为了绕开这个不幸的限制，我们开发一些安全的函数，称为 SIO（安全的 I/O）包，可以用来在信号处理程序中打印简单的消息。

~~~c
#include "csapp.h"

ssize_t sio_putl(long v);
ssize_t sio_puts(char s[]);
// 返回：如果成功则为传送的字节数，如果出错，则为 -1。

void sio_error(char s[]);
// 返回：空。
~~~

sio_putl 和 sio_puts 函数分别向标准输出传送一个 long 类型数和一个字符串。sio_error 函数打印一条错误消息并终止。

图 8-34 给出的是 SIO 包的实现，它使用了 csapp.c 中两个私有的可重入函数。第 3 行的 sio_strlen 函数返回字符串 s 的长度。第 10 行的 sio_ltoa 函数基于来自【61】的 itoa 函数，把 v 转换成它的基 b 字符串表示，保存在 s 中。第 17 行的 _exit 函数是 exit 的一个异步信号安全的变种。

~~~c
ssize_t sio_puts(char s[]) /* Put string */
{
    return write(STDOUT_FILENO, s, sio_strlen(s));
}

ssize_t sio_putl(long v) /* Put long */
{
    char s[128];

    sio_ltoa(v, s, 10); /* Based on K&R itoa() */
    return sio_puts(s);
}

void sio_error(char s[]) /* Put error message and exit */
{
    sio_puts(s);
    _exit(1);
}
~~~

图 8-35 给出了图 8-30 中 SIGINT 处理程序的一个安全的版本。

~~~c
#include "csapp.h"

void sigint_handler(int sig) /* Safe SIGINT handler */
{
    Sio_puts("Caught SIGINT!\n"); /* Safe output */
    _exit(0);                     /* Safe exit */
}
~~~


 - **G2. 保存和恢复 errno。**许多 Linux 异步信号安全的函数都会在出错返回时设置 errno。在处理程序中调用这样的函数可能会干扰主程序中其他依赖于 errno 的部分。解决方法是在进入处理程序时把 errno 保存在一个局部变量中，在处理程序返回前恢复它。注意，只有在处理程序要返回时才有此必要。如果处理程序调用 _exit 终止该进程，那么就不需要这样做了。
  
 - **G3. 阻塞所有的信号，保护对共享全局数据结构的访问。**如果处理程序和主程序或其他处理程序共享一个全局数据结构，那么在访问（读或者写）该数据结构时，你的处理程序和主程序应该暂时阻塞所有的信号。这条规则的原因是从主程序访问一个数据结构 d 通常需要一系列的指令，如果指令序列被访问次的处理程序中断，那么处理程序可能会发现 d 的状态不一致，得到不可预知的结果。在访问 d 时暂时阻塞信号保证了处理程序不会中断该指令序列。
  
 - **G4. 用 volatile 声明全局变量。**考虑一个处理程序和一个 main 函数，它们共享一个全局变量 g。处理程序更新 g，main 周期性地读 g。对于一个优化编译器而言，main 中 g 的值看上去从来没有变化过，因此使用缓存在寄存器中 g 的副本来满足对 g 的每次引用是很安全的。如果这样，main 函数可能永远都无法看到处理程序更新过的值。
 可以用 volatile 类型限定符来定义一个变量，告诉编译器不要缓存这个变量。例如：
 ~~~c
    volatile int g;
 ~~~
 volatile 限定符强迫编译器每次在代码中引用 g 时，都要从内存中读取 g 的值。一般来说，和其他所有共享数据结构一样，应该暂时阻塞信号，保护每次对全局变量的访问。

 - **G5. 用 sig_atomic_t 声明标志。**在常见的处理程序设计中，处理程序会写全局标志来记录收到了信号。主程序周期性地读这个标志，响应信号，再清除该标志。对于通过这种方式来共享的标志，C 提供一种整型数据类型 `sig_atomic_t`，对它的读和写保证会是原子的（不可中断的），因为可以用一条指令来实现它们：
 `volatile sig_atomic_t flag;`
 因为它们是不可中断的，所以可以安全地读和写 sig_atomic_t 变量，而不需要暂时阻塞信号。
 注意，这里对原子性的保证只适用于单个的读和写，不适用于像 flag++ 或 flag=flag+10 这样的更新，它们可能需要多条指令。

要记住我们这里讲述的规则是保守的，也就是说它们不总是严格必需的。例如，如果你知道处理程序绝对不会修改 errno，那么就不需要保存和恢复 errno。或者如果你可以证明 printf 的实例都不会被处理程序中断，那么在处理程序中调用 printf 就是安全的。对共享全局数据结构的访问也是同样。
不过，一般来说这种断言很难证明。所以我们建议你采用保守的方法，遵循这些规则，使得处理程序尽可能简单，调用安全函数，保存和恢复 errno，保护对共享数据结构的访问，并使用 volatile 和 sig_atomic_t。

## 正确的信号处理

信号的一个与直觉不符的方面是未处理的信号是不排队的。因为 pending 位向量中每种类型的信号只对应有一位，所以每种类型最多只能有一个未处理的信号。因此，如果两个类型 k 的信号发送给一个目的进程，而因为目的进程当前正在执行信号 k 的处理程序，所以信号 k 被阻塞了，那么第二个信号就简单地被丢弃了；它不会排队。关键思想是如果存在一个未处理的信号就表明至少有一个信号到达了。

要了解这样会如何影响正确性，来看一个简单的应用，它本质上类似于像 shell 和 Web 服务器这样的真实程序。基本的结构是父进程创建一些子进程，这些子进程各自独立运行一段时间，然后终止。父进程必须回收子进程以避免在系统中留下僵死进程。但是我们还希望父进程能够在子进程运行时自由地去做其他的工作。所以，我们决定用 SIGCHLD 处理程序来回收子进程，而不是显式地等待子进程终止。（回想一下，只要有一个子进程终止或者停止，内核就会发送一个 `SIGCHLD` 信号给父进程。）

图 8-36 展示了我们的初次尝试。父进程设置了一个 SIGCHLD 处理程序，然后创建了 3 个子进程。同时，父进程等待来自终端的一个输入行，随后处理它。这个处理被模型化为一个无限循环。当每个子进程终止时，内核通过发送一个 SIGCHLD 信号通知父进程。父进程捕获这个 SIGCHLD 信号，回收一个子进程，做一些其他的清理工作（模型化为 sleep 语句），然后返回。

~~~c
/* WARNING: This code is buggy! */

/* Function to handle SIGCHLD signals */
void handler1(int sig)
{
    int olderrno = errno; // Save current errno to restore it later

    // Wait for any child process to end. If waitpid returns an error, handle it.
    if ((waitpid(-1, NULL, 0)) < 0)
        sio_error("waitpid error"); // Print error message if waitpid fails

    Sio_puts("Handler reaped child\n"); // Print a message indicating a child process was handled
    Sleep(1); // Sleep for 1 second
    errno = olderrno; // Restore the original errno
}

int main()
{
    int i, n;
    char buf[MAXBUF]; // Buffer for reading input

    // Set up the signal handler for SIGCHLD. If it fails, handle the error.
    if (signal(SIGCHLD, handler1) == SIG_ERR)
        unix_error("signal error"); // Print error message if signal setup fails

    /* Parent creates children */
    for (i = 0; i < 3; i++) {
        // Fork a child process
        if (Fork() == 0) {
            // This block is executed by the child process
            printf("Hello from child %d\n", (int)getpid()); // Child prints its PID
            exit(0); // Child exits
        }
    }

    /* Parent waits for terminal input and then processes it */
    // Read input from STDIN. If read returns an error, handle it.
    if ((n = read(STDIN_FILENO, buf, sizeof(buf))) < 0)
        unix_error("read"); // Print error message if read fails

    printf("Parent processing input\n"); // Indicate that parent is processing input

    // Infinite loop. Without a break condition, this will loop forever.
    while (1)
        ;

    exit(0); // Exit program (unreachable code due to the infinite loop above)
}
// signal1：这个程序是有缺陷的，因为它假设信号是排队的
~~~

图 8-36 中的 signal1 程序看起来相当简单。然而，当在 Linux 系统上运行它时，我们得到如下输出：

~~~shell
linux> ./signal1
Hello from child 14073
Hello from child 14074
Hello from child 14075
Handler reaped child
Handler reaped child
CR
Parent processing input
~~~

从输出中我们注意到，尽管发送了 3 个 SIGCHLD 信号给父进程，但是其中只有两个信号被接收了，因此父进程只是回收了两个子进程。如果挂起父进程，我们看到，实际上子进程 14075 没有被回收，它成了一个僵死进程（在 ps 命令的输出中由字符串 “defunct” 表明）：

~~~shell
Ctrl+Z
Suspended
linux> ps t
  PID TTY   STAT   TIME COMMAND
  .
  .
  .
14072 pts/3 T      0:02 ./signal1
14075 pts/3 Z      0:00 [signal1] <defunct>
14076 pts/3 R+     0:00 ps t
~~~

哪里出错了呢？问题就在于我们的代码没有解决信号不会排队等待这样的情况。所发生的情况是：父进程接收并捕获了第一个信号。当处理程序还在处理第一个信号时，第二个信号就传送并添加到了待处理信号集合里。然而，因为 SIGCHLD 信号被 SIGCHLD 处理程序阻塞了，所以第二个信号就不会被接收。此后不久，就在处理程序还在处理第一个信号时，第三个信号到达了。因为已经有了一个待处理的 SIGCHLD，第三个 SIGCHLD 信号会被丢弃。一段时间之后，处理程序返回，内核注意到有一个待处理的 SIGCHLD 信号，就迫使父进程接收这个信号。父进程捕获这个信号，并第二次执行处理程序。在处理程序完成对第二个信号的处理之后，已经没有待处理的 SIGCHLD 信号了，而且也绝不会再有，因为第三个 SIGCHLD 的所有信息都已经丢失了。由此得到的重要教训是，不可以用信号来对其他进程中发生的事件计数。

为了修正这个问题，我们必须回想一下，存在一个待处理的信号只是暗示自进程最后一次收到一个信号以来，至少已经有一个这种类型的信号被发送了。所以我们必须修改 SIGCHLD 的处理程序，使得每次 SIGCHLD 处理程序被调用时，回收尽可能多的僵死子进程。图 8-37 展示了修改后的 SIGCHLD 处理程序。

~~~c
void handler2(int sig)
{
    int olderrno = errno;

    while (waitpid(-1, NULL, 0) > 0) {
        Sio_puts("Handler reaped child\n");
    }

    if (errno != ECHILD)
        Sio_error("waitpid error");

    Sleep(1);
    errno = olderrno;
}

// 图 8-37 signal2：图 8-36 的一个改进版本，它能够正确解决信号不会排队等待的情况
~~~

~~~c
volatile long counter = 2; // Declare a volatile long counter, initialized to 2

/* Signal handler function for SIGUSR1 */
void handler1(int sig)
{
    sigset_t mask, prev_mask; // Declare signal sets for blocking and storing previous state

    Sigfillset(&mask); // Initialize 'mask' to contain all signals
    Sigprocmask(SIG_BLOCK, &mask, &prev_mask);  // Block all signals, saving previous state

    Sio_putl(--counter); // Safely decrement 'counter' and print it atomically

    Sigprocmask(SIG_SETMASK, &prev_mask, NULL); // Restore the previous signal mask

    _exit(0); // Terminate the process immediately
}

int main()
{
    pid_t pid; // Declare a variable to store the process ID
    sigset_t mask, prev_mask; // Declare signal sets for blocking and storing previous state

    printf("%ld", counter); // Print the current value of 'counter'
    fflush(stdout); // Flush the output buffer to ensure immediate printing

    signal(SIGUSR1, handler1); // Set handler1 as the handler for SIGUSR1 signals
    if ((pid = Fork()) == 0) { // Fork a child process
        while (1) {}; // Infinite loop in the child process
    }

    Kill(pid, SIGUSR1); // Send SIGUSR1 to the child process
    Waitpid(-1, NULL, 0); // Wait for the child process to finish

    Sigfillset(&mask); // Initialize 'mask' to contain all signals
    Sigprocmask(SIG_BLOCK, &mask, &prev_mask);  // Block all signals, saving previous state

    printf("%ld", ++counter); // Increment 'counter' and print its new value

    Sigprocmask(SIG_SETMASK, &prev_mask, NULL); // Restore the previous signal mask

    exit(0); // Exit the program
}
~~~

# 3. 可移植的信号处理

Unix 信号处理的另一个缺陷在于不同的系统有不同的信号处理语义。例如：
**signal 函数的语义各有不同。**有些老的 Unix 系统在信号 k 被处理程序捕获之后就把对信号 k 的反应恢复到默认值。在这些系统上，每次运行之后，处理程序必须调用 signal 函数，显式地重新设置它自己。

系统调用可以被中断。像 read、write 和 accept 这样的系统调用潜在地会阻塞进程一段较长的时间，称为慢速系统调用。在某些较早版本的 Unix 系统中，当处理程序捕获到一个信号时，被中断的慢速系统调用在信号处理程序返回时不再继续，而是立即返回给用户一个错误条件，并将 errno 设置为 EINTR。在这些系统上，程序员必须包括手动重启被中断的系统调用的代码。
要解决这些问题，Posix 标准定义了 sigaction 函数，它允许用户在设置信号处理时，明确指定他们想要的信号处理语义。

~~~c
#include <signal.h>

int sigaction(int signum, struct sigaction *act,
              struct sigaction *oldact);
// 返回：若成功则为 0，若出错则为 -1。
~~~

sigaction 函数运用并不广泛，因为它要求用户设置一个复杂结构的条目。一个更简洁的方式，最初是由 W. Richard Stevens 提出的【110】，就是定义一个包装函数，称为 Signal，它调用 sigaction。图 8-38 给出了 Signal 的定义，它的调用方式与 signal 函数的调用方式一样。

~~~c
handler_t *Signal(int signum, handler_t *handler)
{
    struct sigaction action, old_action;

    action.sa_handler = handler;
    sigemptyset(&action.sa_mask); /* Block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* Restart syscalls if possible */
    
    if (sigaction(signum, &action, &old_action) < 0)
        unix_error("Signal error");
    return (old_action.sa_handler);
}
~~~

Signal 包装函数设置了一个信号处理程序，其信号处理语义如下：
 - 只有这个处理程序当前正在处理的那种类型的信号被阻塞。
 - 和所有信号实现一样，信号不会排队等待。
 - 只要可能，被中断的系统调用会自动重启。
 - 一旦设置了信号处理程序，它就会一直保持，直到 Signal 带着 handler 参数为 SIG_IGN 或者 SIG_DFL 被调用。
我们在所有的代码中实现 Signal 包装函数。

# 同步流以避免讨厌的并发错误

如何编写读写相同存储位置的并发流程序的问题，困扰着数代计算机科学家。一般而言，流可能交错的数量与指令的数量呈指数关系。这些交错中的一些会产生正确的结果，而有些则不会。基本的问题是以某种方式同步并发流，从而得到最大的可行的交错的集合，每个可行的交错都能得到正确的结果。

并发编程是一个很深且很重要的问题，我们将在第 12 章中更详细地讨论。不过，在本章中学习的有关异常控制流的知识，可以让你感觉一下与并发相关的有趣的智力挑战。
例如，考虑图 8-39 中的程序，它总结了一个典型的 Unixshell 的结构。父进程在一个全局作业列表中记录着它的当前子进程，每个作业一个条目。addjob 和 deletejob 函数分别向这个作业列表添加和从中删除作业。

~~~c
/* WARNING: This code is buggy! */
void handler(int sig)
{
    int olderrno = errno;
    sigset_t mask_all, prev_all;
    pid_t pid;

    Sigfillset(&mask_all);
    while ((pid = waitpid(-1, NULL, 0)) > 0) { /* Reap a zombie child */
        Sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
        deletejob(pid); /* Delete the child from the job list */
        Sigprocmask(SIG_SETMASK, &prev_all, NULL);
    }
    if (errno != ECHILD)
        Sio_error("waitpid error");
    errno = olderrno;
}

int main(int argc, char **argv)
{
    int pid;
    sigset_t mask_all, prev_all;

    Sigfillset(&mask_all);
    Signal(SIGCHLD, handler);
    initjobs(); /* Initialize the job list */

    while (1) {
        if ((pid = Fork()) == 0) { /* Child process */
            Execve("/bin/date", argv, NULL);
        }
        Sigprocmask(SIG_BLOCK, &mask_all, &prev_all); /* Parent process */
        addjob(pid);  /* Add the child to the job list */
        Sigprocmask(SIG_SETMASK, &prev_all, NULL);
    }
    exit(0);
}
// 图 8-39 一个具有细微同步错误的 shell 程序。如果子进程在父进程能够开始运行前就结束了，那么 addjob 和 deletejob 会以错误的方式被调用
~~~

当父进程创建一个新的子进程后，它就把这个子进程添加到作业列表中。当父进程在 SIGCHLD 处理程序中回收一个终止的（僵死）子进程时，它就从作业列表中删除这个子进程。
信一看，这段代码是对的。不幸的是，可能发生下面这样的事件序列：
 1. 父进程执行 fork 函数，内核调度新创建的子进程运行，而不是父进程。
 2. 在父进程能够再次运行之前，子进程就终止，并且变成一个僵死进程，使得内核传递一个 SIGCHLD 信号给父进程。
 3. 后来，当父进程再次变成可运行但又在它执行之前，内核注意到有未处理的 SIGCHLD 信号，并通过在父进程中运行处理程序接收这个信号。
 4. 信号处理程序回收终止的子进程，并调用 deletejob，这个函数什么也不做，因为父进程还没有把该子进程添加到列表中。
 5. 在处理程序运行完毕后，内核运行父进程，父进程从 fork 返回，通过调用 add-job 错误地把（不存在的）子进程添加到作业列表中。

因此，对于父进程的 main 程序和信号处理流的某些交错，可能会在 addjob 之前调用 deletejob。这导致作业列表中出现一个不正确的条目，对应于一个不再存在而且永远也不会被删除的作业。另一方面，也有一些交错，事件按照正确的顺序发生。例如，如果在 fork 调用返回时，内核刚好调度父进程而不是子进程运行，那么父进程就会正确地把子进程添加到作业列表中，然后子进程终止，信号处理函数把该作业从列表中删除。

这是一个称为竞争（race）的经典同步错误的示例。在这个情况中，main 函数中调用 addjob 和处理程序中调用 deletejob 之间存在竞争。如果 addjob 赢得竞争，那么结果就是正确的。如果它没有，那么结果就是错误的。这样的错误非常难以调试，因为几乎不可能测试所有的交错。你可能运行这段代码十亿次，也没有一次错误，但是下一次测试却导致引发竞争的交错。

图 8-40 展示了消除图 8-39 中竞争的一种方法。通过在调用 fork 之前，阻塞 SIGCHLD 信号，然后在调用 addjob 之后取消阻塞这些信号，我们保证了在子进程被添加到作业列表中之后回收该子进程。注意，子进程继承了它们父进程的被阻塞集合，所以我们必须在调用 execve 之前，小心地解除子进程中阻塞的 SIGCHLD 信号。

~~~c
void handler(int sig)
{
    int olderrno = errno;
    sigset_t mask_all, prev_all;
    pid_t pid;

    Sigfillset(&mask_all);
    while ((pid = waitpid(-1, NULL, 0)) > 0) { /* Reap a zombie child */
        Sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
        deletejob(pid); /* Delete the child from the job list */
        Sigprocmask(SIG_SETMASK, &prev_all, NULL);
    }
    if (errno != ECHILD)
        Sio_error("waitpid error");
    errno = olderrno;
}

int main(int argc, char **argv)
{
    int pid;
    sigset_t mask_all, mask_one, prev_one;

    Sigfillset(&mask_all);
    Sigemptyset(&mask_one);
    Sigaddset(&mask_one, SIGCHLD);
    Signal(SIGCHLD, handler);
    initjobs(); /* Initialize the job list */
    
    while (1) {
        Sigprocmask(SIG_BLOCK, &mask_one, &prev_one); /* Block SIGCHLD */
        if ((pid = Fork()) == 0) { /* Child process */
            Sigprocmask(SIG_SETMASK, &prev_one, NULL); /* Unblock SIGCHLD */
            Execve("/bin/date", argv, NULL);
        }
        Sigprocmask(SIG_BLOCK, &mask_all, NULL); /* Parent process */
        addjob(pid); /* Add the child to the job list */
        Sigprocmask(SIG_SETMASK, &prev_one, NULL); /* Unblock SIGCHLD */
    }
    exit(0);
}
~~~

# 显式地等待信号
有时候主程序需要显式地等待某个信号处理程序运行。例如，当 Linux shell 创建一个前台作业时，在接收下一条用户命令之前，它必须等待作业终止，被 SIGCHLD 处理程序回收。

图 8-41 给出了一个基本的思路。父进程设置 SIGINT 和 SIGCHLD 的处理程序，然后进入一个无限循环。它阻塞 SIGCHLD 信号，避免 8.5.6 节中讨论过的父进程和子进程之间的竞争。创建了子进程之后，把 pid 重置为 0，取消阻塞 SIGCHLD，然后以循环的方式等待 pid 变为非零。子进程终止后，处理程序回收它，把它非零的 PID 赋值给全局 pid 变量。这会终止循环，父进程继续其他的工作，然后开始下一次迭代。

~~~c
#include "csapp.h"

volatile sig_atomic_t pid;

void sigchld_handler(int s)
{
    int olderrno = errno;
    pid = waitpid(-1, NULL, 0);
    errno = olderrno;
}

void sigint_handler(int s)
{
}

int main(int argc, char **argv)
{
    sigset_t mask, prev;

    Signal(SIGCHLD, sigchld_handler);
    Signal(SIGINT, sigint_handler);
    Sigemptyset(&mask);
    Sigaddset(&mask, SIGCHLD);

    while (1) {
        Sigprocmask(SIG_BLOCK, &mask, &prev); /* Block SIGCHLD */
        if (Fork() == 0) /* Child */
            exit(0);

        /* Parent */
        pid = 0;
        Sigprocmask(SIG_SETMASK, &prev, NULL); /* Unblock SIGCHLD */

        /* Wait for SIGCHLD to be received (wasteful) */
        while (!pid)
            ;

        /* Do some work after receiving SIGCHLD */
        printf(".");
    }
    exit(0);
}
~~~