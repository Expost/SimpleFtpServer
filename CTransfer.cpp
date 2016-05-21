//
// Created by ghost on 16-4-26.
//


#include "CTransfer.h"

CTransfer::CTransfer(std::string ip,int port)
        :port(port),ip(ip),maxi(0),nready(0){
    bzero(&sockaddr,sizeof(sockaddr));
    sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    sockaddr.sin_port = htons(port);
    sockaddr.sin_family = AF_INET;

    for(int i = 0;i < OPEN_MAX;i++){
        pollfds[i].fd = -1;
    }
}

CTransfer::~CTransfer(){
    close(sock);
}



bool CTransfer::BindListen() {
    sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock == -1){
        perror("control socket create error:");
        return false;
    }
    pollfds[0].fd = sock;
    pollfds[0].events = POLLIN;

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt));

    if(bind(sock,(struct sockaddr*)&sockaddr,sizeof(sockaddr)) == -1){
        perror("bind error:");
        return false;
    }

    if(listen(sock,10) == -1){
        perror("listen error:");
        return false;
    }

    return true;
}

void CTransfer::run(){
    if(!this->BindListen()){
        exit(-1);
    }

    while(1){
        //cout << "start poll." << endl;
        nready = poll(pollfds,maxi + 1,-1);
        //cout << "end poll and nread is: " << nready << endl;
        if(nready == -1){
            perror("poll error:");
            exit(-1);
        }
        if(pollfds[0].revents & POLLIN){
            struct sockaddr_in cliaddr;
            socklen_t cliaddrlen = sizeof(cliaddr);
            bzero(&cliaddr,cliaddrlen);
            int clisockfd;
            //cout << "maxi is: " << maxi << " and max_con is: " << max_connections << endl;
            if((clisockfd = accept(sock,(struct sockaddr*)&cliaddr,&cliaddrlen)) == -1) {
                if(errno == EINTR){
                    continue;
                }
                else{
                    perror("accept error:");
                    exit(-1);
                }
            }
            if(Accept(clisockfd, cliaddr) == 1){
                int i = 0;
                for(i = 1;i < OPEN_MAX;i++){
                    if(pollfds[i].fd < 0){
                        pollfds[i].fd = clisockfd;
                        pollfds[i].events = POLLIN;
                        break;
                    }
                }
                if(i == OPEN_MAX){
                    fprintf(stderr,"too many clients.\n");
                    exit(-1);
                }
                maxi = (i > maxi)?i:maxi;
            }
            else{
                close(clisockfd);
            }

            if(--nready <= 0)
                continue;
        }
        for(int i = 1;i <= maxi;i++){
            if(pollfds[i].fd < 0)
                continue;
            //cout << "fd is " << pollfds[i].fd << " and revent is " << pollfds[i].revents << endl;
            if(pollfds[i].revents & POLLIN){
                Receive(pollfds[i].fd);
            }
            if(pollfds[i].revents & POLLNVAL){
                pollfds[i].fd = -1;
            }

        }
    }
}
