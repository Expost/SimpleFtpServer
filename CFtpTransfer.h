//
// Created by ghost on 16-4-27.
//

#ifndef SIMPLEFTPSERVER_CFTPTRANSFER_H
#define SIMPLEFTPSERVER_CFTPTRANSFER_H

#include "CTransfer.h"
#include "CManageClient.h"
#include "Common.h"

class CFtpTransfer:public CTransfer{
private:
    CManageClient m_ClientManager;
    long int ReceiveLine(const int client_sockfd,char *pBuffer,int len);
public:
    CFtpTransfer(const string ip,const int port);
    int Accept(const int client_sockfd, const struct sockaddr_in &cliaddr);
    int Receive(const int client_sockfd);
};



#endif //SIMPLEFTPSERVER_CFTPTRANSFER_H
