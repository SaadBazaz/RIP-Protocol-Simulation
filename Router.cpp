#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <assert.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <vector>
#include "RoutingTable.h"
#include "RIPv0.h"
using namespace std;
#define MAXFD 10 //Size of fds array

std::vector<TableRow> table;
int port;

void printTable()
{
	cout << "Identifier\tFD\tHop Count\tIn Use?" << endl;
	cout << "----------------------------------------------------" << endl;
	for (int i = 0; i < table.size(); i++)
	{
		cout << table[i].identifier << "\t\t" << table[i].fd << "\t" << table[i].hop_count << "\t\t" << (table[i].in_use ? "Yes" : "No") << endl;
	}
}

int findRow(int id)
{
	for (int i = 0; i < table.size(); i++)
	{
		if (id == table[i].identifier)
		{
			return i;
		}
	}
	return -1;
}

int findRowByID_MinHopCount(int id)
{
	int min_hop_count = 10000;
	int index = -1;
	for (int i = 0; i < table.size(); i++)
	{
		if (id == table[i].identifier)
		{
			if (table[i].hop_count < min_hop_count)
			{
				index = i;
				min_hop_count = table[i].hop_count;
			}
		}
	}
	return index;
}

int findRowByFD(int fd)
{
	for (int i = 0; i < table.size(); i++)
	{
		if (fd == table[i].fd)
		{
			return i;
		}
	}
	return -1;
}


int findRowByFDAndID(int fd, int id)
{
	for (int i = 0; i < table.size(); i++)
	{
		if (fd == table[i].fd and id == table[i].identifier)
		{
			return i;
		}
	}
	return -1;
}



vector<int> findAllRowsByType(string type)
{
	vector<int> returnTo;
	for (int i = 0; i < table.size(); i++)
	{
		if (table[i].type == type)
		{
			returnTo.push_back(i);
		}
	}
	return returnTo;
}




vector<int> findAllRowsByFD(int fd)
{
	vector<int> returnTo;
	for (int i = 0; i < table.size(); i++)
	{
		if (fd == table[i].fd)
		{
			returnTo.push_back(i);
		}
	}
	return returnTo;
}



void sendTableToAllRouters(int ignore_fd = -1){
	auto index_vec = findAllRowsByType("ROUTER");

	for (int i=0; i<index_vec.size(); i++){

		if (table[index_vec[i]].fd == ignore_fd)
			continue;

		cout<<"Index is "<<index_vec[i]<<endl;

		string table_string = serialize (table);
		auto router = table[index_vec[i]];
		auto to_send = constructNewMessage(TABLE, port, router.identifier, (void*)table_string.c_str());
		send (router.fd, to_send.c_str(), to_send.size(), 0);
	}

}





