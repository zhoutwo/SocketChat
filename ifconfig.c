  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <sys/types.h> 
  #include <sys/socket.h>
  #include <netinet/in.h>


   int main(int argc, char *argv[])
   {
//	system("ifconfig");
	FILE *fp;
	char buffer[80];
	fp=popen("ifconfig","r");
//	fgets(buffer,sizeof(buffer),fp);
//	printf("%s",buffer);
	char lines[256][256];
	int i = 0;
	fgets(buffer, sizeof buffer, fp);
	fgets(buffer, sizeof buffer, fp);
	
	 
	printf("%s", buffer);
	pclose(fp);

   }
