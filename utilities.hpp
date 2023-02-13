#include <iostream>
#include <cstdint>
#include <cstring>

class Utility
{
public:
    static char* getSubStr(char* str, int pos, size_t len = 0) {
        len = (len == 0) ? strlen(str) - pos : len;
        char *substr = (char *)malloc(sizeof(char) * (len + 1));
        memset(substr, 0, len + 1);
        std::memcpy(substr, str + pos, len);
        return substr;
    }

private:


};