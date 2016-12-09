#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

int main(int argc, char *argv[])
{
	int sockfd, new_fd;
	struct addrinfo hints, *servinfo, *p;
	///////////////////////////////////////
	char name[256];
	gethostname(name, sizeof(name));
	struct hostent *local = gethostbyname(name);
	char *address = local->h_addr;

	char ip[256];
	inet_ntop(AF_INET, address, ip, sizeof(ip));
	printf("%s\n", ip);
	
	///////////////////////////////////////
	
	
	
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(argv[1], NULL, &hints, &servinfo);
	struct sockaddr_in *ipv4 = (struct sockaddr_in *)servinfo->ai_addr;
	void *addr = &(ipv4->sin_addr);
	char ipstr[256];
	inet_ntop(servinfo->ai_family, addr, ipstr, sizeof(ipstr));
	printf("%s\n", ipstr);
} 