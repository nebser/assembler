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

    static int writeData(std::ostream&, int data, int size, int currentColumn);
};
#endif