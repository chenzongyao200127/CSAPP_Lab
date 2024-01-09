/*
 * tsh - A tiny shell program with job control
 *
 * Chen Zongyao
 * solar1s@seu.edu.cn
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

/* Misc manifest constants */
#define MAXLINE 1024   /* max line size */
#define MAXARGS 128    /* max args on a command line */
#define MAXJOBS 16     /* max jobs at any point in time */
#define MAXJID 1 << 16 /* max job ID */

/* Job states */
#define UNDEF 0 /* undefined */
#define FG 1    /* running in foreground */
#define BG 2    /* running in background */
#define ST 3    /* stopped */

/*
 * Jobs states: FG (foreground), BG (background), ST (stopped)
 * Job state transitions and enabling actions:
 *     FG -> ST  : ctrl-z
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 * At most 1 job can be in the FG state.
 */

/* Global variables */
extern char **environ;   /* defined in libc */
char prompt[] = "tsh> "; /* command line prompt (DO NOT CHANGE) */
int verbose = 0;         /* if true, print additional output */
int nextjid = 1;         /* next job ID to allocate */
char sbuf[MAXLINE];      /* for composing sprintf messages */

struct job_t
{                          /* The job struct */
    pid_t pid;             /* job PID */
    int jid;               /* job ID [1, 2, ...] */
    int state;             /* UNDEF, BG, FG, or ST */
    char cmdline[MAXLINE]; /* command line */
};
struct job_t jobs[MAXJOBS]; /* The job list */
/* End global variables */

/* Function prototypes */

/* Here are the functions that you will implement */
void eval(char *cmdline);
int builtin_cmd(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

/* Here are helper routines that we've provided for you */
int parseline(const char *cmdline, char **argv);
void sigquit_handler(int sig);

void clearjob(struct job_t *job);
void initjobs(struct job_t *jobs);
int maxjid(struct job_t *jobs);
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline);
int deletejob(struct job_t *jobs, pid_t pid);
pid_t fgpid(struct job_t *jobs);
struct job_t *getjobpid(struct job_t *jobs, pid_t pid);
struct job_t *getjobjid(struct job_t *jobs, int jid);
int pid2jid(pid_t pid);
void listjobs(struct job_t *jobs);

void usage(void);
void unix_error(char *msg);
void app_error(char *msg);
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);

/*
 * main - The shell's main routine
 */
int main(int argc, char **argv)
{
    char c;
    char cmdline[MAXLINE];
    int emit_prompt = 1; /* emit prompt (default) */

    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout) */
    dup2(1, 2);

    /* Parse the command line */
    while ((c = getopt(argc, argv, "hvp")) != EOF)
    {
        switch (c)
        {
        case 'h': /* print help message */
            usage();
            break;
        case 'v': /* emit additional diagnostic info */
            verbose = 1;
            break;
        case 'p':            /* don't print a prompt */
            emit_prompt = 0; /* handy for automatic testing */
            break;
        default:
            usage();
        }
    }

    /* Install the signal handlers */

    /* These are the ones you will need to implement */
    Signal(SIGINT, sigint_handler);   /* ctrl-c */
    Signal(SIGTSTP, sigtstp_handler); /* ctrl-z */
    Signal(SIGCHLD, sigchld_handler); /* Terminated or stopped child */

    /* This one provides a clean way to kill the shell */
    Signal(SIGQUIT, sigquit_handler);

    /* Initialize the job list */
    initjobs(jobs);

    /* Execute the shell's read/eval loop */
    while (1)
    {

        /* Read command line */
        if (emit_prompt)
        {
            printf("%s", prompt);
            fflush(stdout);
        }
        if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
            app_error("fgets error");
        if (feof(stdin))
        { /* End of file (ctrl-d) */
            fflush(stdout);
            exit(0);
        }

        /* Evaluate the command line */
        eval(cmdline);
        fflush(stdout);
        fflush(stdout);
    }

    exit(0); /* control never reaches here */
}

/*
 * eval - Evaluate the command line that the user has just typed in
 *
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.
 */
