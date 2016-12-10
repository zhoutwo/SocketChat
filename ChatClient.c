#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
#include <pthread.h>

#define SERVADDR_LEN 100
#define BUF_SIZE 256

void error(char *msg)
{
    perror(msg);
    exit(0);
}
void * receiveMessage(void * socket) {
    int sockfd, ret;
	int k = 0;
    char buffer[BUF_SIZE];
    sockfd = (int) socket;
	char server_name[BUF_SIZE];
	
    memset(buffer, 0, BUF_SIZE);
//    if (write(sockfd,"I'm waiting for message",23) < 0)
//        error("ERROR writing to socket");

    while ((ret = read(sockfd, buffer, BUF_SIZE)) > 0) {
		if(k == 0){
			strcpy(server_name, buffer);
			k = 1;
			printf("(%s)\n", server_name);
		}
		else	
			printf("%s: %s", server_name, buffer);
    }
    if (ret < 0) 
        printf("Error receiving data!\n");
    else
        printf("Closing connection\n");
    close(sockfd);
	exit(0);
}




int main(int argc, char *argv[])
{
    int sockfd, portno, n, ret;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[BUF_SIZE];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
	//////////////////////////////////////////
	printf("Provide user name: ");
	char name[BUF_SIZE];
	scanf("%s", name);
	/*char host[256];  
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
	*/
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
	char serverAddr[SERVADDR_LEN];
	inet_ntop(AF_INET, &(serv_addr.sin_addr), serverAddr, SERVADDR_LEN);
	printf("Connection accepted from %s...", serverAddr);
	//--------------完成连接---------------//
	/*char temp[] = "Connection established with ";
	strcat(temp, ip);
	strcat(temp, "(");
	strcat(temp, name);
	strcat(temp, ")");*/
//	printf("Connection established with %s (%s)\n",ip, name);
	write(sockfd, name, strlen(name));
	
	
//    printf("Please enter the message: ");
	pthread_t rThread;
	if (ret = pthread_create(&rThread, NULL, receiveMessage, (void *) sockfd)) {
        printf("ERROR: Return Code from pthread_create() is %d\n", ret);
        error("ERROR creating thread");
    }
    bzero(buffer,BUF_SIZE);
	while(1)
	{
		bzero(buffer,BUF_SIZE);
		fgets(buffer,BUF_SIZE,stdin);
		n = write(sockfd,buffer,strlen(buffer));
		if (n < 0) 
        error("ERROR writing to socket");
		char EXIT[BUF_SIZE];
		bzero(EXIT, BUF_SIZE);
		strcpy(EXIT, "exit\n");
		
//		printf("EXIT: %s : %s", EXIT, buffer);
//		printf("%d\n", strcoll(EXIT, buffer));
		if(strcoll(EXIT, buffer) == 0) break;
		bzero(buffer,BUF_SIZE);
		printf("bzero: %s", buffer);
	}
	printf("Closing connection\n");
//    if (newsockfd < 0) 
//        error("ERROR on accept");
//        close(newsockfd);
    close(sockfd);
//    n = read(sockfd,buffer,255);
//    if (n < 0) 
//        error("ERROR reading from socket");
//    printf("%s\n",buffer);
    return 0;
}

