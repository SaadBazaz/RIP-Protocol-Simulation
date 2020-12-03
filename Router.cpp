#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<unistd.h>
#include<string>
#include<assert.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<vector>
#include "RoutingTable.h"
#include "RIPv0.h"
using namespace std;
#define MAXFD 10	//Size of fds array


std::vector<TableRow> table;

void printTable(){
    cout<<"Identifier\tFD\tHop Count\tIn Use?"<<endl;
    cout<<"----------------------------------------------------"<<endl;
    for (int i=0; i<table.size(); i++){
        cout<<table[i].identifier<<"\t\t"<<table[i].fd<<"\t"<<table[i].hop_count<<"\t\t"<<(table[i].in_use ? "Yes":"No")<<endl;
    }
}

int findRow (int id){
	for (int i=0; i<table.size(); i++){
		if (id == table[i].identifier){
			return i;
		}
	}
	return -1;
}

int findRowByID_MinHopCount (int id){
	int min_hop_count = 10000;
	int index = -1;
	for (int i=0; i<table.size(); i++){
		if (id == table[i].identifier){
			if (table[i].hop_count<min_hop_count){
				index = i;
				min_hop_count = table[i].hop_count;
			}
		}
	}
	return index;
}

int findRowByFD_MinHopCount (int fd){
	int min_hop_count = 10000;
	int index = -1;
	for (int i=0; i<table.size(); i++){
		if (fd == table[i].fd){
			if (table[i].hop_count<min_hop_count){
				index = i;
				min_hop_count = table[i].hop_count;
			}
		}
	}
	return index;
}


void handlePacket (const int &client, string packet){
	auto vec = tokenizeData(packet, '&');
	for (int i=0; i<vec.size(); i++){
			std::cout<<vec[i]<<std::endl;
	}
	switch (stoi(vec[0]))
	{
	case UPDATE:{
		cout<<"In UPDATE..."<<endl;
		TableRow table_row (vec[3]);
		cout<<"Made table_row..."<<endl;
		int index = findRowByFD_MinHopCount(client);

		if (index < 0){
			break;
		}
		
		if (table_row.identifier >= 0){
			table[index].identifier = table_row.identifier;
		}

		// if (table_row.client_name >= 0){
		// 	table[index].client_name = table_row.client_name;
		// }

		if (table_row.status >= 0){
			table[index].status = table_row.status;
		}

		if (table_row.hop_count >= 0){
			table[index].hop_count = table_row.hop_count;
		}

		if (table_row.type != ""){
			table[index].type = table_row.type;
		}

		// if (table_row.in_use >= 0){
			table[index].in_use = table_row.in_use;
		// }

		if (table_row.send_port >= 0){
			table[index].send_port = table_row.send_port;
		}

		if (table_row.receive_port >= 0){
			table[index].receive_port = table_row.receive_port;
		}

		printTable();

		break;
	}
	case ADD:
		break;

	case DELETE:
		break;
	
	case MESSAGE:{
		int index = findRowByID_MinHopCount(stoi(vec[2]));

		if (index < 0){
			break;
		}

		// Forward packet to appropriate socket
		send(table[index].fd, packet.c_str(), strlen(packet.c_str()),0);
		break;
	}
	default:
		break;
	}
	// if (vec[0] == UPDATE){
	// 	if ()

	// }
	send(client,"OK",2,0);	//Reply message to client
}

void fds_add(int fds[],int fd)	//Add a file descriptor to the fds array
{
	int i=0;
	for(;i<MAXFD;++i)
	{
		if(fds[i]==-1)
		{
	      fds[i]=fd;
		  break;
		}
	}
}

int main()
{
    int port;
    cout<<"Which Unique Port do you want to use?\n";
    cin >> port;

	int sockfd=socket(AF_INET,SOCK_STREAM,0);
	assert(sockfd!=-1);
	
    printf("sockfd=%d\n",sockfd);
    
	struct sockaddr_in saddr,caddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family=AF_INET;
	saddr.sin_port=htons(port);
	saddr.sin_addr.s_addr=inet_addr("127.0.0.1");

	int res=bind(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
	assert(res!=-1);
	
	//Create listening queue
	listen(sockfd,5);
	
   //Define fdset collection
    fd_set fdset;
	
	//Define fds array
    int fds[MAXFD];
    int i=0;
    for(;i<MAXFD;++i)
    {
	  	fds[i]=-1;
    }
	
	//Add a file descriptor to the fds array
    fds_add(fds,sockfd);

	while(1)
    {
		FD_ZERO(&fdset);//Clear the fdset array to 0

		int maxfd=-1;

		int i=0;

		//For loop finds the maximum subscript for the ready event in the fds array
		for(;i<MAXFD;i++)
		{
			if(fds[i]==-1)
			{
				continue;
			}

			FD_SET(fds[i],&fdset);

			if(fds[i]>maxfd)
			{
				maxfd=fds[i];
			}
		}

		struct timeval tv={5,0};	//Set timeout of 5 seconds

		int n=select(maxfd+1,&fdset,NULL,NULL,&tv);//Selectect system call, where we only focus on read events
		if(n==-1)	//fail
		{
			perror("select error");
		}
		else if(n==0)//Timeout, meaning no file descriptor returned
		{
			printf(".\n");
		}
		else//Ready event generation
		{
		//Because we only know the number of ready events by the return value of select, we don't know which events are ready.
		//Therefore, each file descriptor needs to be traversed for judgment
			for(i=0;i<MAXFD;++i)
			{
				if(fds[i]==-1)	//If fds[i]==-1, the event is not ready
				{
					continue;
				}
				if(FD_ISSET(fds[i],&fdset))	//Determine if the event corresponding to the file descriptor is ready
				{
			   
				//There are two kinds of cases for judging file descriptors
			   
					if(fds[i]==sockfd)	//A file descriptor is a socket, meaning accept if a new client requests a connection
					{
						//accept
						struct sockaddr_in caddr;
						int len=sizeof(caddr);

						int c=accept(sockfd,(struct sockaddr *)&caddr,(socklen_t *)&len);	//Accept new client connections
						if(c<0)
						{
							continue;
						}
					
						printf("accept c=%d\n",c);
						fds_add(fds,c);//Add the connection socket to the array where the file descriptor is stored
                        TableRow newClient (-1, c, "UNKNOWN");
                        table.push_back(newClient);
                        printTable();
					}
					else   //Receive data recv when an existing client sends data
					{
						char buff[128]={0};
						int res=recv(fds[i],buff,127,0);
						if(res<=0)
						{
							close(fds[i]);
							fds[i]=-1;
							printf("one client over\n");

						}
						else
						{
							printf("recv(%d)=%s\n",fds[i],buff);	//Output Client Sent Information
							handlePacket(fds[i], buff);
						}
					}
				}
			}
		}
	}
}