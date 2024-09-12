/*
 * Ecole Polytechnique Montreal - GIGL
 * Automne 2024
 * Processes - part1.c
 *
 * Ajoutez vos noms, prénoms et matricules
*/

#include "libprocesslab/libprocesslab.h"

void spawn_child_processes(int n)
{
    pid_t child_pid;

    //Father code (before child processes start)

    for (int i = 0; i < n; i++) {
        if ((child_pid = fork()) == 0) {
            //child code
            printf("\nPID=%d\t PPID=%d\n", getpid(), getppid());
            exit(0);
        }
    }

    while (wait(NULL) > 0); // this way, the father waits for all the child processes 

    //Father code (After all child processes end)
    // printf("\nPID=%d\t PPID=%d\n", getpid(), getppid());
}

void question1()
{
    for (int i = 0; i < 2 ; ++i)
    {
        spawn_child_processes(2);
    }
    printf("\nPID=%d\t PPID=%d\n", getpid(), getppid());
}

