/*
 * Ecole Polytechnique Montreal - GIGL
 * Automne 2024
 * Challenges - part1.c
 *
 * Shanmukh Iyer 2251372; Michael Imbert 2212593
 */
#include "challenges_part1.h"

void print_indents(int num, int fd)
{
    for (size_t i = 0; i < num; i++)
    {
        dprintf(fd, "    ");
    }
}

void traverse(char *filename, int indent, int fd)
{
    DIR *dir;
    dirent *entry;
    struct stat info;
    char path[500];

    // printing the curr directory name/path
    print_indents(indent, fd);
    dprintf(fd, "%s\n", filename);
    

    if ((dir = opendir(filename)) == NULL)
    {
        // this dir could not be opened
        perror("opendir() error");
        _exit(2);
    }
    else
    {
        int file_counter = 0;
        while ((entry = readdir(dir)) != NULL)
        {
            // we are able to read an entry in this stream still
            if (entry->d_name[0] != '.')
            {
                // we have discarded use of the . and .. dirs

                // build the next path
                strcpy(path, filename);
                strcat(path, "/");
                strcat(path, entry->d_name);
                if (stat(path, &info) != 0)
                {
                    // could not retrieve info on this inode path
                    perror("stat()");
                    _exit(3);
                }
                else if (S_ISDIR(info.st_mode))
                {
                    // we have a subdir in this dir
                    traverse(path, indent + 1, fd);
                }
                else
                {
                    // we have a file in this dir
                    print_indents(indent + 1, fd);
                    dprintf(fd, "%s\n", entry->d_name);
                    ++file_counter;
                }
            }
        }

        if (file_counter == 0)
        {
            print_indents(indent + 1, fd);
            dprintf(fd, "{Aucun fichier .txt dans ce dir}\n");
        }
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