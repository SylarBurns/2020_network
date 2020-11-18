/*UDP Server*/
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
	int serv_sock;
	char message[BUFSIZE];
	char *file_header = "FNAME";
	char *fname;
	char ACK[BUFSIZE]="ACK";
	int str_len, i, num=0;
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	int clnt_addr_size;
	
	struct timeval TOval = {1,0};
	int TOlen = sizeof(TOval);
	
	struct timeval TOinit = {0,0};
	int TOinitlen = sizeof(TOinit);

	FILE* fp;
	if(argc!=2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_DGRAM, 0);
	if(serv_sock == -1)
		error_handling("UDP socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(atoi(argv[1]));
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
		error_handling("bind() error");
	while(1){
		/*recv filename*/
		while(1){
		clnt_addr_size =sizeof(clnt_addr);
		str_len = recvfrom(serv_sock,message, BUFSIZE,0,
				  (struct sockaddr*)&clnt_addr, &clnt_addr_size);
		message[str_len]=0;
			if(strncmp(message, file_header, strlen(file_header))==0){
				fname = get_body(message, file_header);
				strcpy(ACK+3,fname);
				sendto(serv_sock,ACK,strlen(ACK),0,
					(struct sockaddr*)&clnt_addr, sizeof(clnt_addr));
			break;
			}
		}
		printf("filename: %s\n",fname);
		
		if((fp=fopen(fname,"wb"))==NULL){
			error_handling("file open error");
		}/**/
		/*recv late arriving fname*/	
		while(1){
		clnt_addr_size =sizeof(clnt_addr);
		str_len = recvfrom(serv_sock,message, BUFSIZE,0,
				  (struct sockaddr*)&clnt_addr, &clnt_addr_size);
			if(strncmp(message,file_header,strlen(file_header))==0){
				printf("Reset FP\n");
				fname = get_body(message, file_header);
				strcpy(ACK+3,fname);
				sendto(serv_sock,ACK,strlen(ACK),0,
					(struct sockaddr*)&clnt_addr, sizeof(clnt_addr));
				fclose(fp);
				if((fp=fopen(fname,"wb"))==NULL){
					error_handling("file open error");
				}/*reopen file pointer*/
			}else{
				fwrite(message, sizeof(char), str_len, fp);
				break;
			}
		}
		i=0;
		/*recv content*/
		while(1){
		clnt_addr_size =sizeof(clnt_addr);
		str_len = recvfrom(serv_sock,message, BUFSIZE,0,
				  (struct sockaddr*)&clnt_addr, &clnt_addr_size);
			if(str_len==0){
				break;
			}else{
			fwrite(message, sizeof(char), str_len, fp);
			}
		i++;
		}
		printf("File received: %d * %d\n", i,BUFSIZE);
		setsockopt(serv_sock, SOL_SOCKET,SO_RCVTIMEO,&TOval,TOlen);
		i=0;
		while(1){
			sprintf(message,"END");
			sendto(serv_sock,message,strlen(message),0,
				(struct sockaddr*)&clnt_addr, sizeof(clnt_addr));
			str_len = recvfrom(serv_sock,message, BUFSIZE,0,
				  (struct sockaddr*)&clnt_addr, &clnt_addr_size);
			message[str_len]=0;
			if(str_len == -1){
				continue;//resend END
			}else if(strcmp(message,"END")==0){
				break;
			}else{
				i++;
				continue;
			}
		}
		/*Check file received*/
		sprintf(message,"wc %s", fname);
		system(message);
		/*Reset Socket option*/
		setsockopt(serv_sock, SOL_SOCKET,SO_RCVTIMEO,&TOinit,TOinitlen);
		free(fname);
	}
	return 0;
}

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
	fputc('\n',stderr);
	exit(1);
}
