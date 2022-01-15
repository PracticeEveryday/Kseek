#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#define PORT 8001
#define MAXPENDDING 5
#define MAX 1024

int readline(int fd, char *buf, int bufsize);

int main()
{
	int servSockfd, clntSockfd;
	struct sockaddr_in servAddr,clntAddr;
	char recvBuffer[MAX], sendBuffer[MAX];
	int clntLen, recvLen;


	if((servSockfd=socket(AF_INET, SOCK_STREAM, 0))== -1){
		perror("Sock failed");
		exit(1);
	}

	memset(&servAddr, 0, sizeof(servAddr));

	servAddr.sin_family= AF_INET;
	servAddr.sin_addr.s_addr= htonl(INADDR_ANY);
	servAddr.sin_port = htons(PORT);

	if(bind(servSockfd, (struct sockaddr*)&servAddr, sizeof(servAddr))==-1){
		perror("bind failed");
		exit(1);
	}

	if(listen(servSockfd, MAXPENDDING)==-1){
		perror("listen failed");
		exit(1);
	}

	printf("Server Daemon Start\n");

	while(1){
		clntLen = sizeof(clntAddr);

		if((clntSockfd=accept(servSockfd, (struct sockaddr*)&clntAddr, &clntLen))==-1){
			perror("accept failed");
			exit(1);
		}

		printf(" [%d] Client is in\n",clntSockfd);

		switch(fork()){
			case -1:
				perror("fork failed");
				exit(1);

			case 0:
				close(servSockfd);
				while(1){
					if((recvLen=recv(clntSockfd, recvBuffer, MAX, 0))==-1){
						perror("recv failed");
						exit(1);
					}
					if(recvLen==0)
						break;

					recvBuffer[recvLen]='\0';

					printf("[%d]", clntSockfd);
					printf("Client >>> %s", recvBuffer);
				}
				close(clntSockfd);
				exit(0);

			default:
				while(1){
					//fgets(sendBuffer, MAX, stdin);
					readline(0,sendBuffer,sizeof(sendBuffer));

					if(send(clntSockfd, sendBuffer, strlen(sendBuffer), MSG_NOSIGNAL)
							!= strlen(sendBuffer)){
						if(errno==EPIPE){
							printf("Client is out\n");
							break;
						}

						perror("send failed");
						exit(1);
					}
				}
		}
		close(clntSockfd);
	}
}

int readline(int fd, char *buf, int bufsize)
{
	char c;
	int i,rc;

	bzero(buf,bufsize);

	for(i=0;;i++){
		rc=read(fd, &c, 1);
			if(rc<=0 || c=='\n')
				break;
		if(i<(bufsize-1))
			*(buf+i)=c;
	}
	return i;
}