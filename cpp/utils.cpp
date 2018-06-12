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

int Utils::writeData(ostream& os, unsigned int data, int size,
                     int currentColumn) {
    for (unsigned char i = 0; i < size; i++) {
        unsigned char d = (data >> i) & 0xFF;
        currentColumn = writeByte(os, d, currentColumn);
    }
    return currentColumn;
}

int Utils::writeInstruction(ostream& os, unsigned int data, int size,
                            int currentColumn) {
    auto opcodeSize = 2;
    for (unsigned char i = 0; i < opcodeSize; i++) {
        unsigned char d = data >> (8 * (3 - i));
        currentColumn = writeByte(os, d, currentColumn);
    }
    for (unsigned char i = 0; i < size - opcodeSize; i++) {
        unsigned char d = data >> (8 * i);
        currentColumn = writeByte(os, d, currentColumn);
    }
    return currentColumn;
}