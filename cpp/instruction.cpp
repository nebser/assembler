#include "instruction.h"
#include <iostream>
#include "tokenizer.h"
using std::ostream;

Definition* Definition::decode(TokenStream& tokenStream) {
    auto firstToken = tokenStream.next();
    if (firstToken.getType() == Token::LINE_DELIMITER) {
        return this;
    }
    auto secondToken = tokenStream.next();
    while (true) {
        auto t = firstToken.getType();
        if (t != Token::BIN_NUMBER && t != Token::DEC_NUMBER &&
            t != Token::HEX_NUMBER) {
            throw DecodingException(
                "Data can be initialized only with numbers");
        }
        datas.push_back(firstToken.getIntValue());
        if (secondToken.getType() == Token::LINE_DELIMITER) {
            return this;
        }
        if (secondToken.getType() != Token::COMMA) {
            throw DecodingException("Initializing values can only be numbers");
        }
        firstToken = tokenStream.next();
        secondToken = tokenStream.next();
    }
}

int Definition::write(ostream& os, int currentColumn) const {
    if (datas.size() == 0) {
        for (unsigned char i = 0; i < multiplier; i++) {
            unsigned char d = 0;
            os << std::hex << d;
            currentColumn++;
            if (currentColumn % 16) {
                os << std::endl;
            }
        }
    } else {
        for (auto&& data : datas) {
            for (unsigned char i = 0; i < multiplier; i++) {
                unsigned char d = (data >> i) & 0xFF;
                os << std::hex << d;
                currentColumn++;
                if (currentColumn % 16) {
                    os << std::endl;
                }
            }
        }
    }
    return currentColumn;
}