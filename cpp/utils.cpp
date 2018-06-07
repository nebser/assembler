#include "utils.h"
#include <iostream>
#include <locale>
#include <string>

using std::ostream;
using std::string;
using std::toupper;

string Utils::uppercaseString(const string& str) {
    string result = "";
    for (auto c : str) {
        result += toupper(c);
    }
    return result;
}

int Utils::writeData(ostream& os, int data, int size, int currentColumn) {
    for (unsigned char i = 0; i < size; i++) {
        unsigned char d = (data >> i) & 0xFF;
        if (d < 0x0F) {
            os << 0;
        }
        os << std::hex << d;
        currentColumn += 2;
        if (currentColumn % 16) {
            os << std::endl;
        }
    }
    return currentColumn;
}