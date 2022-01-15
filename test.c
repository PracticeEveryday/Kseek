#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>



int main(int argc, char *argv[])
{

	int fd;
	char buf[1024] = "HELLO WORLD is nice";
	char buf2[1024] = "HELLO WORLD@@@@";
	char buf3[1024];
	int *ptr;
	bzero(buf3, sizeof(buf3));

	if((fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0644)) == -1) {
		perror("open failed");
		exit(1);
	}

	char *j = strtok(buf, "\0");
	char *k = strchr(buf,'\0');
	
	*k = ' ';
//	*j = ' ';
	printf("j : %p\n", j);
	printf("k : %p\n", k);

	int z = strlen(j);

	printf("z : %d\n", z);

	write(fd, buf, sizeof(buf));
	
	lseek(fd, z, SEEK_SET);
	write(fd, buf2, sizeof(buf2));
	
	lseek(fd, 0, SEEK_SET);
	read(fd, buf3, sizeof(buf3));

	printf("buf3 : %s\n", buf3);
}