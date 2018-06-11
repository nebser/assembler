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
    while (newLocationCounter % padd) {
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

// NOTE: insert immediate address checking if necessary
Instruction& SingleAddressInstruction::decode(TokenStream& tokenStream) {
    vector<Token> operandTokens;
    while (!tokenStream.end()) {
        auto t = tokenStream.next();
        if (t.getType() == Token::LINE_DELIMITER) {
            operand = Operand(operandTokens);
            if (dstExists && (operand.getAddressMode() == IMMEDIATE_CONSTANT ||
                              operand.getAddressMode() == IMMEDIATE_SYMBOL)) {
                throw DecodingException(
                    "dst argument for instruction " + name +
                    " can't be used with immediate addressing");
            }
            return *this;
        }
        operandTokens.push_back(t);
    }
    throw DecodingException("Invalid end of instruction " + name);
}

int SingleAddressInstruction::write(ostream& os, int currentColumn) const {
    unsigned int data =
        opcode << 26 |
        (dstExists ? operand.getRegData() << 21 : operand.getRegData() << 16) |
        (0xFF & operand.getConstantData());
    return Utils::writeInstruction(os, data, getSize() / 8, currentColumn);
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
    auto dstSize = dst.getSize();
    auto srcSize = src.getSize();
    unsigned int data =
        opcode << 26 | dst.getRegData() << 21 | src.getRegData() << 16 |
        (0xFF &
         (srcSize > dstSize ? src.getConstantData() : dst.getConstantData()));
    return Utils::writeInstruction(os, data, (dstSize + srcSize + 11) / 4,
                                   currentColumn);
}

Instruction& JmpInstruction::decode(TokenStream& tokenStream) {
    vector<Token> operandTokens;
    while (!tokenStream.end()) {
        auto t = tokenStream.next();
        if (t.getType() == Token::LINE_DELIMITER) {
            break;
        }
        operandTokens.push_back(t);
    }
    if (tokenStream.end()) {
        throw DecodingException("Invalid end of file");
    }
    operand = Operand(operandTokens);
    opcode = operand.getAddressMode() != PC_RELATIVE ? 0x13 : 0x00;
    return *this;
}

int JmpInstruction::write(ostream& os, int currentColumn) const {
    auto operandSize = operand.getSize();
    unsigned int data = prefix << 30 | opcode << 26 | 15 << 21 |
                        operand.getRegData() << 16 |
                        (0xFF & operand.getConstantData());
    return Utils::writeInstruction(os, data, (operandSize + 11) / 8,
                                   currentColumn);
}