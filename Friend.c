#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>


#include "friend.h"

void print_list(int fd, int page, FRIEND_T fr);
int str_fit(char *str, char *bline, int size, int maxc);
int readline(int fd, char *buf, int bufsize);
int check = 0;
void edit(int fd, FRIEND_T fr);
void detailInfo(int fd, FRIEND_T fr, int num);
void add(int fd, FRIEND_T fr);


int main(int argc, char *argv[])
{
	int fd;
	FRIEND_T fr;
	char buf[1024];

	if ((fd = open("info", O_RDWR)) == -1) {
		perror("open failed");
		exit(1);
	}
/*
	while(read(fd, &fr, sizeof(fr)) >0) {
		printf("fr.name : %s\n", fr.name);
		printf("fr.address : %s\n", fr.address);
	}
*/

	while(1) {
		if(check == 0) {
//			system("clear");
			print_list(fd, 1, fr);
			check++;
		}
		
		printf("Command >> \n ");
		fflush(stdout);
		readline(0, buf, sizeof(buf));
//		printf("readline : %s\n", buf);

		if(!strcmp(buf, "quit")) {
//			system("clear");
			printf("Good bye~\n");
			break;

		} else if (!strcmp(buf, "next")) {
//			system("clear");
			check++;
			print_list(fd, check, fr);

		} else if (!strcmp(buf, "pre")) {
//			system("clear");
			check--;
			print_list(fd, check, fr);

		} else if (!strcmp (buf, "edit")) {
			edit(fd, fr);

		} else if (!strcmp (buf, "ls")) {
//			system("clear");
			check =1;
			print_list(fd, check, fr);

		} else if (isdigit(*buf)) {
			detailInfo(fd, fr, atoi(buf));
			edit(fd,fr);
		} else if ( !strcmp(buf, "add")) {
			add(fd, fr);
		}

		
	}
}






void print_list(int fd, int page, FRIEND_T fr)
{

	char bline[1024];
	printf(" [Num]\t[Name]\t[Age]\t[Address]\t[Phone]\n");

	for(int i = 15*(page-1) ; i < 15*page ; i++) {
		bzero(bline, sizeof(bline));
		bzero(&fr, sizeof(fr));
		lseek(fd, i*sizeof(fr), SEEK_SET);
		read(fd, &fr, sizeof(fr));

//		lseek(fd, i*sizeof(fr), SEEK_SET);
//		write(fd, &fr, sizeof(fr));

		str_fit(fr.name, bline, sizeof(bline), 9);
//		printf("bline : %s\n", bline);
		char ibuf[10];
		sprintf(ibuf, "%d", fr.age);
		str_fit(ibuf, bline, sizeof(bline), 14);
//		printf("bline : %s\n", bline);
		str_fit(fr.address, bline, sizeof(bline), 30);
//		printf("bline : %s\n", bline);
		str_fit(fr.phone, bline, sizeof(bline), 44);
		str_fit(fr.email, bline, sizeof(bline), 60);
	

		printf(" [%2d] : %s\n",i, bline);

	}


}

int str_fit(char *str, char *bline, int size, int maxc)
{
//	bzero(bline, sizeof(bline));
	strncat(bline, str, size-strlen(bline));

	int rc;

	for(rc = 0; *bline;){
		int c = 0;
		int t = 1;
		if (!(*bline & 128)) {
			c = 1;
		} else if (!(*bline & 64)) {
			c = 1;
		} else if (!(*bline & 32)) {
			c = 1;
			t = 2;
		} else if (!(*bline & 16)) {
			c = 2;
			t = 3;
		} else if (!(*bline & 8)) {
			c = 2;
			t = 4;
		} 
		if ( rc + c >= maxc )
			break;

		rc += c;
		bline += t;
	}
	memset(bline, ' ', maxc- rc);
	*(bline +maxc -rc) =0;
	

	return rc;
}


int readline(int fd, char *buf, int bufsize)
{
	char rc;
	char c;
	int i;
	
	bzero(buf, bufsize);

	for (i=0 ; ; i++)
	{
		rc = read(fd, &c, 1);

		if (rc <= 0) 
			break;
		if (c == '\n')
			break;

		if(i < (bufsize-1))
			*(buf + i) = c;
	}
	return i;
}



