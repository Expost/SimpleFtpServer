//
// Created by ghost on 16-4-28.
//

#include "CClient.h"


CClient::CClient(int flag):m_Flag(flag),m_PasvDataTransfer(NULL){

}

CClient::~CClient() {
    //cout << "one client is delete." << endl;
    close(m_Flag);
}

ssize_t CClient::sendErrorMsg() {
    if(m_State < USER){
        string ret = "530 Please login with USER and PASS.\r\n";
        return sendMsg(ret);
    }
    else{
        string ret = "500 Unknown command.\r\n";
        return sendMsg(ret);
    }
}

ssize_t CClient::sendWlcomeMsg(){
    string ret = "220 SimpleFtpServer\r\n";
    return sendMsg(ret);
}

bool CClient::IsMe(int flag) {
    if(flag == m_Flag)
        return true;
    else
        return false;
}

EFTPSTATE CClient::GetClientState() {
    return m_State;
}

bool CClient::SetClientState(EFTPSTATE state) {
    m_State = state;
    return true;
}

bool CClient::SetUserDir(const string &dir){
    if(!dir.empty()){
        m_RootDir = dir;
        return true;
    }
    else{
        return false;
    }
}

string CClient::GetUserDir() const{
    return m_RootDir;
}

bool CClient::SetClientName(const string &name){
    m_Name = name;
    return true;
}

string CClient::GetClientName() const{
    return m_Name;
}

int CClient::GetClientfd() const{
    return m_Flag;
}

ssize_t CClient::sendMsg(const string &msg){
    return write(m_Flag,msg.c_str(),msg.size());
}

bool CClient::SetClientPasvDataTransfer(CPasvDataTransfer *pDataTransfer) {
    m_PasvDataTransfer = pDataTransfer;
}

CPasvDataTransfer *CClient::GetClientPasvDataTransfer() const{
    return m_PasvDataTransfer;
}


bool CClient::SetClientLastFileName(const string &name){
    if(!name.empty()){
        m_LastFileName = name;
        return true;
    }
    else{
        return false;
    }
}
string CClient::GetClientLastFileName() const{
    return m_LastFileName;
}

bool CClient::SetClientPortIpAndPort(const string &ip, const int port) {
    m_PortIp = ip;
    m_PortPort = port;
    return true;
}


int CClient::GetClientPortIpAndPort(string &ip) {
    ip = m_PortIp;
    m_PortIp.clear();
    return m_PortPort;
}