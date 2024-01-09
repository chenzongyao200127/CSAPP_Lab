# 接收信号

信号默认行为：
1. 进程终止。
2. 进程终止并转储内存。
3. 进程停止（挂起）直到被 SIGCONT 信号重启。
4. 进程忽略该信号。

~~~shell
| Number | Name      | Default Action | Corresponding Event                      |
| ------ | --------- | -------------- | ---------------------------------------- |
| 1      | SIGHUP    | Terminate      | 终端控制进程结束或控制终端关闭           |
| 2      | SIGINT    | Terminate      | 键盘中断（如 ctrl + c）                  |
| 3      | SIGQUIT   | Core           | 键盘退出信号                             |
| 4      | SIGILL    | Core           | 非法指令                                 |
| 5      | SIGTRAP   | Core           | 跟踪/断点捕获                            |
| 6      | SIGABRT   | Core           | Abort 信号由 abort(3) 发起               |
| 7      | SIGBUS    | Core           | 总线错误（错误的内存访问）               |
| 8      | SIGFPE    | Core           | 浮点异常                                 |
| 9      | SIGKILL   | Terminate      | 强制终止                                 |
| 10     | SIGUSR1   | Terminate      | 用户自定义信号1                          |
| 11     | SIGSEGV   | Core           | 无效的内存引用                           |
| 12     | SIGUSR2   | Terminate      | 用户自定义信号2                          |
| 13     | SIGPIPE   | Terminate      | 管道破裂: 写入无进程读取的管道           |
| 14     | SIGALRM   | Terminate      | 计时器信号（由 alarm(2) 发起）           |
| 15     | SIGTERM   | Terminate      | 终止信号                                 |
| 16     | SIGSTKFLT | Terminate      | 栈故障（不常用）                         |
| 17     | SIGCHLD   | Ignore         | 子进程结束信号                           |
| 18     | SIGCONT   | Continue       | 继续执行暂停的进程                       |
| 19     | SIGSTOP   | Stop           | 停止进程的执行                           |
| 20     | SIGTSTP   | Stop           | 终端停止信号                             |
| 21     | SIGTTIN   | Stop           | 后台进程从终端读取数据时                 |
| 22     | SIGTTOU   | Stop           | 后台进程向终端写数据时                   |
| 23     | SIGURG    | Ignore         | I/O紧急信号                              |
| 24     | SIGXCPU   | Core           | 超出CPU时间限制（由 setrlimit(2) 设置）  |
| 25     | SIGXFSZ   | Core           | 超出文件大小限制（由 setrlimit(2) 设置） |
| 26     | SIGVTALRM | Terminate      | 虚拟计时器到期（由 setitimer(2) 设置）   |
| 27     | SIGPROF   | Terminate      | 性能分析计时器到期                       |
| 28     | SIGWINCH  | Ignore         | 窗口大小变化                             |
| 29     | SIGIO     | Terminate      | I/O相关的可用或紧急信号                  |
| 30     | SIGPWR    | Terminate      | 电源故障（不常用）                       |
| 31     | SIGSYS    | Core           | 无效的系统调用                           |
~~~

~~~c
#include <signal.h>
typedef void (*sighandler_t)(int);

sighandler_t signal(int signum, sighandler_t handler);

// 返回：若成功则为指向前次处理程序的指针，若出错则为 SIG_ERR（不设置 errno）。
~~~

 - 如果 handler 是 SIG_IGN，那么忽略类型为 signum 的信号。
 - 如果 handler 是 SIG_DFL，那么类型为 signum 的信号行为恢复为默认行为。
 - 否则，handler 就是用户定义的函数的地址，这个函数被称为信号处理程序，只要进程接收到一个类型为 signum 的信号，就会调用这个程序。通过把处理程序的地址传递到 signal 函数从而改变默认行为，这叫做设置信号处理程序（installing the handler）。调用信号处理程序被称为捕获信号。执行信号处理程序被称为处理信号。

~~~c
// example.c
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
    (void)snooze(atoi(argv[1])); 26 exit(0);
}
~~~

# 阻塞和解除阻塞信号

