/*
 * Ecole Polytechnique Montreal - GIGL
 * Automne 2024
 * Processes - part1.c
 *
 * Ajoutez vos noms, prénoms et matricules
*/

#include "libprocesslab/libprocesslab.h"

void spawn_child_processes(int n, int curr)
{
    pid_t child_pid;

    //Father code (before child processes start)

    for (int i = 1; i <= n; i++) {
        if ((child_pid = fork()) == 0) {
            //child code
            printf("\n%d.%d\t PID=%d\t PPID=%d\n", curr, i, getpid(), getppid());
            exit(0);
        }
    }

    while (wait(NULL) > 0); // this way, the father waits for all the child processes 

    //Father code (After all child processes end)
    // printf("\nPID=%d\t PPID=%d\n", getpid(), getppid());
}

void question1()
{
    printf("\nPID=%d\n", getpid());
    for (int i = 1; i <= 2 ; ++i)
    {
        spawn_child_processes(2, i);
    }
    
}

