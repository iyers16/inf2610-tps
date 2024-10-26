#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

char *reverseString(char *str);

/*
Main process creates P3:
    P3 creates P2:
        P2 creates P1:
            P1 reads the contents of a file
            reverses the string
            publishes the reversal to a pipe connecting P1(publisher) -> P2(subscriber)
            exit
        reads the contents of the P1 -> P2 pipe
        reverses the string again
        publishes to a P2 -> P3 pipe
        exit
    reads the contents of the P2 -> P3 pipe
    compares to the original file contents
    exits with result code
main process picks up the status code and outputs the result
exit
*/
int main()
{
    int pipe1_2[2], pipe2_3[2];
    pipe(pipe1_2);
    pipe(pipe2_3);

    pid_t p1, p2, p3;

    if ((p3 = fork()) == 0)
    {
        if ((p2 = fork()) == 0)
        {
            if ((p1 = fork()) == 0)
            {
                printf("Inside p1\n");
                int file = open("In.txt", O_RDONLY);
                if (file == -1)
                {
                    perror("Error opening the file\n");
                    exit(EXIT_FAILURE);
                }

                char readBuf[600];
                int r = read(file, readBuf, sizeof(readBuf));
                if (r == -1)
                {
                    perror("Error reading file\n");
                    exit(EXIT_FAILURE);
                }

                reverseString(readBuf);

                close(pipe1_2[0]);
                write(pipe1_2[1], readBuf, sizeof(readBuf));
                close(pipe1_2[1]);

                exit(EXIT_SUCCESS);
            }
            waitpid(p1, NULL, 0);
            printf("Inside p2\n");

            close(pipe1_2[1]);
            char readBuf[600];
            int r = read(pipe1_2[0], readBuf, sizeof(readBuf));
            close(pipe1_2[0]);
            if (r == -1)
            {
                perror("Error reading from the pipe1_2\n");
                exit(EXIT_FAILURE);
            }

            reverseString(readBuf);

            close(pipe2_3[0]);
            write(pipe2_3[1], readBuf, sizeof(readBuf));
            close(pipe2_3[1]);
            exit(EXIT_SUCCESS);
        }
        waitpid(p2, NULL, 0);
        printf("Inside p3\n");

        int file = open("In.txt", O_RDWR);
        if (file == -1)
        {
            perror("Error opening the file\n");
            exit(EXIT_FAILURE);
        }

        char readBuf[600];
        int r = read(file, readBuf, sizeof(readBuf));
        if (r == -1)
        {
            perror("Error reading file\n");
            exit(EXIT_FAILURE);
        }

        close(pipe2_3[1]);
        char readBuf2[600];
        int r2 = read(pipe2_3[0], readBuf2, sizeof(readBuf2));
        close(pipe2_3[0]);
        if (r2 == -1)
        {
            perror("Error reading from the pipe1_2\n");
            exit(EXIT_FAILURE);
        }
        exit(strcmp(readBuf, readBuf2));
    }
    int status;
    waitpid(p3, &status, 0);
    if (!WEXITSTATUS(status))
    {
        printf("Le string doublement inverse est identique a loriginal\n");
    }
    else
    {
        printf("Le string doublement inverse nest pas identique a loriginal\n");
    }

    return 0;
}

char *reverseString(char *str)
{
    int i = 0;
    int j = strlen(str) - 1;
    while (i < j)
    {
        char tmp = str[i];
        str[i] = str[j];
        str[j] = tmp;
        i++;
        j--;
    }
    return str;
}