Linux 提供阻塞信号的隐式和显式的机制：
 - **隐式阻塞机制。**
  内核默认阻塞任何当前处理程序正在处理信号类型的待处理的信号。例如，图 8-31 中，假设程序捕获了信号 s，当前正在运行处理程序 S。如果发送给该进程另一个信号 s，那么直到处理程序 S 返回，s 会变成待处理而没有被接收。
 - **显式阻塞机制。**
  应用程序可以使用 sigprocmask 函数和它的辅助函数，明确地阻塞和解除阻塞选定的信号。

~~~c
#include <signal.h>

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
int sigaddset(sigset_t *set, int signum);
int sigdelset(sigset_t *set, int signum);
//返回；如果成功则为 0，若出错则为 -1。

int sigismember(const sigset_t *set, int signum);
// 返回：若 signum 是 set 的成员则为 1，如果不是则为 0，若出错则为 -1。
~~~

sigprocmask 函数改变当前阻塞的信号集合（8.5.1 节中描述的 blocked 位向量）。具体的行为依赖于 how 的值：
SIG_BLOCK：把 set 中的信号添加到 blocked 中（blocked=blocked | set）。
SIG_UNBLOCK：从 blocked 中删除 set 中的信号（blocked=blocked &~set）。
SIG_SETMASK：block=set。

如果 oldset 非空，那么 blocked 位向量之前的值保存在 oldset 中。

使用下述函数对 set 信号集合进行操作：
1. sigemptyset 初始化 set 为空集合。
2. sigfillset 函数把每个信号都添加到 set 中。
3. sigaddset 函数把 signum 添加到 set，
4. sigdelset 从 set 中删除 signum，如果 signum 是 set 的成员，那么 sigismember 返回 1，否则返回 0。

例如，图 8-32 展示了如何用 sigprocmask 来临时阻塞接收 SIGINT 信号。
~~~c
sigset_t mask, prev_mask;

Sigemptyset(&mask);
Sigaddset(&mask, SIGINT);

/* Block SIGINT and save previous blocked set */
Sigprocmask(SIG_BLOCK, &mask, &prev_mask);
.
.  // Code region that will not be interrupted by SIGINT
.
/* Restore previous blocked set, unblocking SIGINT */
Sigprocmask(SIG_SETMASK, &prev_mask, NULL);
~~~
图 8-32 临时阻塞接收一个信号


# Code Example

~~~c
#include "csapp.h"

void sigint_handler(int sig) /* Safe SIGINT handler */
{
    Sio_puts("Caught SIGINT!\n"); /* Safe output */
    _exit(0);                     /* Safe exit */
}
~~~

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
~~~

~~~c
volatile long counter = 2;

void handler1(int sig)
{
    sigset_t mask, prev_mask;

    Sigfillset(&mask);
    Sigprocmask(SIG_BLOCK, &mask, &prev_mask);  /* Block sigs */
    Sio_putl(--counter);
    Sigprocmask(SIG_SETMASK, &prev_mask, NULL); /* Restore sigs */

    _exit(0);
}

int main()
{
    pid_t pid;
    sigset_t mask, prev_mask;

    printf("%ld", counter);
    fflush(stdout);

    signal(SIGUSR1, handler1);
    if ((pid = Fork()) == 0) {
        while (1) {};
    }
    Kill(pid, SIGUSR1);
    Waitpid(-1, NULL, 0);

    Sigfillset(&mask);
    Sigprocmask(SIG_BLOCK, &mask, &prev_mask);  /* Block sigs */
    printf("%ld", ++counter);
    Sigprocmask(SIG_SETMASK, &prev_mask, NULL); /* Restore sigs */

    exit(0);
}
~~~

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

~~~c
#include "csapp.h"

volatile sig_atomic_t pid;

void sigchld_handler(int s)
{
    int olderrno = errno;
    pid = Waitpid(-1, NULL, 0);
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

        /* Wait for SIGCHLD to be received */
        pid = 0;
        while (!pid)
            sigsuspend(&prev);

        /* Optionally unblock SIGCHLD */
        Sigprocmask(SIG_SETMASK, &prev, NULL);

        /* Do some work after receiving SIGCHLD */
        printf(".");
    }
    exit(0);
}
~~~


