这个ftp服务器仅仅是为了了解ftp协议而编写的，整体比较简陋，目前主动和被动模式都已经支持。
其中的 CFtpClient.py 是使用python编写的简陋的客户端测试脚本。

使用了jsoncpp 库，采用源代码的方式进行引用。

目前支持的命令有：
- USER
- PASS
- LIST
- QUIT
- PWD
- CWD
- PASV
- SIZE
- RETR
- STOR
- DELE
- MKD
- RNFR
- RNTO
- RMD
- PORT
