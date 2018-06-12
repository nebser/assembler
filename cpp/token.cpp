#include "token.h"
#include <string>
#include <vector>
using std::string;
using std::vector;

string Token::joinTokens(const vector<Token>& tokens) {
    string sum = "";
    for (auto&& t : tokens) {
        sum += t.getValue();
    }
    return sum;
}