# Useful Func
1. **`waitpid` Function**
   - **Purpose**: `waitpid` is used in a process to wait for state changes in its child processes. It's more versatile than the simple `wait` function.
   - **Signature**: `pid_t waitpid(pid_t pid, int *status, int options);`
   - **Parameters**:
     - `pid`: The PID of the child process to wait for. Special values include `-1` (wait for any child process), `0` (wait for any child process in the same process group), or a specific PID.
     - `status`: Pointer to an integer where the status information of the stopped or terminated child process is stored.
     - `options`: Modify behavior of `waitpid`. Key options include `WUNTRACED` (also return if a child has stopped) and `WNOHANG` (return immediately if no child has exited).
   - **Return Value**: Returns the PID of the child process on success, `0` if `WNOHANG` was used and no child has exited, or `-1` on error.

2. **`kill` Function**
   - **Purpose**: `kill` is used to send signals to processes or process groups.
   - **Signature**: `int kill(pid_t pid, int sig);`
   - **Parameters**:
     - `pid`: PID of the target process or process group. Special values include `-1` (all processes you have permission to send signals to), `0` (the process group of the calling process), or a specific PID.
     - `sig`: The signal number to be sent (like `SIGKILL`, `SIGTERM`, `SIGSTOP`, etc.).
   - **Return Value**: Returns `0` on success and `-1` on error.

3. **`fork` Function**
   - **Purpose**: `fork` is used to create a new process by duplicating the calling process.
   - **Signature**: `pid_t fork(void);`
   - **Behavior**: The new process, called the child process, is an exact copy of the calling process, known as the parent process, except for a few details (like PID). After `fork`, both parent and child processes execute independently.
   - **Return Value**: Returns the child's PID to the parent process, `0` to the child process, and `-1` on error (with `errno` set appropriately).

4. **`execve` Function**
   - **Purpose**: `execve` is used to execute a program. It replaces the current process image with a new process image, specified by a file.
   - **Signature**: `int execve(const char *pathname, char *const argv[], char *const envp[]);`
   - **Parameters**:
     - `pathname`: Path to the executable file.
     - `argv`: An array of argument strings passed to the new program.
     - `envp`: An array of strings, conventionally of the form `key=value`, which are passed as the environment of the new program.
   - **Behavior**: `execve` does not return on success, as the calling process is completely replaced by the new process. If it fails, `-1` is returned.
   - **Return Value**: Only returns `-1` on failure.

5. **`setpgid` Function**
   - **Purpose**: `setpgid` is used to set the process group ID of a process, allowing for job control and signal management.
   - **Signature**: `int setpgid(pid_t pid, pid_t pgid);`
   - **Parameters**:
     - `pid`: PID of the target process. A value of `0` means the current process.
     - `pgid`: The process group ID to be set. A value of `0` sets the process group ID to the PID of the target process.
   - **Return Value**: Returns `0` on success and `-1` on failure.

6. **`sigprocmask` Function**
   - **Purpose**: `sigprocmask` is used to block or unblock signals, and/or to get/set the current set of blocked signals.
   - **Signature**: `int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);`
   - **Parameters**:
     - `how`: Specifies how the signal mask is changed (e.g., `SIG_BLOCK` to block the specified signals, `SIG_UNBLOCK` to unblock, `SIG_SETMASK` to set the signal mask).
     - `set`: Pointer to the set of signals to be blocked/unblocked/set.
     - `oldset`: If non-NULL, the previous mask is saved here.
   - **Return Value**: Returns `0` on success, `-1` on error.

7. **Options for `waitpid`**
   - **`WUNTRACED`**: Apart from returning if a child has stopped, `waitpid` also returns if the child process has stopped but not traced via `ptrace`.
   - **`WNOHANG`**: `waitpid` returns immediately if no child has exited. It is useful for non-blocking calls, where the parent can continue processing without being held up.

