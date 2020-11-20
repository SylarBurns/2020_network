#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#define BUFSIZE 256
void error_handling (char *message);
int main (int argc, char **argv)
{
  int sock;
  char message[BUFSIZE] = "";
  int str_len, addr_size, i;
  char f_name[BUFSIZE];
  char cbuf[BUFSIZE];
  struct sockaddr_in serv_addr;
  struct sockaddr_in from_addr;
  struct sockaddr_in my_addr;
  if (argc != 4)
    {
      printf ("Usage : %s <IP> <port> <file_name>\n", argv[0]);
      exit (1);
    }
  //socket
  sock = socket (PF_INET, SOCK_STREAM, 0);
  if (sock == -1)
    error_handling ("TCP 소켓 생성 오류");
  
  //connect
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr (argv[1]);
  serv_addr.sin_port = htons (atoi (argv[2]));
  if (connect (sock, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) == -1)
	error_handling("connect error!");

  //file name 저장
  char msg[256];
  sprintf(msg,"%s=",argv[3]);
  send(sock,msg,strlen(msg), 0);
  printf("file_name : %s \n",argv[3]);

  //파일 내용
   FILE* fp =fopen(argv[3],"rb");
   if(!fp)
   	error_handling("file open error");

  while(fgets(message,BUFSIZE,fp)!=NULL){
  	send(sock,message,strlen(message),0);
  //memset(&message,0x00, sizeof (message));
  }
  printf("end!\n");
  close(sock);
  return 0;
}

void error_handling (char *message)
{
  fputs (message, stderr);
  fputc ('\n', stderr);
  exit (1);
 }
