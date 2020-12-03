// RIPv0.h
#ifndef ROUTING_TABLE_H
#define ROUTING_TABLE_H

#include <string>
#include <sstream>
#include <vector>

enum Table_Status {
	CONNECTING,
	CONNECTED,
	FAILED
};


std::vector<std::string> tokenizeData (std::string buffer, char delimiter = ','){
		std::stringstream ss(buffer); 
	    std::vector<std::string> v; 
	  
		while (ss.good()) { 
		    std::string substr; 
		    getline(ss, substr, delimiter); 
		    v.push_back(substr); 
		} 
		
		return v;		
}

struct TableRow {
    int identifier;
    // int client_name;
    int fd;
	short int status;
    int hop_count;
	std::string type;
	bool in_use;
	int send_port;
	int receive_port;

    TableRow (int id, int fd, std::string tp, short int st = CONNECTING, int hc = 0, bool iu = false, int sp = -1, int rp = -1):
	identifier(id),
	// client_name(cn),
	fd(fd),
	status(st),
	hop_count(hc),
	type(tp),
	in_use(0),		//Available by default
	send_port(sp),
	receive_port(rp)	
	{}

	TableRow (std::string data, char delimiter = ','){

		auto vec = tokenizeData (data);
		for (int i=0; i<vec.size(); i++){
			std::cout<<vec[i]<<std::endl;
		}
		identifier = stoi(vec[0]);
		// int client_name;
		fd = stoi(vec[1]);
		status = stoi(vec[2]);
		hop_count = stoi(vec[3]);
		type = vec[4];
		in_use = stoi(vec[5]);
		send_port = stoi(vec[6]);
		receive_port = stoi(vec[7]);
		
	}

	std::string toString (char delimiter = ','){
		return std::to_string(identifier) 		+ ','
		// + 	   std::to_string(client_name) 		+ ','
		+ 	   std::to_string(status) 			+ ','		
		+ 	   std::to_string(fd) 				+ ','
		+ 	   std::to_string(hop_count) 		+ ','
		+ 	   type					 			+ ','
		+ 	   std::to_string(in_use) 			+ ','
		+ 	   std::to_string(send_port) 		+ ','
		+ 	   std::to_string(receive_port) 	
		;
	}



};

#endif