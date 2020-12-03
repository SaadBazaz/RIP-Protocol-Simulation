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

	TableRow myself (myip, -1, "CLIENT", -1, -1, -1, -1, -1);

	cout<<"Sending my unique IP to server..."<<endl;    
	string init_msg = constructNewMessage (UPDATE, myip, router_port, (void*)&myself);
	send(sockfd,init_msg.c_str(),strlen(init_msg.c_str()),0);

	cout<<endl;

	char buff[128] = {0};

	int receiver = -1;
    
	while(buff != "close")
	{

        cout << "Waiting For Request ...\n";
        recv(sockfd,buff,127,0);
 
            // Splitting the Recieved Message into Substrings
            vector<string> data = tokenizeData(buff,'&');

            /*string domainName;
            for (size_t i = 4; data[3].size(); i++)
            {
                domainName += data[3][i];
            }*/
            string domainName = tokenizeData(data[3],' ')[2];
            cout << "Domain Requested: " <<domainName << endl;
            while (domainName.back() == '\n')
            {
                domainName.pop_back();
            }
            
            int pos = SearchDomain(domainName,List);
            string message;
            if (pos != -1)
            {
                cout << "IP Address to be Delivered : "<< List[pos].IPAddress << endl;
                string Response = "DNS RES " + List[pos].DomainName + " " + List[pos].IPAddress;
                message = constructNewMessage(MESSAGE, myip , stoi(data[1]),(void*)Response.c_str());
                send(sockfd,message.c_str(),message.size(),0);
            }
            else
            {
                /*cout << "Domain Not Found !"<<endl;
                char* notFound = "NOT_FOUND";
                message = constructNewMessage(MESSAGE, myip , stoi(data[1]),(void*)notFound);
                send(sockfd,message.c_str(),message.size(),0);*/
            }
            memset(buff, 0, 128);
        
	}
	close(sockfd);
}