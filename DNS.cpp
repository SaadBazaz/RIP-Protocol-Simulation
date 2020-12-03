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
#include "DNS_FILE.h"
#include <vector>
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
vector<Directory> initiateList(){
    vector<Directory> List;
    Directory entry1 = {"www.google.com","209.85.231.104"};
    List.push_back(entry1);
    Directory entry2 = {"www.microsoft.com","207.46.170.123"};
    List.push_back(entry2);
    Directory entry3 = {"www.yahoo.com","72.30.2.43"};
    List.push_back(entry3);
    Directory entry4 = {"www.facebook.com","66.220.149.25"};
    List.push_back(entry4);
    Directory entry5 = {"www.wikipedia.com","208.80.152.2"};
    List.push_back(entry5);
    Directory entry6 = {"www.dell.com","143.166.83.38"};
    List.push_back(entry6);
    Directory entry7 = {"www.twitter.com","128.242.245.116"};
    List.push_back(entry7);
    return List;
}
void PrintDirectory(vector<Directory> List){
    for(int i = 0;i < List.size();i++){
        cout << "Domain Name: " << List[i].DomainName << "\tIP Address: " << List[i].IPAddress << endl;
    }
}
int SearchDomain(string Dom,vector<Directory> List){
    for(int i = 0;i < List.size();i++){
        
        cout<<"Matching '"<<Dom<<"' with "<<List[i].DomainName<<endl;
        if (Dom == List[i].DomainName)
        {
            return i;
        }
    }
    return -1;
}
int main()
{
    vector<Directory> List = initiateList();
    PrintDirectory(List);
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
    int x = 0;
	while(buff != "close")
	{
        ++x;
        cout << "Waiting For Request ...\n";
        recv(sockfd,buff,127,0);
        if (x>1)
        {
            // Splitting the Recieved Message into Substrings
            vector<string> data = tokenizeData(buff,'&');

            /*string domainName;
            for (size_t i = 4; data[3].size(); i++)
            {
                domainName += data[3][i];
            }*/

            cout << "Domain Requested: " << data[3] << endl;
            data[3].pop_back();
            int pos = SearchDomain(data[3],List);
            string message;
            if (pos != -1)
            {
                cout << "IP Address to be Delivered : "<< List[pos].IPAddress << endl;
                message = constructNewMessage(MESSAGE, myip , stoi(data[1]),(void*)List[pos].IPAddress);
                send(sockfd,message.c_str(),message.size(),0);
            }
            else
            {
                /*cout << "Domain Not Found !"<<endl;
                char* notFound = "NOT_FOUND";
                message = constructNewMessage(MESSAGE, myip , stoi(data[1]),(void*)notFound);
                send(sockfd,message.c_str(),message.size(),0);*/
            }
        }
        
        
	}
	close(sockfd);
}