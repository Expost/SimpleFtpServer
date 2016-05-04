//
// Created by ghost on 16-4-28.
//

#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "CFtpCommand.h"
#include "json/json.h"



bool CCommand::MatchCommand(string inCommand) {
    vector<string> v;
    SplitString(inCommand,v,string(" "));
    bool ret = false;
    unsigned long n = v.size();
    if(n > 0){
        if(m_Command == v[0]){
            ret = true;
            if(n > 1){
                m_Args = v[1];
            }
        }
    }
    return ret;
}

const string& CCommand::GetUserInfo(){
    static string userInfo;
    if(userInfo.empty()){
        FILE *file = fopen("user_info.config","r");
        if(file == NULL){
            cout << "user_info.config file is not exits." << endl;
            exit(-1);
        }
        fseek(file,0,2);
        long len = ftell(file);
        fseek(file,0,0);
        char *buf = new char[len];
        fread(buf,1,len,file);
        fclose(file);
        userInfo = string(buf);
    }
    return userInfo;
}


int CUSERCommand::doWhat(CClient *pClient) {
    if(!m_Args.empty()){
        Json::Value value;
        Json::Reader reader;
        if(reader.parse(CCommand::GetUserInfo(),value)){
            Json::Value user = value["user"];
            for(int i = 0;i < user.size();i++){
                string name = user[i]["name"].asString();
                if(name == m_Args){
                    pClient->SetClientName(name);
                }
            }
        }
        else{
            cout << "Load user info error,now exit." << endl;
            exit(-1);
        }
    }
    string ret = "331 Please specify the password.\r\n";
    pClient->sendMsg(ret);
    pClient->SetClientState(USER);
    return 1;
}



int CPASSCommand::doWhat(CClient *pClient) {
    if(pClient->GetClientState() >= PASS){
        string ret = "230 Already logged in.\r\n";
        pClient->sendMsg(ret);
    }
    else {
        if (!pClient->GetClientName().empty()) {
            if (!m_Args.empty()) {
                Json::Value value;
                Json::Reader reader;
                if (reader.parse(CCommand::GetUserInfo(), value)) {
                    Json::Value user = value["user"];
                    for (int i = 0; i < user.size(); i++) {
                        if(user[i]["name"].asString() == pClient->GetClientName()){
                            string pass = user[i]["pass"].asString();
                            if (pass == m_Args) {
                                string ret = "230 Login successful.\r\n";
                                pClient->SetUserDir(user[i]["rootdir"].asString());
                                pClient->sendMsg(ret);
                                pClient->SetClientState(PASS);
                            }
                            else{
                                string ret = "530 Login incorrect.\r\n";
                                pClient->sendMsg(ret);
                                pClient->SetClientState(USER);
                            }
                        }
                    }
                }
                else {
                    cout << "Load user info error,now exit." << endl;
                    exit(-1);
                }
            }
            else {
                string ret = "530 Login incorrect.\r\n";
                pClient->sendMsg(ret);
                pClient->SetClientState(NOP);
            }
        }
        else {
            string ret = "503 Login with USER first.\r\n";
            pClient->sendMsg(ret);
            pClient->SetClientState(NOP);
        }
    }
    return 1;
}


int CQUITCommand::doWhat(CClient *pClient) {
    pClient->SetClientState(QUIT);
    string ret = "221 Goodbye.\r\n";
    pClient->sendMsg(ret);
    return 1;
}



int CPWDCommand::doWhat(CClient *pClient) {
    if(pClient->GetClientState() >= PASS){
        char dir[BUFFER_SIZE];
        bzero(dir,BUFFER_SIZE);
        sprintf(dir,"257 \"%s\".\r\n", pClient->GetUserDir().c_str());
        pClient->sendMsg(string(dir));
    }
    else{
        string ret = "530 Please login with USER and PASS.\r\n";
        pClient->sendMsg(ret);
    }
    return 1;
}

int CCWDCommand::doWhat(CClient *pClient) {
    if(pClient->GetClientState() >= PASS){
        int r = chdir(pClient->GetUserDir().c_str());
        if(r == 0){
            r = chdir(m_Args.c_str());
            char path[BUFFER_SIZE];
            bzero(path,BUFFER_SIZE);
            if(r == 0){
                string ret = "250 Directory successfully changed.\r\n";
                getcwd(path,BUFFER_SIZE);
                pClient->SetUserDir(string(path));
                pClient->sendMsg(ret);
                return 1;
            }
            else{
                string ret = "550 Failed to change directory.\r\n";
                pClient->sendMsg(ret);
                return -1;
            }
        }
        else{
            cout << "Current user dir is error,now program will exit." << endl;
            exit(-1);
        }
    }
    else{
        string ret = "530 Please login with USER and PASS.\r\n";
        pClient->sendMsg(ret);
        return -1;
    }
}

