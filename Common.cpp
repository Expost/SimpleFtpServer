//
// Created by ghost on 16-4-28.
//

#include "Common.h"

char *_strcat(char *pBuffer, const char c) {
    char *p = pBuffer;
    while(*p++ != '\0');
    *p-- = '\0';
    *p = c;
    return pBuffer;
}

void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
    std::string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while(std::string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2-pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if(pos1 != s.length())
        v.push_back(s.substr(pos1));
}
