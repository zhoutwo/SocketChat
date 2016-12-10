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

int currentPid;
int mainPid;
int mainPidSet = 0;
int writePid;
int writePidSet = 0;
int currentSocket;

/* function prototype */
char* getAddress(unsigned int cli_addr);
char* getClientUsername(int sock, char* serverUsername);
int isExit(char* input);
void readSocket (int sock, char* addr, char* username, int mainPid);
void writeSocket (int sock, int readPid);

void error(char *msg)
{
  perror(msg);
  exit(1);
}

void term(int signum) {
  close(currentSocket); // Don't really mind if this is closed more than once
  if (writePidSet) {
    kill(writePid, SIGTERM);
  }
  if (mainPidSet) {
    kill(mainPid, SIGTERM);
  }
  exit(0);
}

int main(int argc, char *argv[])
{
  int sockfd, portno, clilen, pid, running;
  struct sockaddr_in serv_addr, cli_addr;
  char* serverUsername[32];
  char* clientUsername;
  struct sigaction action;
  memset(&action, 0, sizeof(struct sigaction));
  action.sa_handler = term;
  sigaction(SIGTERM, &action, NULL);

  if (argc < 2) {
    fprintf(stderr,"ERROR, no port provided\n");
    exit(1);
  }

  printf("%s", "Provide user name: ");
  fgets(serverUsername, 32, stdin);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  currentSocket = sockfd;
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

  currentPid = getpid();

  while (1) {
    int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
      error("ERROR on accept");
    char* clientAddress = getAddress(cli_addr.sin_addr.s_addr);
    pid = fork();
    if (pid < 0)
       error("ERROR on fork");
    if ((pid == 0) & (pid != currentPid)) {
      clientUsername = getClientUsername(newsockfd, serverUsername);
      readSocket(newsockfd, clientAddress, clientUsername, currentPid);
    }
  } /* end of while */
  kill(pid, SIGTERM);
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
  int r;
  r = strcmp(input, (char *) "exit\n");
  return (r == 0);
}

void killProcess(int sock, int mainPid, int readPid, int writePid) {
  close(sock);
}

void readSocket (int sock, char* addr, char* username, int mainPid)
{
  int n, pid;
  char buffer[256];
  char input[256];

  currentPid = getpid();
  currentSocket = sock;
  mainPid = mainPid;
  mainPidSet = 1;
  printf("Connection established with %s (%s)\n", addr, username);
    
  memset(buffer, 0, 256);
  memset(input, 0, 256);
  pid = fork();
  writePid = pid;
  writePidSet = 1;
  if (pid < 0)
     error("ERROR on fork");
  if ((pid == 0) & (pid != currentPid)) {
    writeSocket(sock, currentPid);
  } else {
    while (1) {
      n = read(sock, buffer, 255);
      if (n < 0) error("ERROR reading from socket");
      if (isExit(buffer)) {
        write(sock, "Closing connection requested by client", 38);
        break;
      }
      printf("<%s>%s\n", username, buffer);
    }
    printf("%s", "Closing connection ...");
  }
  kill(currentPid, SIGTERM);
}

void writeSocket (int sock, int readPid) {
  int n;
  char buffer[256];
  memset(buffer, 0, 256);

  mainPidSet = 0;
  writePidSet = 0;

  while (1) {
    fgets(buffer, 256, stdin);
    if (isExit(buffer)) {
      write(sock, "exit\n", 5);
      write(sock, "Closing connection requested by server", 38);
      break;
    }
    n = write(sock, buffer, sizeof buffer);
    if (n < 0) error("ERROR writing to socket");
    printf("<you>%s", buffer);
  }
  printf("%s", "Closing connection ...");
  kill(readPid, SIGTERM);
}
