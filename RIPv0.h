
// RIPv0.h
#ifndef RIPv0_H
#define RIPv0_H

#include <string>
#include "RoutingTable.h"
using namespace std;


/* 
OVERVIEW

Our RIP Packet consists of:
Command[AMPERSAND]Source address[AMPERSAND]Destination address[AMPERSAND]Path[AMPERSAND]Data

*/


/*
Commands:

1. Add a new entry to table:
ADD <TableRow data, comma-delimited>

2. Update an existing entry in a table:
UPDATE <TableRow data, comma-delimited>

3. Delete the entry from a table:
DELETE <TableRow data, comma-delimited>

4. Send the whole table:
TABLE <Array of TableRow data, Newline-delimited and comma-delimited>

5. Pass a message (from a client, to a client/server):
MESSAGE <messageData>

*/

enum COMMAND {
    ADD,
    TABLE,
    UPDATE,
    DELETE,
    MESSAGE
};

string constructNewMessage (short cmd, int src, int dest, void* data){
    string message;
    switch (cmd)
    {
    case ADD:
        message += "ADD";
        TableRow* table_row_data = static_cast<TableRow *> (data);
        message += '&';
        message += to_string(src);        
        message += '&';
        message += to_string(dest);        
        message += '&';        
        TableRow* table_row_data = static_cast<TableRow *> (data);
        message += table_row_data->toString();
        break;
    case TABLE:
        message += "TABLE";
        break;    
    case UPDATE:
        message += "UPDATE";
        TableRow* table_row_data = static_cast<TableRow *> (data);
        message += '&';
        message += table_row_data->toString();
        break;
    case DELETE:
        message += "DELETE";
        break;
    case MESSAGE:
        message += "MESSAGE";
        message += '&';        
        message += ( * static_cast<string*> (data) );
        break;
    default:
        return NULL;
        break;
    }
}

#endif