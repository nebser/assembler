#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <string>

class AssemblerExceptions {
   public:
    virtual std::string error() const;
};

class ParserException {
   public:
    ParserException(const std::string& token, int lineNumber)
        : token(token), lineNumber(lineNumber) {}

    std::string error() const {
        return "Invalid token " + token + " at line " + lineNumber;
    }

   private:
    std::string token;
    int lineNumber
};

#endif
