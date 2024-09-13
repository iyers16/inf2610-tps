/*
 * Ecole Polytechnique Montreal - GIGL
 * Automne 2024
 * Processes - part1.c
 *
 * Shanmukh Iyer 2251372; Michael Imbert 2212593
 */

#include "libprocesslab/libprocesslab.h"

void question1()
{
    registerProc(getpid(), getppid(), 0, 0);
    pid_t proc11, proc12, proc13;

    if ((proc11 = fork()) == 0)
    {
        registerProc(getpid(), getppid(), 1, 1);
        pid_t proc21;
        if((proc21 = fork()) == 0)
        {
            registerProc(getpid(), getppid(), 2, 1);
            _exit(0);
        }
        waitpid(proc21, NULL, 0);
        _exit(0);
    }
    waitpid(proc11, NULL, 0);

    if ((proc12 = fork()) == 0)
    {
        registerProc(getpid(), getppid(), 1, 2);
        pid_t proc22;
        if((proc22 = fork()) == 0)
        {
            registerProc(getpid(), getppid(), 2, 2);
            _exit(0);
        }
        waitpid(proc22, NULL, 0);
        _exit(0);
    }
    waitpid(proc12, NULL, 0);

    if ((proc13 = fork()) == 0)
    {
        registerProc(getpid(), getppid(), 1, 3);
        pid_t proc23, proc24, proc25, proc26;
        if((proc23 = fork()) == 0)
        {
            registerProc(getpid(), getppid(), 2, 3);
            _exit(0);
        }
        waitpid(proc23, NULL, 0);

        if((proc24 = fork()) == 0)
        {
            registerProc(getpid(), getppid(), 2, 4);
            _exit(0);
        }
        waitpid(proc24, NULL, 0);

        if((proc25 = fork()) == 0)
        {
            registerProc(getpid(), getppid(), 2, 5);
            _exit(0);
        }
        waitpid(proc25, NULL, 0);

        if((proc26 = fork()) == 0)
        {
            registerProc(getpid(), getppid(), 2, 6);
            _exit(0);
        }
        waitpid(proc26, NULL, 0);
        _exit(0);
    }
    waitpid(proc13, NULL, 0);

    printProcRegistrations();
    return;
}