long CSIZECommand::GetFileSize(CClient *pClient) {
    int r = chdir(pClient->GetUserDir().c_str());
    if(r == 0){
        FILE *file = fopen(m_Args.c_str(),"rb");
        if(file == NULL)
            return -1;
        else {
            fseek(file, 0, 2);
            long size = ftell(file);
            fclose(file);
            return size;
        }
    }
    else{
        cout << "Current user dir is error,now program will exit." << endl;
        exit(-1);
    }
}

int CSIZECommand::doWhat(CClient *pClient) {
    if(pClient->GetClientState() >= PASS){
        char buffer[BUFFER_SIZE];
        bzero(buffer,BUFFER_SIZE);
        long size = GetFileSize(pClient);
        if(size >= 0){
            sprintf(buffer,"213 %ld\r\n",size);
            string ret(buffer);
            pClient->sendMsg(ret);
            return 1;
        }
        else{
            string ret = "550 Could not get file size.\r\n";
            pClient->sendMsg(ret);
            return -1;
        }
    }
    else{
        string ret = "530 Please login with USER and PASS.\r\n";
        pClient->sendMsg(ret);
        return -1;
    }
}

int CPASVCommand::doWhat(CClient *pClient){
    if(pClient->GetClientState() >= PASS){
        if(pClient->GetClientPasvDataTransfer() == NULL){
            pClient->SetClientPasvDataTransfer(new CPasvDataTransfer("0.0.0.0",0));
            pClient->GetClientPasvDataTransfer()->Run();
        }

        string ip;
        int port;
        port = pClient->GetClientPasvDataTransfer()->GetIpAndPort(ip);
        char buffer[BUFFER_SIZE];
        bzero(buffer,BUFFER_SIZE);
        for(int i = 0;i < 3;i++){
            ip.replace(ip.find("."),1,",");
        }
        sprintf(buffer,"227 Entering Passive Mode (%s,%d,%d).\r\n",ip.c_str(),port / 256,port % 256);
        string ret(buffer);
        pClient->sendMsg(ret);
        pClient->SetClientState(PASV);
        return 1;
    }
    else{
        string ret = "530 Please login with USER and PASS.\r\n";
        pClient->sendMsg(ret);
        return -1;
    }
}

bool CLISTCommand::CreateFileDetalInfo(const char *name, char *buffer) {
    struct stat s;
    if(0 != stat(name,&s))
        return false;
    char *pTmp = buffer;
    if(S_ISDIR(s.st_mode))
        *pTmp = 'd';
    else if(S_ISLNK(s.st_mode))
        *pTmp = 'l';
    else
        *pTmp = '-';
    pTmp++;
    if(s.st_mode & S_IRUSR)
        *pTmp = 'r';
    else
        *pTmp = '-';
    pTmp++;
    if(s.st_mode & S_IWUSR)
        *pTmp = 'w';
    else
        *pTmp = '-';
    pTmp++;
    if(s.st_mode & S_IXUSR)
        *pTmp = 'x';
    else
        *pTmp = '-';
    pTmp++;
    if(s.st_mode & S_IRGRP)
        *pTmp = 'r';
    else
        *pTmp = '-';
    pTmp++;
    if(s.st_mode & S_IWGRP)
        *pTmp = 'w';
    else
        *pTmp = '-';
    pTmp++;
    if(s.st_mode & S_IXGRP)
        *pTmp = 'x';
    else
        *pTmp = '-';
    pTmp++;
    if(s.st_mode & S_IROTH)
        *pTmp = 'r';
    else
        *pTmp = '-';
    pTmp++;
    if(s.st_mode & S_IWOTH)
        *pTmp = 'w';
    else
        *pTmp = '-';
    pTmp++;
    if(s.st_mode & S_IXOTH)
        *pTmp = 'x';
    else
        *pTmp = '-';
    pTmp++;
    sprintf(pTmp,"%s"," ");
    pTmp += strlen(pTmp);
    sprintf(pTmp,"%2ld",s.st_nlink);
    pTmp += strlen(pTmp);
    sprintf(pTmp,"%s"," ");
    pTmp += strlen(pTmp);
    sprintf(pTmp,"%4d",s.st_uid);
    pTmp += strlen(pTmp);
    sprintf(pTmp,"%s"," ");
    pTmp += strlen(pTmp);
    sprintf(pTmp,"%4d",s.st_gid);
    pTmp += strlen(pTmp);
    sprintf(pTmp,"%s"," ");
    pTmp += strlen(pTmp);
    sprintf(pTmp,"%6ld",s.st_size);
    pTmp += strlen(pTmp);
    sprintf(pTmp,"%s"," ");
    pTmp += strlen(pTmp);
    sprintf(pTmp,"%s",ctime(&s.st_mtime));
    pTmp += strlen(pTmp);
    pTmp -= 2;
    sprintf(pTmp,"%s"," ");
    pTmp += strlen(pTmp);
    sprintf(pTmp,"%s\r\n",name);
    return true;
}


