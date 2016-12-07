  #include <stdio.h>
  #include <sys/types.h> 
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <netdb.h> 
  
  void error(char *msg)
  {
      perror(msg);
      exit(1);
  }

   int main(int argc, char *argv[])
   {
        int sockfd, newsockfd, portno, clilen;
        char buffer[256];
        struct sockaddr_in serv_addr, cli_addr;
        int n;
        if (argc < 2) {
            fprintf(stderr,"ERROR, no port provided\n");
            exit(1);
        }
		printf("Provide user name: ");
		char name[256];
		scanf("%s", name);
//		printf("Your name: %s\n", name);
		
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) 
           error("ERROR opening socket");
        bzero((char *) &serv_addr, sizeof(serv_addr));
		//////////////////////////////////////////////
		char host[256];  
		if(gethostname(host,sizeof(host)) < 0)  
		{  
			printf("无法获取主机名\n");  
		}  
		else  
		{  
			printf("本机计算机名为: %s\n", host);
		} 
		struct hostent *server;
		server = gethostbyname(host);
		if (server == NULL) {
			printf(stderr,"ERROR, no such host\n");
			exit(1);
		}
		/*
		bcopy((char *)server->h_addr, 
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);*/
/*		int i = 0;
		for(i = 0; server -> h_addr_list[i]!=0;i++){
			struct in_addr in;  
			memcpy(&in,server->h_addr_list[i],sizeof(struct in_addr));  
			printf("第%d块网卡的IP为:%I32u\n",i+1,in.s_addr); 
		}*/
		//另一种找的ip方式
		/*
		struct in_addr in;
		memcpy(&in,server->h_addr_list[0],sizeof(struct in_addr)); 
		uint32_t address = in.s_addr;
		printf("%d.%d.%d.%d\n",address&0x000000ff,(address&0x0000ff00)>>8,(address&0x00ff0000)>>16,(address&0xff000000)>>24);
		*/
		char ip[256];
		inet_ntop(AF_INET, server->h_addr, ip, sizeof(ip));
		printf("本机ip: %s\n", ip);
		//////////////////////////////////////////////
        portno = atoi(argv[1]);
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(portno);
//        serv_addr.sin_addr.s_addr = INADDR_ANY;
        if (bind(sockfd, (struct sockaddr *) &serv_addr,
                 sizeof(serv_addr)) < 0) 
                 error("ERROR on binding");
        listen(sockfd,5);
//		printf("%I32u\n", serv_addr.sin_addr.s_addr);
		
		printf("Waiting for connection...\n");
		
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
            error("ERROR on accept");
		//--------------完成连接---------------//
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);
        if (n < 0) error("ERROR reading from socket");
        printf("Here is the message: %s\n",buffer);
        n = write(newsockfd,"I got your message",18);
        if (n < 0) error("ERROR writing to socket");
        return 0; 
   }


