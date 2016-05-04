//
// Created by ghost on 16-4-30.
//

#include <netdb.h>
#include <semaphore.h>
#include "CPasvDataTransfer.h"


CPasvDataTransfer::CPasvDataTransfer(const string &ip, int port):
        m_ControlSock(0){
    sem_init(&m_Signal,NULL,0);
    m_Sock = socket(AF_INET,SOCK_STREAM,0);
    m_Servaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    m_Servaddr.sin_family = AF_INET;
    m_Servaddr.sin_port = htons(port);

    bind(m_Sock,(struct sockaddr*)&m_Servaddr,sizeof(m_Servaddr));
    listen(m_Sock,3);
}

CPasvDataTransfer::~CPasvDataTransfer() {
    close(m_CliSock);
    close(m_Sock);
}

int CPasvDataTransfer::GetIpAndPort(string &ip) {
    struct sockaddr_in sin;
    socklen_t  len = sizeof(sin);
    if(getsockname(m_Sock,(struct sockaddr*)&sin,&len) != 0){
        cout << "Get sockname error: " << strerror(errno) << endl;
        exit(-1);
    }
    char hname[128];
    struct hostent *hent;
    gethostname(hname,sizeof(hname));
    hent = gethostbyname(hname);
    ip = string(inet_ntoa(*(struct in_addr*)(hent->h_addr_list[0])));

    return ntohs(sin.sin_port);
}


void CPasvDataTransfer::Execute() {
    int clientsock;
    struct sockaddr_in clientaddr;

    socklen_t len = sizeof(clientaddr);

    clientsock = accept(m_Sock, (struct sockaddr *)&clientaddr, &len);
    m_CliSock = clientsock;
    sem_wait(&m_Signal);
    int ret = DispatchCommand(clientsock);

    close(clientsock);
    delete this;
}

int CPasvDataTransfer::DispatchCommand(int client_fd) {
    vector<string> v;
    SplitString(m_Msg,v,"#");
    if(v[0] == "SENDLIST"){
        if(!v[1].empty()){
            send(m_CliSock,v[1].c_str(),v[1].size(),0);
        }
        else{
            send(m_CliSock,"\r\n",2,0);
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
            if(send(m_CliSock,buffer,n,0) <= 0)
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
        while((n = recv(m_CliSock,buffer,BUFFER_SIZE,0)) > 0){
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

bool CPasvDataTransfer::SetMsg(const string &msg){
    m_Msg = msg;
    return true;
}

bool CPasvDataTransfer::SetControlSock(int sockfd) {
    m_ControlSock = sockfd;
}

ssize_t CPasvDataTransfer::ControlSockSendMsg(const string &msg) {
    return write(m_ControlSock,msg.c_str(),msg.size());
}


bool CPasvDataTransfer::PostSignal(){
    if(0 == sem_post(&m_Signal)){
        return true;
    }
    else{
        return false;
    }
}