void eval(char *cmdline)
{
    char buf[MAXLINE];
    char *argv[MAXARGS];
    int bg;
    pid_t pid;
    sigset_t mask, prev;

    strcpy(buf, cmdline);
    if (!(bg = parseline(buf, argv)))
        return;

    if (builtin_cmd(argv))
        return;

    // Simplify signal set initialization and block SIGCHLD
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    if (sigprocmask(SIG_BLOCK, &mask, &prev) < 0)
    {
        perror("sigprocmask error");
        return;
    }

    if ((pid = fork()) < 0)
    {
        perror("fork error");
        return;
    }

    if (pid == 0)
    { // Child process
        setpgid(0, 0);
        if (sigprocmask(SIG_SETMASK, &prev, NULL) < 0)
        {
            perror("sigprocmask error");
        }
        if (execve(argv[0], argv, environ) < 0)
        {
            printf("%s: Command not found\n", argv[0]);
            exit(0);
        }
    }

    // Parent process
    sigset_t mask_all;
    sigfillset(&mask_all);

    if (sigprocmask(SIG_BLOCK, &mask_all, NULL) < 0)
    {
        perror("sigprocmask error");
    }

    addjob(jobs, pid, (bg ? BG : FG), cmdline);

    if (sigprocmask(SIG_SETMASK, &prev, NULL) < 0)
    {
        perror("sigprocmask error");
    }

    if (bg)
        printf("[%d] (%d) %s", pid2jid(pid), pid, cmdline);
    else
        waitfg(pid);
    return;
}

/*
 * parseline - Parse the command line and build the argv array.
 *
 * Characters enclosed in single quotes are treated as a single
 * argument.  Return true if the user has requested a BG job, false if
 * the user has requested a FG job.
 */
int parseline(const char *cmdline, char **argv)
{
    static char array[MAXLINE]; /* holds local copy of command line */
    char *buf = array;          /* ptr that traverses command line */
    char *delim;                /* points to first space delimiter */
    int argc;                   /* number of args */
    int bg;                     /* background job? */

    strcpy(buf, cmdline);
    buf[strlen(buf) - 1] = ' ';   /* replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* ignore leading spaces */
        buf++;

    /* Build the argv list */
    argc = 0;
    if (*buf == '\'') // Check if the first character is a single quote
    {
        buf++;                     // Skip the single quote
        delim = strchr(buf, '\''); // Find the next single quote
    }
    else
    {
        delim = strchr(buf, ' '); // Find the next space
    }

    while (delim)
    {
        argv[argc++] = buf;           // Assign the start of the argument to argv
        *delim = '\0';                // Null-terminate the current argument
        buf = delim + 1;              // Move the buffer pointer past the delimiter
        while (*buf && (*buf == ' ')) /* ignore spaces */
            buf++;

        // Repeat the process for the next argument
        if (*buf == '\'')
        {
            buf++;
            delim = strchr(buf, '\'');
        }
        else
        {
            delim = strchr(buf, ' ');
        }
    }
    argv[argc] = NULL; // Null-terminate the argv array

    if (argc == 0) /* ignore blank line */
        return 1;

    /* should the job run in the background? */
    if ((bg = (*argv[argc - 1] == '&')) != 0)
    {
        argv[--argc] = NULL; // Remove the '&' from argv
    }
    return bg; // Return whether it's a background job
}

/*
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.
 */
int builtin_cmd(char **argv)
{
    if (!strcmp(argv[0], "quit")) /* quit command */
        exit(0);
    if (!strcmp(argv[0], "&")) /* Ignore singleton & */
        return 1;
    if (!strcmp(argv[0], "jobs"))
    {
        listjobs(jobs);
        return 1;
    }
    // fg or bg
    if (!strcmp(argv[0], "fg") || !strcmp(argv[0], "bg"))
    {
        do_bgfg(argv);
        return 1;
    }
    return 0; /* Not a builtin command */
}

/*
 * do_bgfg - Execute the builtin bg and fg commands
 */
void do_bgfg(char **argv)
{
    struct job_t *job = NULL; // Pointer to the job to be handled
    int state;                // Command state (BG or FG)
    int id;                   // Stores Job ID (jid) or Process ID (pid)

    // Determine the command (bg or fg)
    if (!strcmp(argv[0], "bg"))
    {
        state = BG;
    }
    else
    {
        state = FG;
    }

    // Check for missing arguments
    if (argv[1] == NULL)
    {
        printf("%s command requires PID or %%jobid argument\n", argv[0]);
        return;
    }

    // Handle job ID input
    if (argv[1][0] == '%')
    {
        if (sscanf(&argv[1][1], "%d", &id) > 0)
        {
            job = getjobjid(jobs, id); // Get the job using job ID
            if (job == NULL)
            {
                printf("%%%d: No such job\n", id);
                return;
            }
        }
    }
    // Handle invalid input
    else if (!isdigit(argv[1][0]))
    {
        printf("%s: argument must be a PID or %%jobid\n", argv[0]);
        return;
    }
    // Handle process ID input
    else
    {
        id = atoi(argv[1]);
        job = getjobpid(jobs, id); // Get the job using process ID
        if (job == NULL)
        {
            printf("(%d): No such process\n", id);
            return;
        }
    }

    // Send SIGCONT signal to the job's process group
    if (kill(-(job->pid), SIGCONT) < 0)
    {
        perror("kill (SIGCONT)");
        return;
    }

    // Update the job's state
    job->state = state;

    // Print job info if moved to background, or wait if moved to foreground
    if (state == BG)
    {
        printf("[%d] (%d) %s", job->jid, job->pid, job->cmdline);
    }
    else
    {
        waitfg(job->pid);
    }
}

