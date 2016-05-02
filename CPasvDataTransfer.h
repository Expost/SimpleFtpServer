//
// Created by ghost on 16-4-30.
//

#ifndef SIMPLEFTPSERVER_CDATATRANSFER_H
#define SIMPLEFTPSERVER_CDATATRANSFER_H

#include <semaphore.h>
#include "Common.h"
#include "CThread.h"


class CPasvDataTransfer: public CThread {
private:

    int m_Sock;
    int m_CliSock;
    struct sockaddr_in m_Servaddr;
    sem_t m_Signal;
    string m_Msg;

    int m_ControlSock;

    int DispatchCommand(int client_fd);
    ssize_t ControlSockSendMsg(const string &msg);
    void Execute();
public:
    CPasvDataTransfer(const string &ip,int port);
    ~CPasvDataTransfer();

    bool SetControlSock(int sockfd);
    int GetIpAndPort(string &ip);
    bool SetMsg(const string &msg);
    bool PostSignal();
};


#endif //SIMPLEFTPSERVER_CDATATRANSFER_H
