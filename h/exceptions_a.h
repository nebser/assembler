#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <string>
#include "utils.h"

class AssemblerExceptions {
   public:
    virtual std::string error() const;
};

class ParserException {
   public:
    ParserException(const std::string& token, int lineNumber)
        : token(token), lineNumber(lineNumber) {}

    std::string error() const {
        return "Invalid token " + token + " at line " +
               Utils::convertToString(lineNumber);
    }

   private:
    std::string token;
    int lineNumber;
};

class StreamException {
   public:
    std::string error() const { return "End of the stream reached"; }
}

#endif
