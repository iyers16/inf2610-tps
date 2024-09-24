/*
 * Ecole Polytechnique Montreal - GIGL
 * Automne 2024
 * Challenges - part1.c
 *
 * Shanmukh Iyer 2251372; Michael Imbert 2212593
 */
#include "challenges_part1.h"

void traverse(char *filename, int indent, int fd)
{
    DIR *dir;
    struct dirent *entry;
    struct stat info;
    struct stat parent_info;
    char path[500];
    char file_names[100][256]; 
    int file_counter = 0;

    // info sur actuel
    if (stat(filename, &info) != 0)
    {
        perror("stat() child error");
        _exit(3);
    }

    // info sur parent
    char parent_path[500];
    strcpy(parent_path, filename);
    strcat(parent_path, "/..");

    if (stat(parent_path, &parent_info) != 0)
    {
        perror("stat() parent error");
        _exit(4);
    }

    if ((dir = opendir(filename)) == NULL)
    {
        perror("opendir() error");
        _exit(2);
    }
    else
    {
        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_name[0] != '.')
            {
                strcpy(path, filename);
                strcat(path, "/");
                strcat(path, entry->d_name);

                if (stat(path, &info) != 0)
                {
                    perror("stat() error");
                    _exit(3);
                }

                if (S_ISDIR(info.st_mode))
                {
                    pid_t child;
                    if ((child = fork()) == 0)
                    {
                        traverse(path, indent + 1, fd);
                        _exit(0);
                    }
                    waitpid(child, NULL, 0);
                }
                else if (strstr(entry->d_name, ".txt"))
                {
                    if (file_counter < 100)
                    {
                        strcpy(file_names[file_counter++], entry->d_name);
                    }
                }
            }
        }

        // info actuel
        dprintf(fd, "Répertoire: %s\n", filename);
        dprintf(fd, "Inode du répertoire: %ld\n", info.st_ino);
        dprintf(fd, "Inode du répertoire parent: %ld\n", parent_info.st_ino);
        dprintf(fd, "Nombre de fichiers texte: %d\n", file_counter);

        // info parent
        for (int i = 0; i < file_counter; i++)
        {
            dprintf(fd, "       {%s}\n", file_names[i]);
        }

        if (file_counter == 0)
        {
            dprintf(fd, "{Aucun fichier .txt dans ce répertoire}\n");
        }

        dprintf(fd, "\n\n");

        closedir(dir);
    }
}

int main(int argc, char *argv[])
{
    const char *pathname = "./challenges_output.txt";
    int flag = O_TRUNC | O_RDWR;
    int fd = open(pathname, flag);
    if (fd < 0)
    {
        perror("open()");
        _exit(1);
    }

    char *root = "./root";
    traverse(root, 0, fd);
    return 0;
}