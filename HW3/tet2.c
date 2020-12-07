#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#define BUFSIZE 1024
void error_handling (char *message);
void send_html (char *fname, int clnt_sock);
void GET_proto (char *message, int clnt_sock);
void POST_proto (char *msg, int clnt_sock);

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
  serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
  serv_addr.sin_port = htons (atoi (argv[1]));
  if (bind (serv_sock, (struct sockaddr *) &serv_addr,
	    sizeof (serv_addr)) == -1)
    error_handling ("bind() error");

  //listen
  if (listen (serv_sock, 5) == -1)
    error_handling ("listen() error");

  clnt_addr_size = sizeof (clnt_addr);
  while (1)
    {
      //accept
      clnt_sock = accept (serv_sock, (struct sockaddr *) &clnt_addr, &clnt_addr_size);
      if (clnt_sock == -1)
		error_handling ("accept() error");
      str_len = recv (clnt_sock, message, BUFSIZE - 1, 0);
    if(message[0]=='G' && message[1]=='E' && message[2]=='T' && message[3]==' ')
	{
	  printf ("GET request arrived\n");
	  GET_proto (message, clnt_sock);
	}
	else if(message[0]=='P' && message[1]=='O' && message[2]=='S' && message[3]=='T' && message[4]==' ')
	{
	  printf ("POST request arrived\n");
	  POST_proto (message, clnt_sock);
	}
	memset (&message, 0, sizeof (message));
    }

  close (serv_sock);
  return 0;
}

void GET_proto (char *message, int clnt_sock)
{
  char request_fname[100];
  int m_index, r_index = 0;
  m_index = 4;
  while (message[m_index] != ' ')
    {
      request_fname[r_index] = message[m_index];
      r_index++;
      m_index++;
    }
  request_fname[r_index] = '\0';
  if (strcmp (request_fname, "/") == 0)
    {
      send_html ("index.html", clnt_sock);
    }
  else if (send_html (request_fname, clnt_sock) == -1)
    {
      char *header = "HTTP/1.1 404 Not Found\n" "\n";
      if (send (clnt_sock, header, strlen (header), 0) < 1)
	  error_handling ("header send error");
    }
}

int send_html (char *fname, int clnt_sock)
{
  //make header and send
  char *header = "HTTP/1.1 200 OK\n" "Content-type: text/html\n" "\n";
  if (send (clnt_sock, header, strlen (header), 0) < 1)
    error_handling ("header send error");
  if (fname[0] == '/')
    {
      fname += 1;
    }
  //read body and send

  char buf[BUFSIZE];
  FILE *fp;
  fp = fopen (fname, "r");

while (fgets (buf, BUFSIZE, fp) != NULL){
	send (clnt_sock, buf, strlen (buf), 0);
}
  fclose (fp);
  close (clnt_sock);
}

void error_handling (char *message)
{
  fputs (message, stderr);
  fputc ('\n', stderr);
  exit (1);
}

void POST_proto (char *msg, int clnt_sock)
{
  unsigned char *header = "HTTP/1.0 200 OK\n" "Content-type: text/html\n" "\n";
  char pp1[1024] = "<h2>";
  char *pp2 = "</h2>";
  char *tmp;
  char *p = strtok (msg, "\n");
  while (p != NULL)
    {
      tmp = p;
      p = strtok (NULL, "\n");
    }
  send (clnt_sock, header, strlen (header), 0);
  strcat (pp1, tmp);
  strcat (pp1, pp2);
  send (clnt_sock, pp1, strlen (pp1), 0);
}