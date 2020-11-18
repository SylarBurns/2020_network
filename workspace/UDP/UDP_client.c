/*UDP Client*/
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>
#define BUFSIZE 1024
void error_handling(char *message);
char* get_body(char* message, char* header);
int main(int argc, char **argv)
{
	int my_sock;
	char message[BUFSIZE];
	char fname[BUFSIZE] = "example.bin";
	char *ACK_header ="ACK";
	char *fname_header ="FNAME";
	char *content_header ="CONT";
	char *id ="3131QQ";
	int str_len, addr_size;	
	
	struct sockaddr_in serv_addr;
	struct sockaddr_in from_addr;
	struct sockaddr_in my_addr;
	
	struct timeval TOval = {0,500000};/*500ms*/
	int TOlen = sizeof(TOval);

	if(argc!=3){
		printf("Usage: %s <IP><port>\n",argv[0]);
		exit(1);
	}
	
	my_sock=socket(PF_INET,SOCK_DGRAM, 0);
	if(my_sock==-1)
		error_handling("socket() error");
	/*Socket Option*/
	setsockopt(my_sock, SOL_SOCKET, SO_RCVTIMEO, &TOval, TOlen);
	/*set IP address and bind*/
	/*memset(&my_addr,0,sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port=htons(0);
	my_addr.sin_addr.s_addr = inet_addr("10.1.0.2");
	if(bind(my_sock, (struct sockaddr*)&my_addr, sizeof(my_addr))==-1)
		error_handling("bind() error");*/
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));
	
	while(1){
		memset(message,0,sizeof(message));
		sprintf(message, "%s%s", fname_header, fname);
		sendto(my_sock, message, strlen(message),0,
			(struct sockaddr*)&serv_addr,sizeof(serv_addr));
		str_len = recvfrom(my_sock, message, BUFSIZE,0,
			(struct sockaddr*)&from_addr, &addr_size);
		if(str_len==-1){
			printf("Timeout\n");
			continue;/*timeout, resend filename*/
		}else{
			message[str_len]=0;
			if(strcmp(fname, get_body(message, ACK_header))==0){
				break;/*file name transfer success*/
			}else{
				printf("file name damaged: %s\n",message);
				continue;/*resend filename*/
			}
		}
	}

	while(1){	
		memset(message,0,BUFSIZE);
		sprintf(message, "%s%s", content_header, id);
		sendto(my_sock, message, strlen(message),0,
			(struct sockaddr*)&serv_addr,sizeof(serv_addr));
		str_len = recvfrom(my_sock, message, BUFSIZE,0,
			(struct sockaddr*)&from_addr, &addr_size);
		if(str_len==-1){
			printf("Timeout\n");
			continue;/*timeout, resend filename*/
		}else{
			message[str_len]=0;
			if(strcmp(id, get_body(message, ACK_header))==0){
				break;/*content header and id transfer success*/
			}else{
				printf("content and id damaged: %s\n",message);
				continue;/*resend filename*/
			}
		}
	}
	printf("server ip: %s",inet_ntoa(from_addr.sin_addr));
	printf("Handshaking success\n");
	close(my_sock);
	return 0;
}
/*get message&header, return body*/
char* get_body(char* message, char* header){
	int header_size = strlen(header);
	int body_size = strlen(message)-header_size;
	char *body = (char*)malloc(sizeof(char)*(body_size+1));
	strncpy(body,message+header_size, body_size);
	body[body_size+1]=0;
	return body;
}
void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
