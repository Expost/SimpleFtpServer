//
// Created by ghost on 16-4-27.
//

#ifndef SIMPLEFTPSERVER_COMMON_H
#define SIMPLEFTPSERVER_COMMON_H

#include <netinet/in.h>
#include <cstdlib>
#include <cerrno>
#include <string.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <poll.h>
#include <vector>
#include <iostream>

using namespace std;

#define BUFFER_SIZE 4096

typedef enum{
    NOP,
    QUIT,
    USER,
    PASS,
    RNFR,
    PASV
} EFTPSTATE;



char *_strcat(char *pBuffer, const char c);
void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c);

#endif //SIMPLEFTPSERVER_COMMON_H
