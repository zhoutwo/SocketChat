/**
 * @file    sockname.c
 * @brief   
 */
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
 
#include <stdio.h>
#include <string.h>
 
int main(int argc, char *argv[])
{
    struct sockaddr_in lisnaddr, clntaddr, addr;
    socklen_t clntlen, len;
    int lisnfd, clntfd;
    char *ptr;
    int optval;
 
    memset(&lisnaddr, 0, sizeof(lisnaddr));
    lisnaddr.sin_family = AF_INET;
    lisnaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    lisnaddr.sin_port = htons(8888);
 
    lisnfd = socket(AF_INET, SOCK_STREAM, 0);
    optval = 1;
    setsockopt(lisnfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    bind(lisnfd, (struct sockaddr *)&lisnaddr, sizeof(lisnaddr));
    listen(lisnfd, 5);
 
    clntlen = sizeof(clntaddr);
    clntfd = accept(lisnfd, (struct sockaddr *)&clntaddr, &clntlen);
 
 
    len = sizeof(addr);
    getsockname(clntfd, (struct sockaddr *)&addr, &len);
    ptr = inet_ntoa(addr.sin_addr);
    printf("%s:%d <==> ", ptr, htons(addr.sin_port));
 
    ptr = inet_ntoa(clntaddr.sin_addr);
    printf("%s:%d\n", ptr, htons(clntaddr.sin_port));
 
    return 0;
}
