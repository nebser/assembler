#ifndef UTILS_H_
#define UTILS_H_

#include <sstream>

class Utils {
   public:
    static std::string convertToString(int input) {
        std::stringstream out;
        out << input;
        return out.str();
    }
};
#endif