void handlePacket(const int &client, string packet)
{
	auto vec = tokenizeData(packet, '&');
	// for (int i=0; i<vec.size(); i++){
	// 		std::cout<<vec[i]<<std::endl;
	// }


	if (vec[0] == "OK"){
		std::cout<<"Something good happened!"<<std::endl;
		return;
	}
	else if (vec[0] == "BAD"){
		std::cout<<"Oops, something went wrong..."<<std::endl;
		return;
	}


	switch (stoi(vec[0]))
	{
	case UPDATE:
	{
		// cout<<"In UPDATE..."<<endl;
		TableRow table_row(vec[3]);
		// cout<<"Made table_row..."<<endl;
		int index = findRowByFDAndID(client, table_row.identifier);

		if (index < 0)
		{

			// if match not found
			// remove all id = -1 values from table and add new table_row
			for (int i=0; i<table.size(); i++){
				if (table[i].identifier == -1){
					table.erase (table.begin() + i);
					i--;
				}
			} 

			table_row.fd = client;
			if (table_row.hop_count < 0)
			{
				table_row.hop_count = 0;
			}
			
			table.push_back(table_row);
			printTable();
			break;
		}


		if (table_row.identifier >= 0)
		{
			table[index].identifier = table_row.identifier;
		}

		// if (table_row.client_name >= 0){
		// 	table[index].client_name = table_row.client_name;
		// }

		if (table_row.status >= 0)
		{
			table[index].status = table_row.status;
		}

		if (table_row.hop_count >= 0)
		{
			table[index].hop_count = table_row.hop_count;
		}

		if (table_row.type != "")
		{
			table[index].type = table_row.type;
		}

		// if (table_row.in_use >= 0){
		table[index].in_use = table_row.in_use;
		// }

		if (table_row.send_port >= 0)
		{
			table[index].send_port = table_row.send_port;
		}

		if (table_row.receive_port >= 0)
		{
			table[index].receive_port = table_row.receive_port;
		}

		printTable();
		sendTableToAllRouters();

		break;
	}
	case ADD:
		break;

	case DELETE:
		break;

	case TABLE:{

		// auto index_vec = findAllRowsByFD(client);

		auto new_rows = deserialize(vec[3]);		


		// cout<<"New Rows - - - - - "<<endl;
		for (int i=0; i<new_rows.size(); i++){


			new_rows[i].fd = client;

			// cout<<new_rows[i].identifier<<endl;

		}
		// cout<<"- - - - - - - - - - "<<endl;



		// Clean useless data out of new_rows vector
		for (int i=0; i<table.size(); i++)
		{
			for (int j=0; j<new_rows.size(); j++){
				if (table[i] == new_rows[j] 
				or (table[i].fd == client and table[i].identifier == new_rows[j].identifier)
				or (table[i].identifier == new_rows[j].identifier and table[i].hop_count == 0)
				or (new_rows[j].hop_count >= 15)
				//  or new_rows[j].identifier == -1 or new_rows[j].identifier == port
				)

				{
					new_rows.erase(new_rows.begin() + j);
					j--;	
				}
			}
		}

		for (int j=0; j<new_rows.size(); j++){
			if (new_rows[j].identifier == -1 or new_rows[j].identifier == port){
				new_rows.erase(new_rows.begin() + j);
				j--;
			}
		}


		// cout<<"New Rows [CLEANED] - - - - - "<<endl;
		// for (int i=0; i<new_rows.size(); i++){
		// 	cout<<new_rows[i].identifier<<endl;
		// }
		// cout<<"- - - - - - - - - - "<<endl;



		// Add data from new_rows into table
		for (int i=0; i<new_rows.size(); i++){
				new_rows[i].hop_count ++;
				new_rows[i].fd = client;
				table.push_back(new_rows[i]);
		}

		// for (int i=0; i<new_rows.size(); i++){
		// 	if (new_rows[i].identifier != port)
		// 		table.push_back(new_rows[i]);
		// }

		printTable();
		sendTableToAllRouters(client);  //ignore this client when sending table to routers
		break;
	}

	case MESSAGE:
	{
		int index = findRowByID_MinHopCount(stoi(vec[2]));

		if (index < 0)
		{
			break;
		}

		if (vec.size() == 4){
			packet += '&';
			packet += to_string(port);
		}
		else {
			packet += ',';
			packet += to_string(port);			
		}

		// Forward packet to appropriate socket
		send(table[index].fd, packet.c_str(), strlen(packet.c_str()), 0);
		break;
	}
	default:
		break;
	}
	// if (vec[0] == UPDATE){
	// 	if ()

	// }
	// send(client, "OK", 2, 0); //Reply message to client
}

void fds_add(int fds[], int fd) //Add a file descriptor to the fds array
{
	int i = 0;
	for (; i < MAXFD; ++i)
	{
		if (fds[i] == -1)
		{
			fds[i] = fd;
			break;
		}
	}
}

int main()
{
	cout << "Which Unique Port do you want to use?\n";
	cin >> port;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(sockfd != -1);

	printf("sockfd=%d\n", sockfd);

	struct sockaddr_in saddr, caddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int res = bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr));
	assert(res != -1);

	//Create listening queue
	listen(sockfd, 5);

	//Define fdset collection
	fd_set fdset;

	//Define fds array
	int fds[MAXFD];
	int i = 0;
	for (; i < MAXFD; ++i)
	{
		fds[i] = -1;
	}


	//Add a file descriptor to the fds array
	fds_add(fds, sockfd);





