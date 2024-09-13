/*
 * Ecole Polytechnique Montreal - GIGL
 * Automne 2024
 * Processes - part1.c
 *
 * Shanmukh Iyer 2251372; Michael Imbert 2212593
 */

#include "libprocesslab/libprocesslab.h"

void spawn_child(int level, int pctr)
{
    pid_t child;
    if((child = fork()) == 0)
    {
        registerProc(getpid(), getppid(), level, pctr);
        _exit(0);
    }
    waitpid(child, NULL, 0);
}

void question1()
{
    registerProc(getpid(), getppid(), 0, 0);
    pid_t proc11, proc12, proc13;

    if ((proc11 = fork()) == 0)
    {
        registerProc(getpid(), getppid(), 1, 1);
        spawn_child(2, 1);
        _exit(0);
    }
    waitpid(proc11, NULL, 0);

    if ((proc12 = fork()) == 0)
    {
        registerProc(getpid(), getppid(), 1, 2);
        spawn_child(2, 2);
        _exit(0);
    }
    waitpid(proc12, NULL, 0);

    if ((proc13 = fork()) == 0)
    {
        registerProc(getpid(), getppid(), 1, 3);
        spawn_child(2, 3);
        spawn_child(2, 4);
        spawn_child(2, 5);
        spawn_child(2, 6);
        _exit(0);
    }
    waitpid(proc13, NULL, 0);

    printProcRegistrations();
    return;
}
