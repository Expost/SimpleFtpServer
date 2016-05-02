
#include "CFtpTransfer.h"



int main() {
    CFtpTransfer t("0.0.0.0",5000);
    t.run();
    return 0;
}