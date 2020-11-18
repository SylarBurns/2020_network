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
	char *fname = argv[3];
	char *ACK_header ="ACK";
	char *end_msg="";
	char *fname_header ="FNAME";
	char ch;
	int str_len, addr_size, i;	
	
	struct sockaddr_in serv_addr;
	struct sockaddr_in from_addr;
	struct sockaddr_in my_addr;
	
	struct timeval TOval = {0,500000};/*500ms*/
	int TOlen = sizeof(TOval);

	FILE* fp;
	if(argc!=4){
		printf("Usage: %s <IP><port><file name>\n",argv[0]);
		exit(1);
	}
	
	my_sock=socket(PF_INET,SOCK_DGRAM, 0);
	if(my_sock==-1)
		error_handling("socket() error");
	/*Socket Option*/
	setsockopt(my_sock, SOL_SOCKET, SO_RCVTIMEO, &TOval, TOlen);
	
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
			continue;/*timeout, resend filename*/
		}else{
			message[str_len]=0;
			if(strcmp(fname, get_body(message, ACK_header))==0){
				printf("File name delivered: %s\n",get_body(message, ACK_header));
				break;/*file name transfer success*/
			}else{
				continue;/*resend filename*/
			}
		}
	}
	sleep(3);
	/*Start sending file*/
	if((fp = fopen(fname,"rb"))==NULL){
		error_handling("file read error");
	}
	i=0;
	while(!feof(fp)){
		str_len = fread(message, 1, BUFSIZE,fp);
		sendto(my_sock,message,str_len,0,
			(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	i++;
	}
	sleep(5);
	sendto(my_sock,end_msg,strlen(end_msg),0,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	while(1){
		str_len = recvfrom(my_sock, message, BUFSIZE,0,
			(struct sockaddr*)&from_addr, &addr_size);
		if(str_len==-1){
			sendto(my_sock,end_msg,strlen(end_msg),0,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
		}else{
			message[str_len]=0;
			if(strcmp("END",message)==0){
				break;/*END successfully received*/
			}else{
				continue;/*resend ""\0 when timeout*/
			}
		}
	}
	sprintf(message, "END");
	sendto(my_sock, message, strlen(message),0,
		(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	i = 0;
	while(i<10){
		str_len = recvfrom(my_sock, message, BUFSIZE,0,
			(struct sockaddr*)&from_addr, &addr_size);
		if(str_len == -1){
			i++;
		}else{
			sendto(my_sock, message, strlen(message),0,
				(struct sockaddr*)&serv_addr,sizeof(serv_addr));
			i=0;
		}
	}
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
