#ifndef TOKEN_H_
#define TOKEN_H_

#include <string>

class Token {
   public:
    enum Type {
        IDENTIFICATOR,
        DEC_NUMBER,
        HEX_NUMBER,
        BIN_NUMBER,
        PC_RELATIVE_QUANT,
        IMMEDIATE_QUANT,
        COMMA,
        OPEN_BRACKETS,
        CLOSED_BRACKETS,
        LABEL,
        LINE_DELIMITER,
        UNDEFINED
    };

    Token(Type type, const std::string& value) : type(type), value(value) {}

    Type getType() const { return type; }

    std::string getValue() const { return value; }

    std::string getTypeDescription() const {
        switch (type) {
            case IDENTIFICATOR:
                return "Regular identificator";
            case DEC_NUMBER:
                return "Signed decimal number";
            case HEX_NUMBER:
                return "Signed hexadecimal number";
            case BIN_NUMBER:
                return "Signed binary number";
            case PC_RELATIVE_QUANT:
                return "PC relative addressing quantificator ($)";
            case IMMEDIATE_QUANT:
                return "Immediate addressing quantificator (&)";
            case COMMA:
                return "Comma (,)";
            case OPEN_BRACKETS:
                return "Open brackets sign ([)";
            case CLOSED_BRACKETS:
                return "Closed brackets sign(])";
            case LABEL:
                return "Label identificator";
            case LINE_DELIMITER:
                return "New line character";
            case UNDEFINED:
                return "Undefined token";
        }
    }

    friend bool operator==(const Token& firstToken, const Token& secondToken) {
        return firstToken.type == secondToken.type &&
               firstToken.value == secondToken.value;
    }

   private:
    Type type;
    std::string value;
};

const Token UNDEFINED_TOKEN = Token(Token::UNDEFINED, "");

#endif