#include "utils.h"
#include <locale>
#include <string>

using std::string;
using std::toupper;

string Utils::uppercaseString(const string& str) {
    string result = "";
    for (auto c : str) {
        result += toupper(c);
    }
    return result;
}