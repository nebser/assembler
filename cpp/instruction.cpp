#include "instruction.h"
#include <iostream>
#include <vector>
#include "token.h"
#include "tokenizer.h"
using std::ostream;
using std::vector;

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
    type = firstToken.getType();
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
    return *this;
}

int AlignDirective::write(ostream& os, int currentColumn) const {
    auto newColumn = currentColumn;
    for (int i = 0; i < size; i++) {
        newColumn = Utils::writeData(os, fill, 1, newColumn);
    }
    return newColumn;
}

Instruction& SingleAddressInstruction::decode(TokenStream& tokenStream) {
    vector<Token> operandTokens;
    while (!tokenStream.end()) {
        auto t = tokenStream.next();
        if (t.getType() == Token::LINE_DELIMITER) {
            operand = Operand(operandTokens);
            return *this;
        }
        operandTokens.push_back(t);
    }
    throw DecodingException("Invalid end of instruction " + name);
}

int SingleAddressInstruction::write(ostream& os, int currentColumn) const {
    auto operandCode = operand.getCode();
    auto operandSize = operand.getSize();
    unsigned int data =
        opcode << (operandSize + 5) | (operandCode << operandSize);
    if (operandSize > 5) {
        data = (data & 0xFFE00000) | (operandCode & ~0xFF);
    }
    return Utils::writeData(os, data, getSize() / 8, currentColumn);
}

Instruction& DoubleAddressInstruction::decode(TokenStream& tokenStream) {
    vector<Token> dstTokens;
    vector<Token> srcTokens;
    while (!tokenStream.end()) {
        auto t = tokenStream.next();
        if (t.getType() == Token::COMMA) {
            dst = Operand(dstTokens);
            break;
        }
        dstTokens.push_back(t);
    }
    if (tokenStream.end()) {
        throw DecodingException("Invalid dst operand for instruction " + name);
    }
    while (!tokenStream.end()) {
        auto t = tokenStream.next();
        if (t.getType() == Token::LINE_DELIMITER) {
            src = Operand(srcTokens);
            if (src.getSize() + dst.getSize() > 26) {
                throw DecodingException(
                    "Only one operand can have additional data for operands");
            }
            return *this;
        }
        srcTokens.push_back(t);
    }
    throw DecodingException("Invalid src operand for instruction " + name);
}

int DoubleAddressInstruction::write(ostream& os, int currentColumn) const {
    auto dstCode = dst.getCode();
    auto dstSize = dst.getSize();
    auto srcCode = src.getCode();
    auto srcSize = src.getSize();
    unsigned int data = opcode << (dstCode + 5) |
                        (dstCode << (dstSize > srcSize ? dstSize : srcSize));
    auto size = 4;
    if (dstSize > 5) {
        data = opcode << (dstSize + srcSize) | ((dstCode & ~0xFF) << 21) |
               srcCode << 16 | (dstCode & 0xFF);
    } else {
        if (srcSize > 5) {
            data = opcode << (dstSize + srcSize) | dstCode << 21 |
                   ((srcCode & ~0xFF) << 16) | (srcCode & 0xFF);
        } else {
            data = opcode << (dstSize + srcSize) | dstCode << dstSize | srcCode;
            size = 2;
        }
    }
    return Utils::writeData(os, data, size, currentColumn);
}