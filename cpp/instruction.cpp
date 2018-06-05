#include "instruction.h"
#include <iostream>
#include "tokenizer.h"
using std::ostream;

WritableDirective& Definition::decode(TokenStream& tokenStream) {
    auto firstToken = tokenStream.next();
    if (firstToken.getType() == Token::LINE_DELIMITER) {
        return *this;
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
            return *this;
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

WritableDirective& SkipDirective::decode(TokenStream& tokenStream) {
    auto firstToken = tokenStream.next();
    auto type = firstToken.getType();
    if (type != Token::HEX_NUMBER && type != Token::BIN_NUMBER &&
        type != Token::DEC_NUMBER) {
        throw DecodingException(
            "Size parameter of the skip directive must be a number");
    }
    size = firstToken.getIntValue();
    auto secondToken = tokenStream.next();
    if (secondToken.getType() == Token::LINE_DELIMITER) {
        return *this;
    }
    if (secondToken.getType() != Token::COMMA) {
        throw DecodingException(
            "Format of the .skip directive must be .skip size, [fill]");
    }
    firstToken = tokenStream.next();
    secondToken = tokenStream.next();
    auto type = firstToken.getType();
    if (type != Token::HEX_NUMBER && type != Token::BIN_NUMBER &&
        type != Token::DEC_NUMBER) {
        throw DecodingException(
            "Fill parameter of the skip directive must be a number");
    }
    fill = firstToken.getIntValue();
    if (secondToken.getType() == Token::LINE_DELIMITER) {
        return *this;
    }
    throw DecodingException(
        "Format of the .skip directive must be .skip size, [fill]");
}

int SkipDirective::write(ostream& os, int currentColumn) const {
    auto newColumn = currentColumn;
    for (int i = 0; i < size; i++) {
        newColumn = Utils::writeData(os, fill, 1, newColumn);
    }
    return newColumn;
}

AlignDirective& AlignDirective::decode(TokenStream& tokenStream) {
    auto firstToken = tokenStream.next();
    auto type = firstToken.getType();
    if (type != Token::BIN_NUMBER && type != Token::DEC_NUMBER &&
        type != Token::HEX_NUMBER) {
        throw DecodingException("Padd must be an int value");
    }
    auto secondToken = tokenStream.next();
    padd = firstToken.getIntValue();
    if (secondToken.getType() == Token::LINE_DELIMITER) {
        return *this;
    }
    if (secondToken.getType() != Token::COMMA) {
        throw DecodingException(
            "Align directive must have a format .align "
            "padd[,][fill][,][max_padd]");
    }
    auto middleToken = tokenStream.next();
    type = middleToken.getType();
    if (type == Token::BIN_NUMBER || type == Token::DEC_NUMBER ||
        type == Token::HEX_NUMBER) {
        fill = middleToken.getIntValue();
        auto dummyToken = tokenStream.next();
        if (dummyToken.getType() == Token::LINE_DELIMITER) {
            return *this;
        }
        if (dummyToken.getType() != Token::COMMA) {
            throw DecodingException(
                "Align directive must have a format .align "
                "padd[,][fill][,][max_padd]");
        }

    } else {
        if (type != Token::COMMA) {
            throw DecodingException(
                "Fill part of the .align directive must be an int value");
        }
    }
    firstToken = tokenStream.next();
    type = firstToken.getType();
    if (type != Token::BIN_NUMBER && type != Token::DEC_NUMBER &&
        type != Token::HEX_NUMBER) {
        throw DecodingException("Padd must be an int value");
    }
    maxPadd = firstToken.getIntValue();
    secondToken = tokenStream.next();
    if (secondToken.getType() == Token::LINE_DELIMITER) {
        return *this;
    }
    throw DecodingException(
        "Fill part of the .align directive must be an int value");
}

AlignDirective& AlignDirective::evaluate(int currentLocationCounter) {
    auto newLocationCounter = currentLocationCounter;
    while (currentLocationCounter % padd) {
        newLocationCounter++;
    }
    auto calculatedPad = newLocationCounter - currentLocationCounter;
    size = calculatedPad > maxPadd ? 0 : calculatedPad;
}

int AlignDirective::write(ostream& os, int currentColumn) const {
    auto newColumn = currentColumn;
    for (int i = 0; i < size; i++) {
        newColumn = Utils::writeData(os, fill, 1, newColumn);
    }
    return newColumn;
}