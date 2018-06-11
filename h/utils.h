#ifndef UTILS_H_
#define UTILS_H_

#include <iostream>
#include <sstream>
#include <string>

class Utils {
   public:
    static std::string convertToString(int input) {
        std::stringstream out;
        out << input;
        return out.str();
    }

    static std::string uppercaseString(const std::string&);

    static int writeData(std::ostream&, unsigned int data, int size,
                         int currentColumn);

    static int writeInstruction(std::ostream&, unsigned int data, int size,
                                int currentColumn);

    static int writeByte(std::ostream& os, unsigned char d, int currentColumn) {
        if (d < 0x0F) {
            os << 0;
        }
        os << std::hex << d;
        currentColumn += 2;
        if (currentColumn % 16) {
            os << std::endl;
        }
        return currentColumn;
    }
};
#endif