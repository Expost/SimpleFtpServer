//
// Created by ghost on 16-4-28.
//


#ifndef SIMPLEFTPSERVER_CCLIENT_H
#define SIMPLEFTPSERVER_CCLIENT_H

#include "Common.h"
#include "CPasvDataTransfer.h"

class CClient {
private:
    EFTPSTATE m_State;
    string m_RootDir;
    int m_Flag;
    string m_Name;
    string m_LastFileName;
    CPasvDataTransfer *m_PasvDataTransfer;

    string m_PortIp;
    int m_PortPort;
public:
    CClient(int flag);
    ~CClient();

    ssize_t sendWlcomeMsg();
    ssize_t sendErrorMsg();
    ssize_t sendMsg(const string &msg);

    bool IsMe(int flag);

    bool SetClientPortIpAndPort(const string &ip,const int port);
    int GetClientPortIpAndPort(string &ip);

    bool SetClientPasvDataTransfer(CPasvDataTransfer *pDataTransfer);
    CPasvDataTransfer *GetClientPasvDataTransfer() const;

    bool SetClientLastFileName(const string &name);
    string GetClientLastFileName() const;

    bool SetClientState(EFTPSTATE state);
    EFTPSTATE GetClientState();
    bool SetUserDir(const string &dir);
    string GetUserDir() const;
    bool SetClientName(const string &name);
    string GetClientName() const;
    int GetClientfd() const;
};


#endif //SIMPLEFTPSERVER_CCLIENT_H
