/*
 * Ecole Polytechnique Montreal - GIGL
 * Automne 2024
 * Processes - part1.c
 *
 * Shanmukh Iyer 2251732;
 */

#include "libprocesslab/libprocesslab.h"

void spawn_child_processes(int level, int num_children, int ctr)
{
    pid_t child_pid;

    for (int i = 1; i <= num_children; i++)
    {
        if ((child_pid = fork()) == 0)
        {
            registerProc(getpid(), getppid(), 2, ctr+i-1);
            printf("\n\t\tLevel %d.%d\t PID=%d\t PPID=%d\n", level, ctr + i - 1, getpid(), getppid());
            _exit(0);
        }
        waitpid(child_pid, NULL, 0);
    }
}

void question1()
{
    printf("\nLevel 0 PID=%d\n", getpid());
    int ctr = 1;
    pid_t kids1 = fork();
    if (kids1 == 0)
    {
        registerProc(getpid(), getppid(), 1, 1);
        printf("\n\tLevel %d.%d\t PID=%d\t PPID=%d\n", 1, 1, getpid(), getppid());
        spawn_child_processes(2, 1, ctr);
        _exit(0);
    }
    ctr += 1;
    waitpid(kids1, NULL, 0);

    pid_t kids2 = fork();
    if (kids2 == 0)
    {
        registerProc(getpid(), getppid(), 1, 2);
        printf("\n\tLevel %d.%d\t PID=%d\t PPID=%d\n", 1, 2, getpid(), getppid());
        spawn_child_processes(2, 1, ctr);
        _exit(0);
    }
    ctr += 1;
    waitpid(kids2, NULL, 0);

    pid_t kids3 = fork();
    if (kids3 == 0)
    {
        registerProc(getpid(), getppid(), 1, 3);
        printf("\n\tLevel %d.%d\t PID=%d\t PPID=%d\n", 1, 3, getpid(), getppid());
        spawn_child_processes(2, 4, ctr);
        _exit(0);
    }
    ctr += 3;
    waitpid(kids3, NULL, 0);
    printProcRegistrations();
}
