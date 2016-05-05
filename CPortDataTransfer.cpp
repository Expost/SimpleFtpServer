//
// Created by ghost on 16-5-5.
//

#include "CPortDataTransfer.h"


CPortDataTransfer::CPortDataTransfer(string &ip,int port):m_RemoteIp(ip),m_RemotePort(port){};


bool CPortDataTransfer::SetControlSock(int sockfd){
    m_ControlSock = sockfd;
    return true;
}


bool CPortDataTransfer::SetMsg(const string &msg){
    m_Msg = msg;
    return true;
}

ssize_t CPortDataTransfer::ControlSockSendMsg(const string &msg){
    return write(m_ControlSock,msg.c_str(),msg.size());
}

void CPortDataTransfer::Execute() {
    int clientsock = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in clientaddr;
    clientaddr.sin_addr.s_addr = inet_addr(m_RemoteIp.c_str());
    clientaddr.sin_port = htons(m_RemotePort);
    clientaddr.sin_family = AF_INET;

    if(0 == connect(clientsock,(struct sockaddr*)&clientaddr,sizeof(clientaddr))){
        int ret = DispatchCommand(clientsock);
    }
    else{
        string ret = "425 Failed to establish connection.\r\n";
        ControlSockSendMsg(ret);
    }

    close(clientsock);
    delete this;
}


int CPortDataTransfer::DispatchCommand(int client_fd) {
    vector<string> v;
    SplitString(m_Msg,v,"#");
    if(v[0] == "SENDLIST"){
        if(!v[1].empty()){
            send(client_fd,v[1].c_str(),v[1].size(),0);
        }
        else{
            send(client_fd,"\r\n",2,0);
        }
        string ret = "226 Directory send OK.\r\n";
        ControlSockSendMsg(ret);
    }
    else if(v[0] == "SENDFILE"){
        //cout << "Get file " << v[1].c_str() << "  " <<  v[2].c_str() << endl;

        chdir(v[1].c_str());
        FILE *file = fopen(v[2].c_str(),"rb");
        fseek(file,0,2);
        long size = ftell(file);
        fseek(file,0,0);
        int n = 0;
        char buffer[BUFFER_SIZE];
        bzero(buffer,BUFFER_SIZE);
        time_t current_time,pre_time;
        pre_time= time(NULL);
        unsigned long sum_sec = 0;
        unsigned long sum = 0;
        while((n = fread(buffer,1,BUFFER_SIZE,file)) > 0){
            if(send(client_fd,buffer,n,0) <= 0)
                break;
            sum_sec += n;
            sum += n;
            current_time = time(NULL);
            if(current_time - pre_time >= 1){
                pre_time = current_time;
                double speed = (sum_sec * 1.0) / 1024;
                double progress = sum * 1.0 / size * 100;
                printf("%s ,progress: %.1f%% ,speed is: %.1f KB/s\n",v[2].c_str(),progress,speed);
                sum_sec = 0;
            }
            bzero(buffer,BUFFER_SIZE);
        }
        fclose(file);
        cout << "Send file completed." << endl;
        string ret = "226 Transfer complete.\r\n";
        ControlSockSendMsg(ret);
        return 1;
    }
    else if(v[0] == "RECVFILE"){
        cout << "Get file " << v[1].c_str() << "  " <<  v[2].c_str() << endl;
        chdir(v[1].c_str());
        FILE *file = fopen(v[2].c_str(),"wb");
        ssize_t n = 0;
        char buffer[BUFFER_SIZE];
        bzero(buffer,BUFFER_SIZE);
        time_t current_time,pre_time;
        pre_time = time(NULL);
        unsigned long sum_sec = 0;
        unsigned sum = 0;
        while((n = recv(client_fd,buffer,BUFFER_SIZE,0)) > 0){
            fwrite(buffer,1,n,file);
            sum_sec += n;
            current_time = time(NULL);
            if(current_time - pre_time >= 1){
                pre_time = current_time;
                double speed = (sum_sec  * 1.0) / 1024;
                printf("speed is: %.1f KB/s\n",speed);
                sum_sec = 0;
            }
            bzero(buffer,BUFFER_SIZE);
        }
        fclose(file);
        cout << "Recv file completed." << endl;
        string ret = "226 Transfer complete.\r\n";
        ControlSockSendMsg(ret);
        return 1;
    }
    else{
        return 0;
    }
}