{
	char option;
	
	do {

		cout << "Would you like to connect to any other routers? (y/n)\n";
		cin >> option;
		if (option == 'n' or option == 'N'){
			break;
		} 

		int router_port;
		cout<<"Enter the partner router's Unique Port: ";
		cin >> router_port;

		int sockfd_router = socket(AF_INET, SOCK_STREAM, 0);
		assert(sockfd_router != -1);

		//Set Address Information
		struct sockaddr_in saddr;
		memset(&saddr, 0, sizeof(saddr));
		saddr.sin_family = AF_INET;
		saddr.sin_port = htons(router_port);
		saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

		//Link to router
		int res = connect(sockfd_router, (struct sockaddr *)&saddr, sizeof(saddr));
		assert(res != -1);

		TableRow myself(port, -1, "ROUTER");

		cout << "Sending this router's unique port to partner router..." << endl;
		string init_msg = constructNewMessage(UPDATE, port, router_port, (void *)&myself);
		send(sockfd_router, init_msg.c_str(), strlen(init_msg.c_str()), 0);

		cout << endl;


		TableRow table_row (router_port, sockfd_router, "ROUTER");
		table.push_back(table_row);

		fds_add(fds, sockfd_router);		

		printTable();

	}
	while(option != 'n' and option != 'N');


}


	while (1)
	{
		FD_ZERO(&fdset); //Clear the fdset array to 0

		int maxfd = -1;

		int i = 0;

		//For loop finds the maximum subscript for the ready event in the fds array
		for (; i < MAXFD; i++)
		{
			if (fds[i] == -1)
			{
				continue;
			}

			FD_SET(fds[i], &fdset);

			if (fds[i] > maxfd)
			{
				maxfd = fds[i];
			}
		}

		struct timeval tv = {5, 0}; //Set timeout of 5 seconds

		int n = select(maxfd + 1, &fdset, NULL, NULL, &tv); //Selectect system call, where we only focus on read events
		if (n == -1)										//fail
		{
			perror("select error");
		}
		else if (n == 0) //Timeout, meaning no file descriptor returned
		{
			printf(".\n");
			sendTableToAllRouters();
		}
		else //Ready event generation
		{
			//Because we only know the number of ready events by the return value of select, we don't know which events are ready.
			//Therefore, each file descriptor needs to be traversed for judgment
			for (i = 0; i < MAXFD; ++i)
			{
				if (fds[i] == -1) //If fds[i]==-1, the event is not ready
				{
					continue;
				}
				if (FD_ISSET(fds[i], &fdset)) //Determine if the event corresponding to the file descriptor is ready
				{

					//There are two kinds of cases for judging file descriptors
					try
					{
						if (fds[i] == sockfd) //A file descriptor is a socket, meaning accept if a new client requests a connection
						{
							//accept
							struct sockaddr_in caddr;
							int len = sizeof(caddr);

							int c = accept(sockfd, (struct sockaddr *)&caddr, (socklen_t *)&len); //Accept new client connections
							if (c < 0)
							{
								continue;
							}

							printf("accept c=%d\n", c);
							fds_add(fds, c); //Add the connection socket to the array where the file descriptor is stored
							TableRow newClient(-1, c, "UNKNOWN");
							table.push_back(newClient);
							printTable();

							sendTableToAllRouters();

						}
						else //Receive data recv when an existing client sends data
						{
							char buff[500] = {0};
							int res = recv(fds[i], buff, 499, 0);
							if (res <= 0)
							{
								close(fds[i]);
								auto index_vec = findAllRowsByFD(fds[i]);

								fds[i] = -1;
								printf("one client over\n");

								for (int i=0; i<index_vec.size(); i++)
								{
									table.erase(table.begin() + index_vec[i]);
									if (i+1!=index_vec.size())
										index_vec[i+1]--;
								}

								printTable();
								sendTableToAllRouters();
							}
							else
							{
								printf("recv(%d)=%s\n", fds[i], buff); //Output Client Sent Information
								handlePacket(fds[i], buff);
							}
						}
					}
					catch (...)
					{
						cout<<"Crashed!"<<endl;
						// send(fds[i], "BAD", 3, 0);
						sleep(5);
					}
				}
			}
		}
	}
}