/*
 * waitfg - Block until process pid is no longer the foreground process
 *
 * One of the tricky parts of the assignment is deciding on the allocation of work between the waitfg and sigchld handler functions.
 * We recommend the following approach:
 * In waitfg, use a busy loop around the sleep function.
 */
void waitfg(pid_t pid)
{
    sigset_t mask_all, prev_all;
    sigfillset(&mask_all);
    sigprocmask(SIG_BLOCK, &mask_all, &prev_all);

    while (fgpid(jobs) == pid)
    {
        // Busy loop with sleep to reduce CPU usage
        sleep(1);
    }

    sigprocmask(SIG_SETMASK, &prev_all, NULL);
    return;
}

/*****************
 * Signal handlers
 *****************/

/*
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.
 */
void sigchld_handler(int sig)
{
    int olderrno = errno;
    sigset_t mask_all, prev_all;
    sigfillset(&mask_all);

    pid_t pid;
    int status;

    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0)
    {
        sigprocmask(SIG_BLOCK, &mask_all, &prev_all); // Block signals when updating the job list

        if (WIFEXITED(status))
        {
            // If the child exited normally
            deletejob(jobs, pid);
        }
        else if (WIFSIGNALED(status))
        {
            // If the child was terminated by a signal
            printf("Job [%d] (%d) terminated by signal %d\n", pid2jid(pid), pid, WTERMSIG(status));
            deletejob(jobs, pid);
        }
        else if (WIFSTOPPED(status))
        {
            // If the child was stopped
            printf("Job [%d] (%d) stopped by signal %d\n", pid2jid(pid), pid, WSTOPSIG(status));
            struct job_t *jp = getjobpid(jobs, pid);
            if (jp != NULL)
            {
                jp->state = ST;
            }
        }

        sigprocmask(SIG_SETMASK, &prev_all, NULL); // Restore previous signal mask
    }

    errno = olderrno;
}

/*
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.
 */
void sigint_handler(int sig)
{
    int olderrno = errno;
    int pid;
    sigset_t mask_all, prev_all;
    sigfillset(&mask_all);
    sigprocmask(SIG_BLOCK, &mask_all, &prev_all);

    if ((pid = fgpid(jobs)) != 0)
    {
        sigprocmask(SIG_SETMASK, &prev_all, NULL);
        if (kill(-pid, SIGINT) < 0)
        {
            unix_error("kill (SIGINT)");
        }
    }

    errno = olderrno;
    return;
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.
 */
void sigtstp_handler(int sig)
{
    int olderrno = errno;
    int pid;
    sigset_t mask_all, prev_all;
    sigfillset(&mask_all);
    sigprocmask(SIG_BLOCK, &mask_all, &prev_all);

    if ((pid = fgpid(jobs)) > 0)
    {
        sigprocmask(SIG_SETMASK, &prev_all, NULL);
        if (kill(-pid, SIGSTOP) < 0)
        {
            unix_error("kill (SIGSTOP)");
        }
    }

    errno = olderrno;
    return;
}

/*********************
 * End signal handlers
 *********************/

/***********************************************
 * Helper routines that manipulate the job list
 **********************************************/
/* clearjob - Clear the entries in a job struct */
void clearjob(struct job_t *job)
{
    job->pid = 0;           // Set process ID to 0
    job->jid = 0;           // Set job ID to 0
    job->state = UNDEF;     // Set job state to undefined
    job->cmdline[0] = '\0'; // Clear the command line string
}

/* initjobs - Initialize the job list */
void initjobs(struct job_t *jobs)
{
    int i;

    for (i = 0; i < MAXJOBS; i++)
        clearjob(&jobs[i]); // Clear each job in the list
}

/* maxjid - Returns largest allocated job ID */
int maxjid(struct job_t *jobs)
{
    int i, max = 0;

    for (i = 0; i < MAXJOBS; i++)
        if (jobs[i].jid > max)
            max = jobs[i].jid; // Find the highest job ID
    return max;
}

/* addjob - Add a job to the job list */
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline)
{
    int i;

    if (pid < 1)
        return 0; // Reject invalid PIDs

    for (i = 0; i < MAXJOBS; i++)
    {
        if (jobs[i].pid == 0) // Find an empty job slot
        {
            jobs[i].pid = pid;
            jobs[i].state = state;
            jobs[i].jid = nextjid++; // Assign a new job ID
            if (nextjid > MAXJOBS)
                nextjid = 1;                  // Reset job ID counter if it exceeds MAXJOBS
            strcpy(jobs[i].cmdline, cmdline); // Copy command line to job
            if (verbose)
            {
                printf("Added job [%d] %d %s\n", jobs[i].jid, jobs[i].pid, jobs[i].cmdline);
            }
            return 1;
        }
    }
    printf("Tried to create too many jobs\n"); // Job list is full
    return 0;
}