8. **Example of `waitpid` with `WNOHANG` and `WUNTRACED`**:
   ```c
   #include <sys/wait.h>
   #include <stdio.h>
   #include <unistd.h>

   int main() {
       pid_t pid = fork();

       if (pid == 0) { // Child process
           // Child process code
           sleep(2); // Simulate some work
           return 0;
       } else { // Parent process
           int status;
           while (waitpid(pid, &status, WNOHANG | WUNTRACED) == 0) {
               printf("Child is still running...\n");
               sleep(1); // Check again in 1 second
           }
           printf("Child exited with status %d\n", status);
       }
       return 0;
   }
   ```

9. **Example of `kill`**:
   ```c
   #include <signal.h>
   #include <stdio.h>
   #include <unistd.h>

   int main() {
       pid_t pid = fork();

       if (pid == 0) { // Child process
           // Infinite loop, waiting for a signal
           while(1) {
               sleep(1);
           }
       } else { // Parent process
           sleep(2); // Wait for the child to set up
           if (kill(pid, SIGINT) == -1) {
               perror("kill failed");
               return 1;
           }
           printf("SIGINT signal sent to child %d\n", pid);
       }
       return 0;
   }
   ```

10. **Example of `fork`**:
   ```c
   #include <stdio.h>
   #include <unistd.h>

   int main() {
       pid_t pid = fork();

       if (pid == 0) { // Child process
           printf("Hello from child process!\n");
       } else { // Parent process
           printf("Hello from parent process!\n");
       }
       return 0;
   }
   ```

11. **Example of `execve`**:
   ```c
   #include <unistd.h>

   int main() {
       char *args[] = {"/bin/ls", "-l", NULL};
       char *env[] = {NULL};

       if (fork() == 0) { // Child process
           execve("/bin/ls", args, env);
           // Note: execve does not return on success
           perror("execve failed");
       }
       // Parent continues here
       return 0;
   }
   ```

12. **Example of `setpgid`**:
   ```c
   #include <unistd.h>
   #include <stdio.h>

   int main() {
       pid_t pid = fork();

       if (pid == 0) { // Child process
           setpgid(0, 0); // Set its own group
           printf("Child process group ID: %d\n", getpgrp());
       } else { // Parent process
           printf("Parent process group ID: %d\n", getpgrp());
       }
       return 0;
   }
   ```

13. **Example of `sigprocmask`**:
   ```c
   #include <signal.h>
   #include <stdio.h>

   int main() {
       sigset_t newmask, oldmask;

       sigemptyset(&newmask);
       sigaddset(&newmask, SIGINT); // Block SIGINT

       // Block signals and save current signal mask
       if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
           perror("sigprocmask error");
           return 1;
       }

       // Critical code section...
       printf("SIGINT is blocked\n");

       // Restore original signal mask
       if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
           perror("sigprocmask error");
           return 1;
       }

       printf("SIGINT is unblocked\n");

       return 0;
   }
   ```



`eval` 函数中的信号操作在类似 shell 的程序中用于管理 shell 及其子进程如何处理信号，特别是 `SIGCHLD`。下面详细解释了函数中的每个信号操作：

1. **在父进程中阻塞 SIGCHLD（在 fork 之前）**：
   - `sigemptyset(&mask_one); sigaddset(&mask_one, SIGCHLD);`
   - 这些行初始化一个信号集（`mask_one`）并将 `SIGCHLD` 添加到此集中。当子进程终止、停止或在被停止后恢复时，`SIGCHLD` 会被发送给父进程。
   - `if (sigprocmask(SIG_BLOCK, &mask_one, &prev_one) < 0) { ... }`
   - 这里的 `sigprocmask` 调用阻塞了父进程中的 `SIGCHLD`。这样做是为了防止子进程在父进程有机会将子进程添加到其作业列表之前就发送 `SIGCHLD` 信号。这确保了父进程正确处理子进程的终止。

2. **在子进程中解除对 SIGCHLD 的阻塞（在 fork 之后）**：
   - `if ((pid = fork()) == 0) { if (sigprocmask(SIG_SETMASK, &prev_one, NULL) < 0) { ... } ... }`
   - 在由 `fork()` 创建的子进程中，信号掩码被重置为之前的状态（在父进程阻塞 `SIGCHLD` 之前）。这很重要，因为子进程继承了父进程的信号掩码，我们希望子进程对 `SIGCHLD` 有默认的行为。

