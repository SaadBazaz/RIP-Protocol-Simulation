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

		auto vec = tokenizeData (data, delimiter);
		// for (int i=0; i<vec.size(); i++){
		// 	std::cout<<vec[i]<<std::endl;
		// }
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

    //overloading '==' operator
    friend bool operator==(TableRow &t1, TableRow &t2);
};

/* 
    Defining the overloading operator function
    Here we are simply comparing the hour, minute and
    second values of two different TableRow objects to compare
    their values
*/
bool operator== (TableRow &t1, TableRow &t2)
{
    return ( 
	t1.identifier == t2.identifier && 
	t1.fd == t2.fd && 
	t1.status == t2.status && 
	t1.hop_count == t2.hop_count && 
	t1.type == t2.type && 
	t1.in_use == t2.in_use && 
	t1.send_port == t2.send_port && 
	t1.receive_port == t2.receive_port 
	);
}




std::string serialize (std::vector<TableRow> &table){
	std::string returnTo;
	for (int i=0; i<table.size(); i++){
		returnTo += table[i].toString();
		if (i+1 != table.size())
			returnTo += '\n';
	}
	return returnTo;
}

std::vector<TableRow> deserialize (std::string table_string){
	std::vector <std::string> rows = tokenizeData(table_string, '\n');

	std::vector<TableRow> returnTo; 

	for (int i=0; i<rows.size(); i++){
		TableRow new_row (rows[i]);
		returnTo.push_back(new_row);
	}

	return returnTo;
}








#endif