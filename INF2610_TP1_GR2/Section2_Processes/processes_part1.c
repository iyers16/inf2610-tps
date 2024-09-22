/*
 * Ecole Polytechnique Montreal - GIGL
 * Automne 2024
 * Processes - part1.c
 *
 * Shanmukh Iyer 2251372; Michael Imbert 2212593
 */

#include "libprocesslab/libprocesslab.h"

int spawn_child(int level, int pctr)
{
    pid_t child;
    if((child = fork()) == 0)
    {
        registerProc(getpid(), getppid(), level, pctr);
        _exit(0);
    }
    waitpid(child, NULL, 0);
    return 1;
}


void question1()
{
    int total_children = 0;
    int status;
    registerProc(getpid(), getppid(), 0, 0);
    pid_t proc11, proc12, proc13;

    if ((proc11 = fork()) == 0)
    {
        registerProc(getpid(), getppid(), 1, 1);
        int children_ctr = spawn_child(2, 1);
        _exit(children_ctr);
    }
    waitpid(proc11, &status, 0);
    total_children += WEXITSTATUS(status);
    total_children ++;

    if ((proc12 = fork()) == 0)
    {
        registerProc(getpid(), getppid(), 1, 2);
        int children_ctr = spawn_child(2, 2);
        _exit(children_ctr);
    }
    waitpid(proc12, &status, 0);
    total_children += WEXITSTATUS(status);
    total_children ++;

    if ((proc13 = fork()) == 0)
    {
        registerProc(getpid(), getppid(), 1, 3);
        int children_ctr = 0;

        children_ctr += spawn_child(2, 3);
        children_ctr += spawn_child(2, 4);
        children_ctr += spawn_child(2, 5);
        children_ctr += spawn_child(2, 6);
        
        _exit(children_ctr);
    }
    waitpid(proc13, &status, 0);
    total_children += WEXITSTATUS(status); 
    total_children ++;

    printf("Total children created: %d\n", total_children);
    printProcRegistrations();
    execlp("ls", "ls", "-l", NULL);
    return;
}
