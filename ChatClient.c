#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>

   void error(char *msg)
   {
       perror(msg);
       exit(0);
   }

   int main(int argc, char *argv[])
   {
     int sockfd, portno, n;
     struct sockaddr_in serv_addr;
     struct hostent *server;
     char buffer[256];
     if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
     }
	 //////////////////////////////////////////
	printf("Provide user name: ");
	char name[256];
	scanf("%s", name);
	char host[256];  
	if(gethostname(host,sizeof(host)) < 0)  
	{  
		printf("无法获取主机名\n");  
	}  
	else  
	{  
		printf("本机计算机名为: %s\n", host);
	} 
	struct hostent *local;
	local = gethostbyname(host);
	if (local == NULL) {
		printf(stderr,"ERROR, no such host\n");
		exit(1);
	}
	char ip[256];
	inet_ntop(AF_INET, local->h_addr, ip, sizeof(ip));
	printf("本机ip: %s\n", ip);
	
	 //////////////////////////////////////////
     portno = atoi(argv[2]);
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
         error("ERROR opening socket");
     server = gethostbyname(argv[1]);
     if (server == NULL) {
         fprintf(stderr,"ERROR, no such host\n");
         exit(0);
     }
	 ///////////////////////////////////////////////
     bzero((char *) &serv_addr, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     bcopy((char *)server->h_addr, 
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
     serv_addr.sin_port = htons(portno);
	printf("Waiting for connection...\n");
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
	
	//--------------完成连接---------------//
	char temp[] = "Connection established with ";
	strcat(temp, ip);
	strcat(temp, "(");
	strcat(temp, name);
	strcat(temp, ")");
//	printf("Connection established with %s (%s)\n",ip, name);
	write(sockfd, temp, strlen(temp));
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
        error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
        error("ERROR reading from socket");
    printf("%s\n",buffer);
    return 0;
   }

