#ifndef __LSTR_H__
#define __LSTR_H__

#include <iostream>
#include <sstream>

namespace lstr {
    char unescapeChar(std::string str, bool &didEscape, unsigned long index) {
        didEscape = true;
        if (str[index] == '\\') {
            switch (str[index+1]) {
                case 'a':
                    return '\a';
                case 'b':
                    return '\b';
                case 'r':
                    return '\r';
                case 't':
                    return '\t';
                case 'v':
                    return '\v';
                case 'n':
                    return '\n';
                case '\\':
                    return '\\';
                case '"':
                    return '"';
                case '\'':
                    return '\'';
                case '0':
                    return '\0';
                default:
                    didEscape = false;
                    return -1;
            }
        }

        didEscape = false;
        return -1;
    }

    std::string unescape(std::string src) {
        std::stringstream strstream;

        for (unsigned long i = 0; i < src.size(); i++) {
            bool didEscape;
            char unescaped = unescapeChar(src, didEscape, i);

            if (didEscape) {
                i++;
                strstream << unescaped;
            } else {
                strstream << src[i];
            }
        }

        return strstream.str();
    }
}

#endif 
