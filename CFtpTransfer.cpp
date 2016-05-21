//
// Created by ghost on 16-4-27.
//

#include "CFtpTransfer.h"

CFtpTransfer::CFtpTransfer(const string ip,const int port,const int max_connection)
        :CTransfer(ip,port),m_CurrentConnetions(0),m_MaxConnections(max_connection){
    if(!m_ClientManager.LoadConfig()){
        cout << "Can not find the deny_ip config file,program would exit" << endl;
        exit(-1);
    }
}


int CFtpTransfer::Accept(const int client_sockfd, const struct sockaddr_in &cliaddr) {
    //cout << "Current con is: " << m_CurrentConnetions << " Max con is: " << m_MaxConnections << endl;
    if(m_CurrentConnetions >= m_MaxConnections) {
        cout << "Server is in max connections." << endl;
        return -1;
    }
    if(!m_ClientManager.IsAccepted(cliaddr.sin_addr.s_addr)){
        cout << "From ip: " << inet_ntoa(cliaddr.sin_addr) << ":" << ntohs(cliaddr.sin_port) << " is diened." << endl;
        return -1;
    }
    CClient *pClient = new CClient(client_sockfd);
    m_ClientManager.AppendClient(pClient);
    pClient->sendWlcomeMsg();

    m_CurrentConnetions++;
    cout << "Append a new client with ip:" << inet_ntoa(cliaddr.sin_addr) << ":" << ntohs(cliaddr.sin_port) << endl;
    return 1;
}

int CFtpTransfer::Receive(const int client_sockfd) {
    long int n;
    char buffer[BUFFER_SIZE];
    bzero(buffer,BUFFER_SIZE);
    n = ReceiveLine(client_sockfd,buffer,BUFFER_SIZE);
    if(n > 0){
        cout << "From client command: " << buffer << endl;
        CClient *pClient = m_ClientManager.FindClient(client_sockfd);
        if(pClient){
            CCommand *pCommand = m_ClientManager.FindCommand(string(buffer));
            if(pCommand){
                pCommand->doWhat(pClient);
                EFTPSTATE state = pClient->GetClientState();
                if(state == QUIT){
                    m_ClientManager.EarseClient(client_sockfd);
                    m_CurrentConnetions--;
                    //cout << "one con disconnected and current con is: " << m_CurrentConnetions << endl;
                }
            }
            else{
                pClient->sendErrorMsg();
            }
        }
    }
    else{
        m_ClientManager.EarseClient(client_sockfd);
        m_CurrentConnetions--;
        //cout << "one con disconnected and current con is: " << m_CurrentConnetions << endl;
    }
}

ssize_t CFtpTransfer::ReceiveLine(const int client_sockfd, char *pBuffer,int len) {
    ssize_t n = 0;
    ssize_t sum = 0;
    char c = 0;
    while((n = read(client_sockfd,&c,1)) > 0 && sum < BUFFER_SIZE){
        if(c != '\n'){
            if(c != '\r'){
                _strcat(pBuffer,c);
                sum += n;
            }
        }
        else{
            return sum;
        }
    }
    return n;
}