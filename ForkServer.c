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

/* function prototype */
char* getAddress(unsigned int cli_addr);
char* getClientUsername(int sock, char* serverUsername);
int isExit(char* input);
void readSocket (int sock, char* addr, char* username, int* isRunning);
void writeSocket (int sock, int* isRunning);

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
  int sockfd, portno, clilen, pid, parentPid, running;
  struct sockaddr_in serv_addr, cli_addr;
  char* serverUsername[32];
  char* clientUsername;

  running = 1;
  int* isRunning = &running;

  if (argc < 2) {
    fprintf(stderr,"ERROR, no port provided\n");
    exit(1);
  }

  printf("%s", "Provide user name: ");
  fgets(serverUsername, 32, stdin);

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

  printf("%s", "Waiting for connection ...\n");

  clilen = sizeof(cli_addr);

  parentPid = getpid();

  while (*isRunning) {
    int newsockfd = accept(sockfd, 
         (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) 
      error("ERROR on accept");
    char* clientAddress = getAddress(cli_addr.sin_addr.s_addr);

    pid = fork();
    if (pid < 0)
       error("ERROR on fork");
    if ((pid == 0) & (pid != parentPid))  {
       // close(sockfd);
      clientUsername = getClientUsername(newsockfd, serverUsername);
      readSocket(newsockfd, clientAddress, clientUsername, isRunning);
      exit(0);
    }
  } /* end of while */
  exit(0);
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

char* getClientUsername(int sock, char* serverUsername) {
  int n;
  char buffer[256];
  memset(buffer, 0, 256);
  n = read(sock, buffer, 255);
  if (n < 0) error("ERROR reading from socket");
  n = write(sock, serverUsername, sizeof serverUsername);
  if (n < 0) error("ERROR writing to socket");
  return buffer;
}

int isExit(char* input) {
  return strcmp(input, (char *) 'exit');
}

/******** DOSTUFF() *********************
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void readSocket (int sock, char* addr, char* username, int* isRunning)
{
  int n, pid, currentPid;
  char buffer[256];
  char input[256];

  currentPid = getpid();

  printf("Connection established with %s (%s)\n", addr, username);
    
  memset(buffer, 0, 256);
  memset(input, 0, 256);

  pid = fork();
  if (pid < 0)
     error("ERROR on fork");
  if ((pid == 0) & (pid != currentPid))  {
    writeSocket(sock, isRunning);
  } else {
    while (*isRunning) {
      n = read(sock,buffer,255);
      if (n < 0) error("ERROR reading from socket");
      if (isExit(buffer)) {
        write(sock,"Closing connection requested by client",38);
        *isRunning = 0;
        break;
      }
      printf("<%s>%s\n",username,buffer);
    }
    close(sock);
    printf("%s", "Closing connection ...");
  }
  exit(0);
}

void writeSocket (int sock, int* isRunning) {
  int n;

  char buffer[256];
  memset(buffer, 0, 256);
  while (*isRunning) {
    fgets(buffer, 256, stdin);
    if (isExit(buffer)) {
        write(sock,"Closing connection requested by server",38);
        *isRunning = 0;
        break;
    }
    n = write(sock, buffer, sizeof buffer);
    if (n < 0) error("ERROR writing to socket");
    printf("<you>%s", buffer);
  }
}
