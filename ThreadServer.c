// gcc tcpserver.c -o server -pthread
// ./server

  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <sys/types.h> 
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <pthread.h>

  #define PORT 62828
  #define BUF_SIZE 256
  #define CLADDR_LEN 100

    void error(char *msg)
    {
        perror(msg);
        exit(1);
    }

    void * receiveMessage(void * socket) {
        int sockfd, ret;
        char buffer[BUF_SIZE];
        sockfd = (int) socket;

        memset(buffer, 0, BUF_SIZE);
        if (write(sockfd,"I'm waiting for message",23) < 0)
            error("ERROR writing to socket");

        while ((ret = read(sockfd, buffer, BUF_SIZE)) > 0) {
            printf("client: %s", buffer);
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
        int sockfd, newsockfd, portno, clilen;
        char buffer[BUF_SIZE];
        struct sockaddr_in serv_addr, cli_addr;
        pid_t childpid;
        char clientAddr[CLADDR_LEN];
        pthread_t rThread;
        int ret, n;

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) 
           error("ERROR opening socket");
        printf("Socket created...\n");

        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);
        serv_addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
            error("ERROR on binding");
        printf("Binding done...\n");

        printf("Waiting for a connection...\n");
        listen(sockfd,5);
        clilen = sizeof(cli_addr);
        while (newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) {
            inet_ntop(AF_INET, &(cli_addr.sin_addr), clientAddr, CLADDR_LEN);
            printf("Connection accepted from %s...\n", clientAddr);

            //creating a new thread for receiving messages from the client
            if (ret = pthread_create(&rThread, NULL, receiveMessage, (void *) newsockfd)) {
                printf("ERROR: Return Code from pthread_create() is %d\n", ret);
                error("ERROR creating thread");
            }
        }

        if (newsockfd < 0) 
            error("ERROR on accept");
//        close(newsockfd);
        close(sockfd);

        pthread_exit(NULL);
        return 0; 
    }
