#ifndef DNS_FILE_H
#define DNS_FILE_H

#include <string>
#include <sstream>
#include <vector>

struct Directory {
    string DomainName;
    char IPAddress[128];
};

#endif