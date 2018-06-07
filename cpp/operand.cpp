#include "operand.h"
#include <string>
#include <vector>
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

Operand::Operand(const vector<Token>& tokens)
    : constantDataRaw(UNDEFINED_TOKEN) {
    determineOperand(tokens);
}

void Operand::determineOperand(const vector<Token>& tokens) {
    switch (tokens[0].getType()) {
        case Token::HEX_NUMBER:
        case Token::BIN_NUMBER:
        case Token::DEC_NUMBER:
            if (tokens.size() != 1) {
                throw DecodingException("Invalid operand " +
                                        joinTokens(tokens));
            }
            constantData = tokens[0].getIntValue();
            addressMode = IMMEDIATE_CONSTANT;
            return;
        case Token::IMMEDIATE_QUANT:
            if (tokens.size() != 2 ||
                tokens[1].getType() != Token::IDENTIFICATOR) {
                throw DecodingException("Invalid operand " +
                                        joinTokens(tokens));
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
                                        joinTokens(tokens));
            }
            addressMode = MEMORY_CONSTANT;
            constantData = tokens[1].getIntValue();
            return;
        case Token::PC_RELATIVE_QUANT:
            if (tokens.size() != 2 ||
                tokens[1].getType() != Token::IDENTIFICATOR) {
                throw DecodingException("Invalid operand " +
                                        joinTokens(tokens));
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
                                            joinTokens(tokens));
                }
                addressMode = REG_INDIRECT_W_DISPL;
                registryData = registries[index].code;
                constantDataRaw = tokens[2];
                return;
            }
            if (tokens.size() == 1 ||
                tokens[0].getType() != Token::IDENTIFICATOR) {
                throw DecodingException("Invalid operand " +
                                        joinTokens(tokens));
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
        default:
            throw DecodingException("Invalid operand " + joinTokens(tokens));
    }
}

string Operand::joinTokens(const vector<Token>& token) const {
    string result = "";
    for (auto&& t : token) {
        result += t.getValue() + " ";
    }
    return result;
}
