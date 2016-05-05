//
// Created by ghost on 16-5-5.
//

#ifndef SIMPLEFTPSERVER_CPORTDATATRANSFER_H
#define SIMPLEFTPSERVER_CPORTDATATRANSFER_H

#include "Common.h"
#include "CThread.h"

class CPortDataTransfer: public CThread {
private:
    string m_Msg;
    string m_RemoteIp;
    int m_RemotePort;

    int m_ControlSock;
    int DispatchCommand(int client_fd);
    ssize_t ControlSockSendMsg(const string &msg);
    void Execute();

public:
    CPortDataTransfer(string &ip,int port);
    bool SetControlSock(int sockfd);
    bool SetMsg(const string &msg);
};


#endif //SIMPLEFTPSERVER_CPORTDATATRANSFER_H
