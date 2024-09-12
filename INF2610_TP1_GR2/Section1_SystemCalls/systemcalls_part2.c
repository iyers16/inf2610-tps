/*
 * Ecole Polytechnique Montreal - GIGLL
 * Automne 2024
 * SystemCalls - part2.c
 *
 * Shanmukh Iyer 2251372; Michael Imbert 2212593
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void part21()
{
    char message[] = "77dbcb01f571f1c32g196c3a7d27f62e (printed using printf)";
    size_t i = 0;
    while (message[i] != '\0')
    {
        printf("%c", message[i]);
        ++i;
    }

    write(1, "77dbcb01f571f1c32g196c3a7d27f62e (printed using write)\n", 56);
    printf("\n");
}

void part22()
{
    setvbuf(stdout, NULL, _IONBF, 0);
    part21();
}

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        printf("Le programme a un seul paramètre : 1 ou 2\n");
        return 1;
    }
    switch (atoi(argv[1]))
    {
    case 1:
        part21();
        break;
    case 2:
        part22();
        break;
    default:
        printf(" Le paramètre du programme est invalide\n");
        return 1;
    }
    return 0;
}
