#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int main()
{
    pid_t p1, p2, p3;

    if ((p3 = fork()) == 0)
    {
        int pipe2to3[2];
        pipe(pipe2to3);

        if ((p2 = fork()) == 0)
        {
            int pipe1to2[2];
            pipe(pipe1to2);

            if ((p1 = fork()) == 0)
            {
                // open file In.txt
                int fd = open("In.txt", O_RDONLY);
                if (fd == -1)
                {
                    perror("open");
                    exit(1);
                }

                // redirect stdin to point to In.txt
                dup2(fd, STDIN_FILENO);
                close(fd);

                // redirect stdout to the writing end of pipe1to2
                dup2(pipe1to2[1], STDOUT_FILENO);
                close(pipe1to2[0]);
                close(pipe1to2[1]);
                execlp("rev", "rev", NULL);
                perror("exec rev failed");
                exit(1);
            }
            waitpid(p1, NULL, 0);

            // redirect stdin to the reading end of pipe1to2
            dup2(pipe1to2[0], STDIN_FILENO);
            close(pipe1to2[0]);
            close(pipe1to2[1]);

            // redirect stdout to the writing end of pipe2to3
            dup2(pipe2to3[1], STDOUT_FILENO);
            close(pipe2to3[0]);
            close(pipe2to3[1]);
            execlp("rev", "rev", NULL);
            perror("exec rev 2 failed");
            exit(1);
        }
        waitpid(p2, NULL, 0);

        // redirect stdin to the reading end of pipe2to3
        dup2(pipe2to3[0], STDIN_FILENO);
        close(pipe2to3[0]);
        close(pipe2to3[1]);

        execlp("cmp", "cmp", "-", "In.txt", "-s", NULL);
        perror("exec cmp failed");
        exit(1);
    }
    int status;
    waitpid(p3, &status, 0);
    if (WIFEXITED(status))
    {
        if (WEXITSTATUS(status) == 0)
        {
            printf("Les fichiers sont identiques\n");
        }
        else
        {
            printf("Les fichiers sont différents\n");
        }
    }
    return 0;
}
