#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<assert.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
using namespace std;

int sockfd;
pthread_t listener_thread;


/*
Listens to messages from the router it is connected to
*/
void* messageListener (void* Args){
	char buff[128] = {0};
	do{
		recv(sockfd,buff,127,0);
		printf("Received a message: %s\n",buff);
		printf("\n");
	} while(buff!="close");	
}

int main()
{
    int port;
    cout <<"Which server do you want to connect to ?\n";
    cin >> port;
	sockfd = socket(AF_INET,SOCK_STREAM,0);	
	assert(sockfd != -1 );

	//Set Address Information
	struct sockaddr_in saddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//Link to router
	int res = connect(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
	assert(res != -1);
    
	pthread_create (&listener_thread, NULL, messageListener, (void*)NULL); 


	while(1)
	{
        cout << "Clients Available : \n";
        cout << "Which Client to send message : ";
		char buff[128] = {0};
		printf("Please Input:");
		fgets(buff,128,stdin);
		send(sockfd,buff,strlen(buff),0);

		if(strncmp(buff,"close",3) ==0 )
		{
			break;
		}


		memset(buff,0,128);
		recv(sockfd,buff,127,0);
		printf("RecvBuff:%s\n",buff);
        printf("\n");
	}
	close(sockfd);
}