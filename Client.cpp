#include<stdio.h>
#include <iostream>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<assert.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
using namespace std;
int main()
{
    int port;
    cout <<"Which server do you want to connect to ?\n";
    cin >> port;
	int sockfd = socket(AF_INET,SOCK_STREAM,0);	
	assert(sockfd != -1 );

	//Set Address Information
	struct sockaddr_in saddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//Link to server
	int res = connect(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
	assert(res != -1);
    
	while(1)
	{
		char buff[128] = {0};
		printf("Please Input:");
		fgets(buff,128,stdin);
		if(strncmp(buff,"end",3) ==0 )
		{
			break;
		}
		send(sockfd,buff,strlen(buff),0);
		memset(buff,0,128);
		recv(sockfd,buff,127,0);
		printf("RecvBuff:%s\n",buff);
        printf("\n");
	}
	close(sockfd);
}