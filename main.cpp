
#include "CFtpTransfer.h"



int main(int argc,char **argv) {
    if(argc != 4){
        cout << "Please user SimpleFtpServer [ip] [port] [max_connection] to execute this program." << endl;
        return -1;
    }
    CFtpTransfer t(argv[1],atoi(argv[2]),atoi(argv[3]));
    //CFtpTransfer t("0.0.0.0",5000,2);
    t.run();
    return 0;
}