int CLISTCommand::doWhat(CClient *pClient) {
    if(pClient->GetClientState() >= PASV) {
        string msg = "SENDLIST#";
        chdir(pClient->GetUserDir().c_str());
        DIR *dir = opendir(pClient->GetUserDir().c_str());
        struct dirent *dirp;
        char buffer[BUFFER_SIZE];
        while ((dirp = readdir(dir)) != NULL) {
            bzero(buffer, BUFFER_SIZE);
            if ((strcmp(dirp->d_name, ".") == 0) || (strcmp(dirp->d_name, "..") == 0))
                continue;
            if(dirp->d_name[0] == '.')
                continue;
            bzero(buffer,BUFFER_SIZE);
            CreateFileDetalInfo(dirp->d_name,buffer);

            msg += string(buffer);
        }
        string ret = "150 Here comes the directory listing.\r\n";

        pClient->sendMsg(ret);
        pClient->GetClientPasvDataTransfer()->SetMsg(msg);
        pClient->GetClientPasvDataTransfer()->SetControlSock(pClient->GetClientfd());
        pClient->GetClientPasvDataTransfer()->PostSignal();

        pClient->SetClientState(PASS);
        pClient->SetClientPasvDataTransfer(NULL);
        return 1;
    }
    else{
        string ret = "425 Use PORT or PASV first.\r\n";
        pClient->sendMsg(ret);
        return -1;
    }
}

int CSTORCommand::doWhat(CClient *pClient){
    if(pClient->GetClientState() >= PASV){
        string msg = "RECVFILE#";
        int r = chdir(pClient->GetUserDir().c_str());
        if(r == 0){
            msg += pClient->GetUserDir();
            msg = msg + "#" + m_Args;
            pClient->GetClientPasvDataTransfer()->SetMsg(msg);
            string ret = "150 Ok to send data.\r\n";
            pClient->sendMsg(ret);
            pClient->GetClientPasvDataTransfer()->SetControlSock(pClient->GetClientfd());
            pClient->GetClientPasvDataTransfer()->PostSignal();

            pClient->SetClientState(PASS);
            pClient->SetClientPasvDataTransfer(NULL);
            return 1;
        }
        else{
            cout << "Current user dir is error,now program will exit." << endl;
            exit(-1);
        }
    }
    else{
        string ret = "425 Use PORT or PASV first.\r\n";
        pClient->sendMsg(ret);
        return -1;
    }
}

int CRETRCommand::doWhat(CClient *pClient){
    if(pClient->GetClientState() >= PASV){
        string msg = "SENDFILE#";
        int r = chdir(pClient->GetUserDir().c_str());
        if(r == 0){
            FILE *file = fopen(m_Args.c_str(),"rb");
            if(file != NULL){
                fseek(file,0,2);
                long len = ftell(file);
                fclose(file);
                char buffer[BUFFER_SIZE];
                bzero(buffer,BUFFER_SIZE);
                sprintf(buffer,"150 Opening BINARY mode data connection for %s (%ld bytes).\r\n",
                        m_Args.c_str(),len);

                string ret(buffer);
                pClient->sendMsg(ret);

                msg += pClient->GetUserDir();
                msg = msg + "#" + m_Args;
                pClient->GetClientPasvDataTransfer()->SetMsg(msg);
                pClient->GetClientPasvDataTransfer()->SetControlSock(pClient->GetClientfd());
                pClient->GetClientPasvDataTransfer()->PostSignal();

                pClient->SetClientState(PASS);
                pClient->SetClientPasvDataTransfer(NULL);
                return 1;
            }
            else{
                string ret = "550 Failed to open file.\r\n";
                pClient->sendMsg(ret);
                return -1;
            }
        }
        else{
            cout << "Current user dir is error,now program will exit." << endl;
            exit(-1);
        }
    }
    else{
        string ret = "425 Use PORT or PASV first.\r\n";
        pClient->sendMsg(ret);
        return -1;
    }
};


int CSYSTCommand::doWhat(CClient *pClient) {
    if(pClient->GetClientState() >= PASS) {
        string ret = "215 UNIX Type: L8\r\n";
        pClient->sendMsg(ret);
        return 1;
    }
    else{
        string ret = "530 Please login with USER and PASS.\r\n";
        pClient->sendMsg(ret);
        return -1;
    }
}

