#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<assert.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include "RIPv0.h"
#include <pthread.h>
#include <time.h>
#include "RoutingTable.h"
using namespace std;

/*
As we assume the client will only be connected to one device at a time,
We will not maintain the fds array etc. We'll just send messages
or listen to one socket only.
*/


int sockfd;
pthread_t listener_thread;


/*
Listens to messages from the router it is connected to
*/
void* messageListener (void* Args){
	// cout<<"Thread start"<<endl;
	sleep(2);	// Give it some time to start
	char msg_buff[128] = {0};
	// cout<<"I'm here"<<endl;
	do{
		recv(sockfd,msg_buff,127,0);
		printf("Received a message: %s\n",msg_buff);
		printf("\n");
		// cout<<"In loop";
		memset(msg_buff,0,128);
		sleep(2);
	} while(strcmp(msg_buff,"close") != 0);	
}

int main()
{
    int router_port;
	int myip;
    cout <<">> Which server do you want to connect to? ";
    cin >> router_port;

	cout <<">> Enter your unique IP: ";
    cin >> myip;
	
	cout<<endl;

	cout<<"Starting connection to server..."<<endl;
	sockfd = socket(AF_INET,SOCK_STREAM,0);	
	assert(sockfd != -1 );

	//Set Address Information
	struct sockaddr_in saddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(router_port);
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//Link to router
	int res = connect(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
	assert(res != -1);

	cout<<"Creating listening thread..."<<endl;    
	pthread_create (&listener_thread, NULL, messageListener, (void*)NULL); 

	TableRow myself (myip, -1, "CLIENT", -1, -1, -1, -1, -1);

	cout<<"Sending my unique IP to server..."<<endl;    
	string init_msg = constructNewMessage (UPDATE, myip, router_port, (void*)&myself);
	send(sockfd,init_msg.c_str(),strlen(init_msg.c_str()),0);

	cout<<endl;

	char buff[128] = {0};

	int receiver = -1;
	while(buff != "close")
	{
		sleep(2);
		cout<<">> Enter the unique IP of the client you wish to talk to: ";
		cin>>receiver;

		memset(buff,0,128);
		cout<<">> YOU: ";
		cin.ignore();
		fgets(buff,128,stdin);

		string message = constructNewMessage(MESSAGE, myip, receiver, (void*)buff );
		send(sockfd,message.c_str(),message.size(),0);

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