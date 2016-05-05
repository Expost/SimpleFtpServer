//
// Created by ghost on 16-4-28.
//

#ifndef SIMPLEFTPSERVER_CFTPCOMMAND_H
#define SIMPLEFTPSERVER_CFTPCOMMAND_H

#include "Common.h"
#include "CClient.h"


class CCommand {
protected:
    string m_Command;
    string m_Args;

    static const string& GetUserInfo();
public:

    virtual bool MatchCommand(string inCommand);
    virtual int doWhat(CClient *pClient) = 0;
};



class CUSERCommand:public CCommand {
private:

public:
    CUSERCommand(){m_Command = "USER";}
    int doWhat(CClient *pClient);

};


class CPASSCommand:public CCommand{
private:

public:
    CPASSCommand(){m_Command = "PASS";}
    int doWhat(CClient *pClient);
};

class CLISTCommand:public CCommand{
private:
    bool CreateFileDetalInfo(const char *name,char *buffer);
    bool GetDirInfo(CClient *pClient,string &msg);
public:
    CLISTCommand(){m_Command = "LIST";}
    int doWhat(CClient *pClient);
};


class CQUITCommand:public CCommand{
private:

public:
    CQUITCommand(){m_Command = "QUIT";}
    int doWhat(CClient *pClient);
};

class CPWDCommand:public CCommand{
private:

public:
    CPWDCommand(){m_Command = "PWD";}
    int doWhat(CClient *pClient);
};


class CCWDCommand:public CCommand{
private:

public:
    CCWDCommand(){m_Command = "CWD";}
    int doWhat(CClient *pClient);
};

class CPASVCommand:public CCommand{
private:

public:
    CPASVCommand(){m_Command = "PASV";}
    int doWhat(CClient *pClient);
};


class CSIZECommand:public CCommand{
private:
    long GetFileSize(CClient *pClient);
public:
    CSIZECommand(){m_Command = "SIZE";}
    int doWhat(CClient *pClient);
};



class CRETRCommand:public CCommand{
private:

public:
    CRETRCommand(){m_Command = "RETR";}
    int doWhat(CClient *pClient);
};

class CSYSTCommand:public CCommand{
private:

public:
    CSYSTCommand(){m_Command = "SYST";}
    int doWhat(CClient *pClient);
};


class CTYPECommand:public CCommand{
private:

public:
    CTYPECommand(){m_Command = "TYPE";}
    int doWhat(CClient *pClient);
};

class CSTORCommand:public CCommand{
private:

public:
    CSTORCommand(){m_Command = "STOR";}
    int doWhat(CClient *pClient);
};

class CDELECommand:public CCommand{
private:

public:
    CDELECommand(){m_Command = "DELE";}
    int doWhat(CClient *pClient);
};

class CMKDCommand:public CCommand{
private:

public:
    CMKDCommand(){m_Command = "MKD";}
    int doWhat(CClient *pClient);
};

class CRNFRCommand:public CCommand{
private:

public:
    CRNFRCommand(){m_Command = "RNFR";}
    int doWhat(CClient *pClient);
};

class CRNTOCommand:public CCommand{
private:

public:
    CRNTOCommand(){m_Command = "RNTO";}
    int doWhat(CClient *pClient);
};

class CRMDCommand:public CCommand{
private:

public:
    CRMDCommand(){m_Command = "RMD";}
    int doWhat(CClient *pClient);
};

class CPORTCommand:public CCommand{
private:

public:
    CPORTCommand(){m_Command = "PORT";}
    int doWhat(CClient *pClient);
};




#endif //SIMPLEFTPSERVER_CFTPCOMMAND_H
