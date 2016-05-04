//
// Created by ghost on 16-4-28.
//

#ifndef SIMPLEFTPSERVER_CMANAGECLIENT_H
#define SIMPLEFTPSERVER_CMANAGECLIENT_H

#include "Common.h"
#include "CClient.h"
#include "CFtpCommand.h"
#include "json/json.h"

class CManageClient {
private:
    vector<CClient*> m_Clients;
    vector<CCommand *>m_CommandFunc;
    Json::Value m_Config;

    bool m_AllowOrDeny;

    bool IsInBlackList(in_addr_t ip) const;
    bool IsInWhiteList(in_addr_t ip) const;
public:
    CManageClient();
    ~CManageClient();
    bool LoadConfig();
    bool AppendClient(CClient *pClient);
    bool AppendCommand(CCommand *command);
    bool EarseClient(const int flag);
    bool IsAccepted(in_addr_t ip) const;
    CClient *FindClient(const int flag) const;
    CCommand *FindCommand(const string command) const;
};


#endif //SIMPLEFTPSERVER_CMANAGECLIENT_H
