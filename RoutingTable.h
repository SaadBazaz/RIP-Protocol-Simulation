
#include <string>

struct TableRow {
    int identifier;
    int client_name;
    int fd;
    int hop_count;
	std::string type;
	bool in_use;
	int send_port;
	int receive_port;

    TableRow (int id,int r,int f, int hc, std::string tp, bool iu = false, int sp = -1, int rp = -1):
	identifier(id),
	client_name(r),
	fd(f),
	hop_count(hc),
	type(tp),
	in_use(0),		//Available by default
	send_port(sp),
	receive_port(rp)	
	{}

	std::string toString (char delimiter = ','){
		return std::to_string(identifier) 		+ ','
		+ 	   std::to_string(client_name) 		+ ','
		+ 	   std::to_string(fd) 				+ ','
		+ 	   std::to_string(hop_count) 		+ ','
		+ 	   type					 			+ ','
		+ 	   std::to_string(in_use) 			+ ','
		+ 	   std::to_string(send_port) 		+ ','
		+ 	   std::to_string(receive_port) 	;
	}
};

