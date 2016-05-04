//
// Created by ghost on 16-4-28.
//

#include "CManageClient.h"

CManageClient::CManageClient(){
    AppendCommand(new CUSERCommand);
    AppendCommand(new CPASSCommand);
    AppendCommand(new CLISTCommand);
    AppendCommand(new CPWDCommand);
    AppendCommand(new CQUITCommand);
    AppendCommand(new CCWDCommand);
    AppendCommand(new CPASVCommand);
    AppendCommand(new CSIZECommand);
    AppendCommand(new CRETRCommand);
    AppendCommand(new CSYSTCommand);
    AppendCommand(new CTYPECommand);
    AppendCommand(new CSTORCommand);
    AppendCommand(new CDELECommand);
    AppendCommand(new CMKDCommand);
    AppendCommand(new CRNFRCommand);
    AppendCommand(new CRNTOCommand);
    AppendCommand(new CRMDCommand);
}

bool CManageClient::LoadConfig() {
    FILE *file = fopen("allow_deny.config","r");
    if(file == NULL){
        cout << "allow_deny.config file is not exits." << endl;
        exit(-1);
    }
    fseek(file,0,2);
    long len = ftell(file);
    fseek(file,0,0);
    char *buf = new char[len];
    fread(buf,1,len,file);
    fclose(file);

    Json::Reader reader;
    if(reader.parse(buf,m_Config)){
        delete buf;
        m_AllowOrDeny = m_Config["bow"].asBool();   //true is black,false is white

        return true;
    }
    delete buf;
    return false;
}

CManageClient::~CManageClient(){
    for(int i = 0;i < m_Clients.size();i++){
        delete m_Clients[i];
    }
    for(int i = 0;i < m_CommandFunc.size();i++){
        delete m_CommandFunc[i];
    }
}

bool CManageClient::AppendClient(CClient *pClient){
    if(pClient != NULL) {
        m_Clients.push_back(pClient);
        return true;
    }
    return false;
}

bool CManageClient::EarseClient(const int flag){
    bool ret = false;
    for(vector<CClient*>::iterator i = m_Clients.begin();
            i != m_Clients.end();i++){
        if((*i)->IsMe(flag)){
            delete *i;
            m_Clients.erase(i);
            ret = true;
            break;
        }
    }
    return ret;
}

CClient* CManageClient::FindClient(const int flag) const{
    for(int i = 0;i < m_Clients.size();i++){
        if(m_Clients[i]->IsMe(flag))
            return m_Clients[i];
    }
    return NULL;
}

CCommand* CManageClient::FindCommand(const string command) const{
    for(int i = 0;i < m_CommandFunc.size();i++){
        if(m_CommandFunc[i]->MatchCommand(command)){
            return m_CommandFunc[i];
        }
    }
    return NULL;
}

bool CManageClient::IsInBlackList(const in_addr_t ip) const{
    if(!m_Config.empty()){
        Json::Value forbidden_ip = m_Config["black"];
        for(int i = 0;i < forbidden_ip.size();i++){
            string balst_ip = forbidden_ip[i].asString();
            if(ip == inet_addr(balst_ip.c_str())){
                return true;
            }
        }
    }
    return false;
}


bool CManageClient::IsAccepted(in_addr_t ip) const {
    if(m_AllowOrDeny){   //true is black,false is white
        if(IsInBlackList(ip))
            return false;
        else
            return true;
    }
    else{
        return IsInWhiteList(ip);
    }
}

bool CManageClient::IsInWhiteList(in_addr_t ip) const {
    if(!m_Config.empty()){
        Json::Value allow_ip = m_Config["white"];
        for(int i = 0;i < allow_ip.size();i++){
            string balst_ip = allow_ip[i].asString();
            if(ip == inet_addr(balst_ip.c_str())){
                return true;
            }
        }
    }
    return false;
}


bool CManageClient::AppendCommand(CCommand *command) {
    if(command != NULL){
        m_CommandFunc.push_back(command);
        return true;
    }
    return false;
}