/* deletejob - Delete a job whose PID=pid from the job list */
int deletejob(struct job_t *jobs, pid_t pid)
{
    int i;

    if (pid < 1)
        return 0; // Reject invalid PIDs

    for (i = 0; i < MAXJOBS; i++)
    {
        if (jobs[i].pid == pid) // Find the job with the given PID
        {
            clearjob(&jobs[i]);         // Clear the job
            nextjid = maxjid(jobs) + 1; // Update the next job ID
            return 1;
        }
    }
    return 0; // Job not found
}

/* fgpid - Return PID of current foreground job, 0 if no such job */
pid_t fgpid(struct job_t *jobs)
{
    int i;

    for (i = 0; i < MAXJOBS; i++)
        if (jobs[i].state == FG) // Check if the job is in the foreground
            return jobs[i].pid;
    return 0; // No foreground job found
}

/* getjobpid  - Find a job (by PID) on the job list */
struct job_t *getjobpid(struct job_t *jobs, pid_t pid)
{
    int i;

    if (pid < 1)
        return NULL; // Reject invalid PIDs
    for (i = 0; i < MAXJOBS; i++)
        if (jobs[i].pid == pid) // Find the job with the given PID
            return &jobs[i];
    return NULL; // Job not found
}

/* getjobjid  - Find a job (by JID) on the job list */
struct job_t *getjobjid(struct job_t *jobs, int jid)
{
    int i;

    if (jid < 1)
        return NULL; // Reject invalid job IDs
    for (i = 0; i < MAXJOBS; i++)
        if (jobs[i].jid == jid) // Check if the job ID matches
            return &jobs[i];    // Return a pointer to the job
    return NULL;                // Job not found
}

/* pid2jid - Map process ID to job ID */
int pid2jid(pid_t pid)
{
    int i;

    if (pid < 1)
        return 0; // Reject invalid process IDs
    for (i = 0; i < MAXJOBS; i++)
        if (jobs[i].pid == pid) // Check if the process ID matches
        {
            return jobs[i].jid; // Return the corresponding job ID
        }
    return 0; // PID not found in the job list
}

/* listjobs - Print the job list */
void listjobs(struct job_t *jobs)
{
    int i;

    for (i = 0; i < MAXJOBS; i++)
    {
        if (jobs[i].pid != 0) // Check if the job slot is in use
        {
            printf("[%d] (%d) ", jobs[i].jid, jobs[i].pid); // Print job ID and process ID
            switch (jobs[i].state)
            {
            case BG:
                printf("Running "); // Job is running in the background
                break;
            case FG:
                printf("Foreground "); // Job is running in the foreground
                break;
            case ST:
                printf("Stopped "); // Job is stopped
                break;
            default:
                printf("listjobs: Internal error: job[%d].state=%d ",
                       i, jobs[i].state); // Error case for undefined states
            }
            printf("%s", jobs[i].cmdline); // Print the command line of the job
        }
    }
}

/******************************
 * end job list helper routines
 ******************************/

/***********************
 * Other helper routines
 ***********************/

/*
 * usage - print a help message
 */
void usage(void)
{
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -p   do not emit a command prompt\n");
    exit(1);
}

/*
 * unix_error - unix-style error routine
 */
void unix_error(char *msg)
{
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

/*
 * app_error - application-style error routine
 */
void app_error(char *msg)
{
    fprintf(stdout, "%s\n", msg);
    exit(1);
}

/*
 * Signal - wrapper for the sigaction function
 */
handler_t *Signal(int signum, handler_t *handler)
{
    struct sigaction action, old_action;

    action.sa_handler = handler;
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
        unix_error("Signal error");
    return (old_action.sa_handler);
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig)
{
    printf("Terminating after receipt of SIGQUIT signal\n");
    exit(1);
}
