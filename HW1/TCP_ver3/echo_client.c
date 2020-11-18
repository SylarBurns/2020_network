#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>
#define BUFSIZE 30

void error_handling(char *message);

int main(int argc, char **argv)
{
	int sock;
	char message[BUFSIZE];
	int str_len;
	struct sockaddr_in serv_addr;
	
	int i=0;
	char MSG1[]="";
	char MSG2[]="Good ";
	char MSG3[]="Evening ";
	char MSG4[]="Everybody!";
	if(argc!=3){
		printf("Usage: %s <IP><port>\n",argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET,SOCK_STREAM, 0);
	if(sock==-1)
		error_handling("socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));

	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error!");
	
	write(sock,MSG1,strlen(MSG1));
	//write(sock,MSG2,strlen(MSG2));
	//write(sock,MSG3,strlen(MSG3));
	//write(sock,MSG4,strlen(MSG4));
	for(i=0;i<4;i++){
		str_len = read(sock,message,BUFSIZE-1);
		message[str_len]=0;
		printf("No %d message from the server: %s\n",i,message);
	}

	close(sock);
	return 0;
}
void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
