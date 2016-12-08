/* A simple server in the internet domain using TCP
   The port number is passed as an argument 
   This version runs forever, forking off a separate 
   process for each connection
   gcc server2.c 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void dostuff(int);
char* getAddress(unsigned int cli_addr); /* function prototype */

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(stderr,"ERROR, no port provided\n");
    exit(1);
  }
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) 
    error("ERROR opening socket");
  memset(&serv_addr, 0, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr *) &serv_addr,
    sizeof(serv_addr)) < 0) 
    error("ERROR on binding");
  listen(sockfd,5);
  clilen = sizeof(cli_addr);
  while (1) {
    newsockfd = accept(sockfd, 
         (struct sockaddr *) &cli_addr, &clilen);
    printf("%s", getAddress(cli_addr.sin_addr.s_addr));
    if (newsockfd < 0) 
       error("ERROR on accept");
    pid = fork();
    if (pid < 0)
       error("ERROR on fork");
    if (pid == 0)  {
       close(sockfd);
       dostuff(newsockfd);
       exit(0);
    }
    else close(newsockfd);
  } /* end of while */
  return 0; /* we never get here */
}

/*
  This function returns the pointer to the string representation of an IP address encoded in an unsigned integer
*/
char* getAddress(unsigned int addr) {
  char* result[15];
  unsigned int a0 = addr & 0xFF;
  unsigned int a1 = (addr >> 8) & 0xFF;
  unsigned int a2 = (addr >> 16) & 0xFF;
  unsigned int a3 = (addr >> 24) & 0xFF;
  snprintf(result, 15, "%d.%d.%d.%d", a0, a1, a2, a3);
  return result;
}

/******** DOSTUFF() *********************
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void dostuff (int sock)
{
   int n;
   char buffer[256];
      
   memset(buffer, 0, 256);
   n = read(sock,buffer,255);
   if (n < 0) error("ERROR reading from socket");
   printf("Here is the message: %s\n",buffer);
   n = write(sock,"I got your message",18);
   if (n < 0) error("ERROR writing to socket");
}