void edit(int fd, FRIEND_T fr)
{
	char buf[1024];
	char *ptr;
	char *address, *name, *phone;
	char *ChangeV;


	while(1){
		printf("edit >> \n");
		fflush(stdout);
	
		int re = readline( 0, buf, sizeof(buf));

		if(!strcmp(buf, "quit")) 
			break;
//			print_list(fd, 1, fr);
			

		if(re < sizeof(fr)) {
			bzero(&fr, sizeof(fr));
		}
		printf("readline : %s\n", buf);
	
		ptr = strchr (buf, '=');
	
		int th = atoi(buf);		// 몇 번째 구조체
		printf("th : %d\n", th);
		int parm = *(ptr-1);	// 무엇을 바꿀 것인가 switch문 파라미터
		printf("parm : %c\n", parm);
		ptr++;
	
		ChangeV = ptr;
		printf("ChangeV : %s\n", ChangeV);
	
		switch (parm) {
			case 'a' :
				lseek(fd, th*sizeof(fr), SEEK_SET);
				read(fd, &fr, sizeof(fr));
	
				strcpy(fr.address, ChangeV);
				lseek(fd, th*sizeof(fr), SEEK_SET);
				write(fd, &fr, sizeof(fr));
				break;
				
			case 'p' :
				lseek(fd, th*sizeof(fr), SEEK_SET);
				read(fd, &fr, sizeof(fr));
	
				strcpy(fr.phone, ChangeV);
				lseek(fd, th*sizeof(fr), SEEK_SET);
				write(fd, &fr, sizeof(fr));
				break;
	
			case 'n' :
				lseek(fd, th*sizeof(fr), SEEK_SET);
				read(fd, &fr, sizeof(fr));

				strcpy(fr.name, ChangeV);
				lseek(fd, th*sizeof(fr), SEEK_SET);
				write(fd, &fr, sizeof(fr));
				break;
	
			case 'e' :
				lseek(fd, th*sizeof(fr), SEEK_SET);
				read(fd, &fr, sizeof(fr));
	
				strcpy(fr.email, ChangeV);
				lseek(fd, th*sizeof(fr), SEEK_SET);
				write(fd, &fr, sizeof(fr));
				break;
	
			case 'A' :
				lseek(fd, th*sizeof(fr), SEEK_SET);
				read(fd, &fr, sizeof(fr));
				
				int z = atoi(ChangeV);
				fr.age= z;
				lseek(fd, th*sizeof(fr), SEEK_SET);
				write(fd, &fr, sizeof(fr));
				break;
		}
	}
}
	

void detailInfo(int fd, FRIEND_T fr, int num)
{
	lseek(fd, num*sizeof(fr), SEEK_SET);
	read(fd, &fr, sizeof(fr));
	
	printf(" 이름 : %s\n", fr.name);
	printf(" 나이 : %d\n", fr.age);
	printf(" 주소 : %s\n", fr.address);
	printf(" 이메일 : %s\n", fr.email);
	printf(" 전화번호 : %s\n", fr.phone);
	printf("-- -- -- -- -- -- -- -- --\n");
}



void add(int fd, FRIEND_T fr)
{
	char buf[1024];
	char *ptr;
//	readline(0, buf, sizeof(buf));
	
	int th = lseek(fd, 0, SEEK_END)/sizeof(fr);

	printf("%d's....", th);
	bzero(buf, sizeof(buf));

	lseek(fd, 0, SEEK_SET);
	read(fd, &fr, sizeof(fr));
	

	printf("ADD NAME : ");
	fflush(stdout);
	readline(0, buf, sizeof(buf));
	strcpy(fr.name, buf);

	printf("ADD AGE : ");
	fflush(stdout);
	readline(0, buf, sizeof(buf));
	int i = atoi(buf);
	fr.age = i;

	printf("ADD ADDRESS : ");
	fflush(stdout);
	readline(0, buf, sizeof(buf));
	strcpy(fr.address, buf);

	printf("ADD PHONE NUMBER : ");
	fflush(stdout);
	readline(0, buf, sizeof(buf));
	strcpy(fr.phone, buf);

	printf("ADD E-MAIL : ");
	fflush(stdout);
	readline(0, buf, sizeof(buf));
	strcpy(fr.email, buf);

	lseek(fd, 0, SEEK_SET);
	write(fd, &fr, sizeof(fr));
}