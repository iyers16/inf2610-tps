/*
 * Ecole Polytechnique Montreal - GIGL
 * Automne 2024
 * SystemCalls - part1.c
 *
 * Shanmukh Iyer 2251372; Michael Imbert 2212593
*/

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main () {
	const char *pathname = "./systemcalls_output2.txt";
	int flag = O_TRUNC | O_RDWR;
	int fd = open(pathname, flag);
	if(fd < 0) {
		perror("Appel système open a échoué\n");
		exit(2);
	}
	printf("%d\n", fd);
	write(1, "Saisissez votre texte suivi de CTRL-D : ", 41);
	
	char c;
	while(read(0, &c, 1) > 0) {
		write(fd, &c, 1);
	}
	close(fd);
	return 0;
}
