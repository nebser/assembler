#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <string>
#include "utils.h"

class AssemblerException {
   public:
    virtual std::string error() const = 0;
};

class ParserException : public AssemblerException {
   public:
    ParserException(const std::string& token, int lineNumber)
        : token(token), lineNumber(lineNumber) {}

    std::string error() const override {
        return "Invalid token " + token + " at line " +
               Utils::convertToString(lineNumber);
    }

   private:
    std::string token;
    int lineNumber;
};

class StreamException : public AssemblerException {
   public:
    std::string error() const override { return "End of the stream reached"; }
};

class SystemException : public AssemblerException {
   public:
    SystemException(const std::string& error) : errorReason(error) {}

    std::string error() const override {
        return "System error: " + errorReason;
    }

   private:
    std::string errorReason;
};

#endif
