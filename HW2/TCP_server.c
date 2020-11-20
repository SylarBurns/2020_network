#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#define BUFSIZE 1024
void error_handling (char *message);

int main (int argc, char **argv)
{
  int serv_sock;
  int clnt_sock;
  char message[BUFSIZE];
  int str_len, num = 0;
  struct sockaddr_in serv_addr;
  struct sockaddr_in clnt_addr;
  int clnt_addr_size;
  if (argc != 2)
    {
      printf ("Usage : %s <port>\n", argv[0]);
      exit (1);
    }
//socket 
  serv_sock = socket (PF_INET, SOCK_STREAM, 0);
  if (serv_sock == -1)
    error_handling ("TCP socket error");

//bind
  memset (&serv_addr, 0, sizeof (serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr ("10.1.0.1");
  serv_addr.sin_port = htons (atoi (argv[1]));
  if (bind (serv_sock, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) ==  -1)
    error_handling ("bind() error");

//listen
  if (listen (serv_sock, 5) == -1)
    error_handling ("listen() error");

  clnt_addr_size = sizeof (clnt_addr);
  while (1){
//accept
      clnt_sock =accept (serv_sock, (struct sockaddr *) &clnt_addr, &clnt_addr_size);
      if (clnt_sock == -1)
		error_handling ("accept() error");

      char f_name[BUFSIZE];
      char *p;
      str_len = recv (clnt_sock, message, BUFSIZE-1, 0);

//file name 
      char *ptr = strtok (message, "=");
      strcpy (f_name, ptr);

	  printf("file name: %s\n",f_name);
      int i = 0;
//first content
	  int len = strlen(ptr)+1;
      ptr += len; 
      FILE *fp = fopen (f_name, "wb");
      if (ptr != NULL)
	{
	  fwrite (ptr, sizeof (char), strlen (ptr), fp);
	  printf ("recv num: %d \n", i++);
	}
	  memset (&message, 0x00, sizeof (message));
//other contents
      while ((str_len = recv (clnt_sock, message, BUFSIZE-1, 0) != 0))
	{
	  fwrite (message, sizeof (char), strlen (message), fp);
	  memset (&message, 0x00, sizeof (message));
	  printf("recv num: %d \n",i++);
	}

      fclose (fp);
      printf ("end!\n");
      close (clnt_sock);
    }
  close (serv_sock);
  return 0;

}

void error_handling (char *message)
{
  fputs (message, stderr);
  fputc ('\n', stderr);
  exit (1);
}
