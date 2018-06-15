#include "instruction.h"
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
        datas.push_back(Operand(
            firstToken, {REG_DIRECT, REG_INDIRECT_W_DISPL, MEMORY_CONSTANT,
                         MEMORY_SYMBOL, PC_RELATIVE, PSW}));
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

vector<RelocationData> Definition::evaluate(const SymbolTable& symbolTable,
                                            int address,
                                            const std::string& section) {
    vector<RelocationData> relData;
    auto cnt = 0;
    auto size = getSize();
    for (auto&& data : datas) {
        auto displ = cnt * multiplier;
        auto r = data.evaluate(symbolTable, address + displ, address + size,
                               section);
        if (r != nullptr) {
            relData.push_back(*r);
            delete r;
        }
        cnt++;
    }
    return relData;
}

int Definition::write(ostream& os, int currentColumn) const {
    if (datas.size() == 0) {
        return Utils::writeData(os, 0, multiplier, currentColumn);
    } else {
        for (auto&& data : datas) {
            currentColumn = Utils::writeData(os, data.getFullConstantData(),
                                             multiplier, currentColumn);
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
            operand = new Operand(operandTokens,
                                  {IMMEDIATE_CONSTANT, IMMEDIATE_SYMBOL});
            return *this;
        }
        operandTokens.push_back(t);
    }
    throw DecodingException("Invalid end of instruction " + name);
}

int SingleAddressInstruction::write(ostream& os, int currentColumn) const {
    unsigned int data = opcode << 26 |
                        (dstExists ? operand->getRegData() << 21
                                   : operand->getRegData() << 16) |
                        operand->getConstantData();
    return Utils::writeInstruction(os, data, getSize() / 8, currentColumn);
}

Instruction& DoubleAddressInstruction::decode(TokenStream& tokenStream) {
    vector<Token> dstTokens;
    vector<Token> srcTokens;
    while (!tokenStream.end()) {
        auto t = tokenStream.next();
        if (t.getType() == Token::COMMA) {
            dst =
                new Operand(dstTokens, {IMMEDIATE_CONSTANT, IMMEDIATE_SYMBOL});
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
            src = new Operand(srcTokens);
            if (src->getSize() + dst->getSize() > 26) {
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
    auto dstSize = dst->getSize();
    auto srcSize = src->getSize();
    unsigned int data =
        opcode << 26 | dst->getRegData() << 21 | src->getRegData() << 16 |
        (srcSize > dstSize ? src->getConstantData() : dst->getConstantData());
    // os << "Name " << name << dstSize << " " << srcSize << std::endl;
    return Utils::writeInstruction(os, data, (dstSize + srcSize + 6) / 8,
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
    operand =
        new Operand(operandTokens, {IMMEDIATE_CONSTANT, IMMEDIATE_SYMBOL});
    opcode = operand->getAddressMode() == PC_RELATIVE ? 0x00 : 0x0D;
    return *this;
}

int JmpInstruction::write(ostream& os, int currentColumn) const {
    auto operandSize = operand->getSize();
    unsigned int data = prefix << 30 | opcode << 26 | 15 << 21 |
                        operand->getRegData() << 16 |
                        operand->getConstantData();
    return Utils::writeInstruction(os, data, (operandSize + 11) / 8,
                                   currentColumn);
}