3. **在父进程中暂时阻塞所有信号（在 fork 之后）**：
   - `sigfillset(&mask_all);`
   - 这行代码初始化另一个信号集（`mask_all`）并添加所有信号。
   - `sigprocmask(SIG_BLOCK, &mask_all, &prev_all);`
   - 这里，`sigprocmask` 用于暂时阻塞所有信号。这样做是为了保护添加子进程到作业列表（`addjob`）的关键部分。阻塞所有信号可以防止任何信号中断这部分代码，从而可能导致竞争条件。

4. **在父进程中恢复之前的信号掩码**：
   - `sigprocmask(SIG_SETMASK, &prev_all, NULL);`
   - 在将子进程添加到作业列表后，信号掩码被恢复为之前的状态（`prev_all`），重新启用了暂时被阻塞的信号的处理。

5. **前台作业等待和在接收到 SIGCHLD 时暂停执行**：
   - `while (fgpid(jobs) == pid) sigsuspend(&prev_one);`
   - 对于前台作业，父进程等待子进程完成。`sigsuspend` 临时用 `prev_one` 替换当前信号掩码（允许 `SIGCHLD`）并暂停进程，直到收到信号。当捕获到 `SIGCHLD`（表示子进程状态已改变）时，循环检查前台作业是否仍在运行，如果是，则再次暂停。这个循环确保了 shell 在前台作业完成之前不会继续执行。

6. **在作业管理之后解除对 SIGCHLD 的阻塞**：
   - `sigprocmask(SIG_SETMASK, &prev_one, NULL);`
   - 最后，`SIGCHLD` 再次被解除阻塞，恢复了父进程中正常的信号处理。

通过操纵信号掩码并仔细控制何时阻塞和解除阻塞信号，shell 能够正确且安全地管理作业，确保像 `SIGCHLD` 这样的信号在没有竞争条件的情况下得到适当的处理。


# Example Shell Code:
~~~c
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv)
{
    char *delim;         /* Points to first space delimiter */
    int argc;            /* Number of args */
    int bg;              /* Background job? */

    buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* Ignore leading spaces */
        buf++;

    /* Build the argv list */
    argc = 0;
    while ((delim = strchr(buf, ' '))) {
        argv[argc++] = buf;
        *delim = '\0';
        buf = delim + 1;
        while (*buf && (*buf == ' ')) /* Ignore spaces */
            buf++;
    }
    argv[argc] = NULL;

    if (argc == 0) /* Ignore blank line */
        return 1;

    /* Should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0)
        argv[--argc] = NULL;

    return bg;
}


/* eval - Evaluate a command line */
void eval(char *cmdline)
{
    char *argv[MAXARGS]; /* Argument list execve() */
    char buf[MAXLINE];   /* Holds modified command line */
    int bg;              /* Should the job run in bg or fg? */
    pid_t pid;           /* Process id */

    strcpy(buf, cmdline);
    bg = parseline(buf, argv);
    if (argv[0] == NULL)
        return;   /* Ignore empty lines */

    if (!builtin_command(argv)) {
        if ((pid = Fork()) == 0) {   /* Child runs user job */
            if (execve(argv[0], argv, environ) < 0) {
                printf("%s: Command not found.\n", argv[0]);
                exit(0);
            }
        }

        /* Parent waits for foreground job to terminate */
        if (!bg) {
            int status;
            if (waitpid(pid, &status, 0) < 0)
                unix_error("waitfg: waitpid error");
            }
        else
            printf("%d %s", pid, cmdline);
    }
    return;
}


/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv)
{
    if (!strcmp(argv[0], "quit")) /* quit command */
        exit(0);
    if (!strcmp(argv[0], "&"))    /* Ignore singleton & */
        return 1;
    return 0;                     /* Not a builtin command */
}

#include "csapp.h"
#define MAXARGS 128

/* Function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);

int main()
{
    char cmdline[MAXLINE]; /* Command line */

    while (1) {
        /* Read */
        printf("> ");
        Fgets(cmdline, MAXLINE, stdin);
        if (feof(stdin))
            exit(0);

        /* Evaluate */
        eval(cmdline);
    }
}
~~~

