#include "operand.h"
#include <string>
#include <vector>
#include "data.h"
#include "token.h"
#include "utils.h"
using std::string;
using std::vector;

vector<Operand::Registry> Operand::constructRegistries() {
    vector<Registry> r;
    r.push_back(Registry("r0", 0));
    r.push_back(Registry("r1", 1));
    r.push_back(Registry("r2", 2));
    r.push_back(Registry("r3", 3));
    r.push_back(Registry("r4", 4));
    r.push_back(Registry("r5", 5));
    r.push_back(Registry("r6", 6));
    r.push_back(Registry("r7", 7));
    return r;
}

const std::vector<Operand::Registry> Operand::registries =
    constructRegistries();

int Operand::getRegistry(const string& name) {
    auto pos = 0;
    for (auto&& r : registries) {
        if (r.name == name) {
            return pos;
        }
        pos++;
    }
    return -1;
}

Operand::Operand(const vector<Token>& tokens,
                 const vector<AddressMode>& invalidAddressModes)
    : constantDataRaw(UNDEFINED_TOKEN) {
    determineOperand(tokens);
    if (isIllegalAddressMode(invalidAddressModes)) {
        throw DecodingException("Invalid address mode for " +
                                Token::joinTokens(tokens));
    }
}

Operand::Operand(const Token& token,
                 const vector<AddressMode>& invalidAddressModes)
    : constantDataRaw(UNDEFINED_TOKEN) {
    std::vector<Token> tokens;
    tokens.push_back(token);
    determineOperand(tokens);
    if (isIllegalAddressMode(invalidAddressModes)) {
        throw DecodingException("Invalid address mode for " +
                                Token::joinTokens(tokens));
    }
}

bool Operand::isIllegalAddressMode(
    const vector<AddressMode>& invalidAddressModes) const {
    for (auto&& iam : invalidAddressModes) {
        if (addressMode == iam) {
            return true;
        }
    }
    return false;
}

void Operand::determineOperand(const vector<Token>& tokens) {
    switch (tokens[0].getType()) {
        case Token::HEX_NUMBER:
        case Token::BIN_NUMBER:
        case Token::DEC_NUMBER:
            if (tokens.size() != 1) {
                throw DecodingException("Invalid operand " +
                                        Token::joinTokens(tokens));
            }
            constantData = tokens[0].getIntValue();
            addressMode = IMMEDIATE_CONSTANT;
            return;
        case Token::IMMEDIATE_QUANT:
            if (tokens.size() != 2 ||
                tokens[1].getType() != Token::IDENTIFICATOR) {
                throw DecodingException("Invalid operand " +
                                        Token::joinTokens(tokens));
            }
            addressMode = IMMEDIATE_SYMBOL;
            constantDataRaw = tokens[1];
            return;
        case Token::LOCATION_VALUE_QUANT:
            if (tokens.size() != 2 ||
                (tokens[1].getType() != Token::BIN_NUMBER &&
                 tokens[1].getType() != Token::HEX_NUMBER &&
                 tokens[1].getType() != Token::DEC_NUMBER)) {
                throw DecodingException("Invalid operand " +
                                        Token::joinTokens(tokens));
            }
            addressMode = MEMORY_CONSTANT;
            constantData = tokens[1].getIntValue();
            return;
        case Token::PC_RELATIVE_QUANT:
            if (tokens.size() != 2 ||
                tokens[1].getType() != Token::IDENTIFICATOR) {
                throw DecodingException("Invalid operand " +
                                        Token::joinTokens(tokens));
            }
            addressMode = PC_RELATIVE;
            constantDataRaw = tokens[1];
            return;
        case Token::IDENTIFICATOR: {
            auto index = getRegistry(tokens[0].getValue());
            if (index != -1) {
                if (tokens.size() == 1) {
                    addressMode = REG_DIRECT;
                    registryData = registries[index].code;
                    return;
                }
                if (tokens[1].getType() != Token::OPEN_BRACKETS ||
                    tokens.size() != 4 ||
                    tokens[3].getType() != Token::CLOSED_BRACKETS) {
                    throw DecodingException("Invalid operand " +
                                            Token::joinTokens(tokens));
                }
                addressMode = REG_INDIRECT_W_DISPL;
                registryData = registries[index].code;
                constantDataRaw = tokens[2];
                return;
            }
            if (tokens.size() != 1) {
                throw DecodingException("Invalid operand " +
                                        Token::joinTokens(tokens));
            }
            if (tokens[0].getValue() == "PSW" ||
                tokens[0].getValue() == "psw") {
                addressMode = PSW;
                return;
            }
            addressMode = MEMORY_SYMBOL;
            constantDataRaw = tokens[0];
            return;
        }
        case Token::ASCI_CHARACTER:
            if (tokens.size() != 1) {
                throw DecodingException("Invalid operand " +
                                        Token::joinTokens(tokens));
            }
            addressMode = IMMEDIATE_CONSTANT;
            constantData = tokens[0].getValue()[0];
            return;
        default:
            throw DecodingException("Invalid operand " +
                                    Token::joinTokens(tokens));
    }
}

RelocationData* Operand::evaluate(const SymbolTable& symbolTable,
                                  int myLocation, int nextInstructionLocation,
                                  const std::string& mySection) {
    switch (addressMode) {
        case IMMEDIATE_CONSTANT:
        case MEMORY_CONSTANT:
        case PSW:
        case REG_DIRECT:
            return nullptr;
        case REG_INDIRECT_W_DISPL:
            if (constantDataRaw.getType() != Token::IDENTIFICATOR) {
                constantData = constantDataRaw.getIntValue();
                return nullptr;
            }
        case IMMEDIATE_SYMBOL:
        case MEMORY_SYMBOL: {
            auto symbol = symbolTable.getSymbol(constantDataRaw.getValue());
            auto section = symbolTable.getSection(mySection);
            constantData = symbol.address;
            return new RelocationData(myLocation, RelocationData::APSOLUTE,
                                      symbol.scope == SymbolTable::LOCAL
                                          ? symbol.section
                                          : symbol.number);
        }
        case PC_RELATIVE: {
            auto symbol = symbolTable.getSymbol(constantDataRaw.getValue());
            auto section = symbolTable.getSection(mySection);
            if (section.number == symbol.section) {
                constantData = symbol.address - nextInstructionLocation;
                return nullptr;
            }
            constantData =
                symbol.address - (nextInstructionLocation - myLocation);
            return new RelocationData(myLocation, RelocationData::RELATIVE,
                                      symbol.scope == SymbolTable::LOCAL
                                          ? symbol.section
                                          : symbol.number);
        }
    }
}
