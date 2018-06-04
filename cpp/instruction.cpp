#include "instruction.h"
#include <iostream>
#include "tokenizer.h"
using std::ostream;

WritableDirective* Definition::decode(TokenStream& tokenStream) {
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
        return Utils::writeData(os, 0, multiplier, currentColumn);
    } else {
        for (auto&& data : datas) {
            currentColumn =
                Utils::writeData(os, data, multiplier, currentColumn);
        }
    }
    return currentColumn;
}

WritableDirective* SkipDirective::decode(TokenStream& tokenStream) {
    auto firstToken = tokenStream.next();
    auto type = firstToken.getType();
    if (type != Token::HEX_NUMBER && type != Token::BIN_NUMBER &&
        type != Token::DEC_NUMBER) {
        throw DecodingException(
            "Size parameter of the skip directive must be a number");
    }
    auto secondToken = tokenStream.next();
    if (secondToken.getType() == Token::LINE_DELIMITER) {
        return this;
    }
    if (secondToken.getType() != Token::COMMA) {
        throw DecodingException(
            "Format of the .skip directive must be .skip size, [fill]");
    }
    size = firstToken.getIntValue();
    firstToken = tokenStream.next();
    secondToken = tokenStream.next();
    if (type != Token::HEX_NUMBER && type != Token::BIN_NUMBER &&
        type != Token::DEC_NUMBER) {
        throw DecodingException(
            "Fill parameter of the skip directive must be a number");
    }
    if (secondToken.getType() == Token::LINE_DELIMITER) {
        return this;
    }
    throw DecodingException(
        "Format of the .skip directive must be .skip size, [fill]");
}

int SkipDirective::write(ostream& os, int currentColumn) const {
    for (int i = 0; i < size; i++) {
        Utils::writeData(os, fill, 1, currentColumn);
    }
}