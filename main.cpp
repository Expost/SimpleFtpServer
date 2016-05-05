
#include "CFtpTransfer.h"



int main(int argc,char **argv) {
    if(argc != 1){
        cout << "Please user SimpleFtpServer [ip] [port] to execute this program." << endl;
        return -1;
    }
    //CFtpTransfer t(argv[1],atoi(argv[2]));
    CFtpTransfer t("0.0.0.0",5000);
    t.run();
    return 0;
}