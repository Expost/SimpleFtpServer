//
// Created by ghost on 16-4-26.
//

#ifndef SIMPLEFTPSERVER_CTRANSFER_H
#define SIMPLEFTPSERVER_CTRANSFER_H

#include <iostream>
#include "Common.h"

#define OPEN_MAX 1000

class CTransfer {
protected:
    int sock;

private:
    struct sockaddr_in sockaddr;
    std::string ip;
    int port;

    int maxi;
    struct pollfd pollfds[OPEN_MAX];
    int nready;

    bool BindListen();
public:
    CTransfer(const string ip,const int port);

    void run();
    virtual int Accept(const int client_sockfd, const struct sockaddr_in &cliaddr) = 0;
    virtual int Receive(const int client_sockfd) = 0;
    virtual ~CTransfer();
};


#endif //SIMPLEFTPSERVER_CTRANSFER_H