int CTYPECommand::doWhat(CClient *pClient) {
    if(pClient->GetClientState() >= PASS){
        if(m_Args == "I"){
            string ret = "200 Switching to Binary mode.\r\n";
            pClient->sendMsg(ret);
            return 1;
        }
        else if(m_Args == "A"){
            string ret = "200 Switching to ASCII mode.\r\n";
            pClient->sendMsg(ret);
            return 1;
        }
        // there should have some orther type mode.
        else{
            string ret = "500 Unrecognised TYPE command.\r\n";
            pClient->sendMsg(ret);
            return -1;
        }
    }
    else{
        string ret = "530 Please login with USER and PASS.\r\n";
        pClient->sendMsg(ret);
        return -1;
    }
}

int CDELECommand::doWhat(CClient *pClient) {
    if(pClient->GetClientState() >= PASS){
        chdir(pClient->GetUserDir().c_str());
        FILE *file = fopen(m_Args.c_str(),"rb");
        if(file != NULL){
            fclose(file);
            if(0 == remove(m_Args.c_str())){
                string ret = "250 Delete operation successful.\r\n";
                pClient->sendMsg(ret);
                return 1;
            }
        }
        string ret = "550 Delete operation failed.\r\n";
        pClient->sendMsg(ret);
        return -1;
    }
    else{
        string ret = "530 Please login with USER and PASS.\r\n";
        pClient->sendMsg(ret);
        return -1;
    }
}

int CMKDCommand::doWhat(CClient *pClient) {
    if(pClient->GetClientState() >= PASS){
        chdir(pClient->GetUserDir().c_str());
        if(0 == mkdir(m_Args.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)){
            chdir(m_Args.c_str());
            char buffer[BUFFER_SIZE];
            bzero(buffer,BUFFER_SIZE);
            getcwd(buffer,BUFFER_SIZE);
            string ret = "257 \"" + string(buffer) + "\" created.\r\n";
            pClient->sendMsg(ret);
            return 1;
        }
        else {
            string ret = "550 Create directory operation failed.\r\n";
            pClient->sendMsg(ret);
            return -1;
        }
    }
    else{
        string ret = "530 Please login with USER and PASS.\r\n";
        pClient->sendMsg(ret);
        return -1;
    }
}

int CRMDCommand::doWhat(CClient *pClient) {
    if(pClient->GetClientState() >= PASS){
        chdir(pClient->GetUserDir().c_str());
        if(0 == rmdir(m_Args.c_str())){
            string ret = "250 Remove directory operation successful.\r\n";
            pClient->sendMsg(ret);
            return 1;
        }
        else{
            string ret = "550 Remove directory operation failed.\r\n";
            pClient->sendMsg(ret);
            return -1;
        }
    }
    else{
        string ret = "530 Please login with USER and PASS.\r\n";
        pClient->sendMsg(ret);
        return -1;
    }
}


int CRNFRCommand::doWhat(CClient *pClient) {
    if(pClient->GetClientState() >= PASS){
        chdir(pClient->GetUserDir().c_str());
        FILE *file = fopen(m_Args.c_str(),"r");
        if(file != NULL){
            fclose(file);
            string ret = "350 Ready for RNTO.\r\n";
            pClient->sendMsg(ret);
            pClient->SetClientLastFileName(m_Args);
            pClient->SetClientState(RNFR);
            return 1;
        }
        else{
            string ret = "550 RNFR command failed.\r\n";
            pClient->sendMsg(ret);
            return -1;
        }
    }
    else{
        string ret = "530 Please login with USER and PASS.\r\n";
        pClient->sendMsg(ret);
        return -1;
    }
}

int CRNTOCommand::doWhat(CClient *pClient) {
    if(pClient->GetClientState() >= RNFR) {
        chdir(pClient->GetUserDir().c_str());
        if(0 == rename(pClient->GetClientLastFileName().c_str(),m_Args.c_str())){
            string ret = "250 Rename successful.\r\n";
            pClient->sendMsg(ret);
            pClient->SetClientState(PASS);
            return 1;
        }
        else{
            string ret = "550 Rename failed.\r\n";
            pClient->sendMsg(ret);
            pClient->SetClientState(PASS);
            return -1;
        }
    }
    else if(pClient->GetClientState() >= PASS){
        string ret = "503 RNFR required first.\r\n";
        pClient->sendMsg(ret);
        return -1;
    }
    else{
        string ret = "530 Please login with USER and PASS.\r\n";
        pClient->sendMsg(ret);
        return -1;
    }
}

