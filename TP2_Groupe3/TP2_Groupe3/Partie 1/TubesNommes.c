#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

int main() {
    const char *P1_2 = "pipe1to2";
    const char *P2_3 = "pipe2to3";
    pid_t p1, p2, p3;
    mkfifo(P1_2, 0666);
    mkfifo(P2_3, 0666);

    if ((p3 = fork()) == 0) {
        if ((p2 = fork()) == 0) {
            if ((p1 = fork()) == 0) {
                // Open In.txt for reading
                int fd_in = open("In.txt", O_RDONLY);
                if (fd_in == -1) {
                    perror("open In.txt failed");
                    exit(1);
                }

                // Redirect stdin to In.txt
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);

                // Open pipe1to2 for writing and redirect stdout to it
                int pipe_fd1 = open(P1_2, O_WRONLY);
                if (pipe_fd1 == -1) {
                    perror("open pipe1to2 for writing failed");
                    exit(1);
                }
                dup2(pipe_fd1, STDOUT_FILENO);
                close(pipe_fd1);

                // Execute the first `rev` command
                execlp("rev", "rev", NULL);
                perror("exec rev failed");
                exit(1);
            }

            // Open pipe1to2 for reading and redirect stdin to it
            int pipe_fd1 = open(P1_2, O_RDONLY);
            if (pipe_fd1 == -1) {
                perror("open pipe1to2 for reading failed");
                exit(1);
            }
            dup2(pipe_fd1, STDIN_FILENO);
            close(pipe_fd1);

            // Open pipe2to3 for writing and redirect stdout to it
            int pipe_fd2 = open(P2_3, O_WRONLY);
            if (pipe_fd2 == -1) {
                perror("open pipe2to3 for writing failed");
                exit(1);
            }
            dup2(pipe_fd2, STDOUT_FILENO);
            close(pipe_fd2);

            // Execute the second `rev` command
            execlp("rev", "rev", NULL);
            perror("exec rev 2 failed");
            exit(1);
        }

        // Open pipe2to3 for reading and redirect stdin to it
        int pipe_fd2 = open(P2_3, O_RDONLY);
        if (pipe_fd2 == -1) {
            perror("open pipe2to3 for reading failed");
            exit(1);
        }
        dup2(pipe_fd2, STDIN_FILENO);
        close(pipe_fd2);

        // Execute the `cmp` command to compare output with `In.txt`
        execlp("cmp", "cmp", "-", "In.txt", "-s", NULL);
        perror("exec cmp failed");
        exit(1);
    }

    // Wait for p3 to finish
    int status;
    waitpid(p3, &status, 0);

    // Display the result based on cmp's exit status
    if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) == 0) {
            printf("Les fichiers sont identiques\n");
        } else {
            printf("Les fichiers sont différents\n");
        }
    }

    // Clean up named pipes
    unlink(P1_2);
    unlink(P2_3);

    return 0;
}
