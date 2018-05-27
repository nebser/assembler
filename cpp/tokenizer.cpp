#include "tokenizer.h"
#include <fstream>
#include <string>
#include <vector>
#include "exceptions_a.h"
using std::ifstream;
using std::string;
using std::vector;

vector<Token> Tokenizer::parse(ifstream& input) const {
    vector<Token> tokens;
    try {
        auto currentLine = 1;
        for (string line; getline(input, line); currentLine++) {
            auto t = parse(line, currentLine);
            tokens.insert(tokens.end(), t.begin(), t.end());
            tokens.push_back(createNewLineToken());
        }
    } catch (const ifstream::failure& f) {
        throw SystemException("Error while parsing input " + string(f.what()));
    }
    return tokens;
}

vector<Token> Tokenizer::parse(const std::string& input, int lineNumber) const {
    auto feeder = Feeder(input);
    auto end = false;
    auto state = HUNTING;
    string pendingToken = "";
    vector<Token> tokens;

    while (!end) {
        auto character = feeder.feed();
        switch (state) {
            case HUNTING:
                switch (character) {
                    case ' ':
                    case '\t':
                        continue;
                    case '\0':
                        end = true;
                        break;
                    case '0':
                        pendingToken += character;
                        state = ZERO_DETECTED;
                        break;
                    case '1':
                        pendingToken += character;
                        state = ONE_DETECTED;
                        break;
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        pendingToken += character;
                        state = DEC_NUMERIC_DETECTION;
                        break;
                    case '$':
                    case '&':
                    case '[':
                    case ']':
                    case ',':
                        tokens.push_back(createCharBasedToken(character));
                        break;
                    default:
                        pendingToken += character;
                        state = IDENTIFICATOR_DETECTION;
                        break;
                }
                break;
            case IDENTIFICATOR_DETECTION:
                switch (character) {
                    case ' ':
                    case '\t':
                        tokens.push_back(
                            createIdentificatorToken(pendingToken));
                        pendingToken.clear();
                        state = HUNTING;
                        break;
                    case '[':
                    case ',':
                        tokens.push_back(
                            createIdentificatorToken(pendingToken));
                        tokens.push_back(createCharBasedToken(character));
                        pendingToken.clear();
                        state = HUNTING;
                        break;
                    case ']':
                        tokens.push_back(
                            createIdentificatorToken(pendingToken));
                        tokens.push_back(createCharBasedToken(character));
                        pendingToken.clear();
                        state = CLOSED_BRACKETS_DETECTED;
                        break;
                    case '\0':
                        tokens.push_back(
                            createIdentificatorToken(pendingToken));
                        end = true;
                        break;
                    case ':':
                        state = LABEL_DETECTION;
                        break;
                    case '.':
                        throw ParserException(
                            pendingToken + string(1, character), lineNumber);
                    default:
                        pendingToken += character;
                        break;
                }
                break;
            case ZERO_DETECTED:
                switch (character) {
                    case ' ':
                    case '\t':
                        pendingToken += character;
                        tokens.push_back(
                            createDecimalNumberToken(pendingToken));
                        pendingToken.clear();
                        state = HUNTING;
                        break;
                    case '[':
                    case ',':
                        tokens.push_back(
                            createDecimalNumberToken(pendingToken));
                        pendingToken.clear();
                        tokens.push_back(createCharBasedToken(character));
                        state = HUNTING;
                        break;
                    case ']':
                        tokens.push_back(
                            createDecimalNumberToken(pendingToken));
                        pendingToken.clear();
                        tokens.push_back(createCharBasedToken(character));
                        state = CLOSED_BRACKETS_DETECTED;
                        break;
                    case 'x':
                    case 'X':
                        pendingToken += character;
                        state = HEX_NUMERIC_DETECTION;
                        break;
                    case '0':
                    case '1':
                        pendingToken += character;
                        state = BIN_NUMERIC_DETECTION;
                        break;
                    case '\0':
                        tokens.push_back(
                            createDecimalNumberToken(pendingToken));
                        end = true;
                        break;
                    default:
                        throw ParserException(
                            pendingToken + string(1, character), lineNumber);
                }
                break;
            case ONE_DETECTED:
                switch (character) {
                    case '0':
                    case '1':
                        pendingToken += character;
                        break;
                    case 'b':
                    case 'B':
                        pendingToken += character;
                        state = BIN_NUMERIC_DETECTED;
                        break;
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        pendingToken += character;
                        state = DEC_NUMERIC_DETECTION;
                        break;
                    case ' ':
                    case '\t':
                        tokens.push_back(
                            createDecimalNumberToken(pendingToken));
                        state = HUNTING;
                        break;
                    case '[':
                    case ',':
                        tokens.push_back(
                            createDecimalNumberToken(pendingToken));
                        pendingToken.clear();
                        tokens.push_back(createCharBasedToken(character));
                        state = HUNTING;
                        break;
                    case ']':
                        tokens.push_back(
                            createDecimalNumberToken(pendingToken));
                        pendingToken.clear();
                        tokens.push_back(createCharBasedToken(character));
                        state = CLOSED_BRACKETS_DETECTED;
                    case '\0':
                        tokens.push_back(
                            createDecimalNumberToken(pendingToken));
                        end = true;
                        break;
                    default:
                        throw ParserException(
                            pendingToken + string(1, character), lineNumber);
                }
                break;
            case DEC_NUMERIC_DETECTION:
                switch (character) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        pendingToken += character;
                        break;
                    case ' ':
                    case '\t':
                        tokens.push_back(
                            createDecimalNumberToken(pendingToken));
                        pendingToken.clear();
                        state = HUNTING;
                        break;
                    case ',':
                        tokens.push_back(
                            createDecimalNumberToken(pendingToken));
                        pendingToken.clear();
                        tokens.push_back(createCharBasedToken(character));
                        state = HUNTING;
                        break;
                    case ']':
                        tokens.push_back(
                            createDecimalNumberToken(pendingToken));
                        pendingToken.clear();
                        state = CLOSED_BRACKETS_DETECTED;
                        break;
                    case '\0':
                        tokens.push_back(
                            createDecimalNumberToken(pendingToken));
                        end = true;
                        break;
                    default:
                        throw ParserException(
                            pendingToken + string(1, character), lineNumber);
                }
                break;
            case HEX_NUMERIC_DETECTION:
                switch (character) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                    case 'A':
                    case 'a':
                    case 'B':
                    case 'b':
                    case 'C':
                    case 'c':
                    case 'D':
                    case 'd':
                    case 'E':
                    case 'e':
                    case 'F':
                    case 'f':
                        pendingToken += character;
                        break;
                    case ' ':
                    case '\t':
                        tokens.push_back(createHexNumberToken(pendingToken));
                        pendingToken.clear();
                        state = HUNTING;
                        break;
                    case ']':
                        tokens.push_back(createHexNumberToken(pendingToken));
                        pendingToken.clear();
                        tokens.push_back(createCharBasedToken(character));
                        state = CLOSED_BRACKETS_DETECTED;
                        break;
                    case '\0':
                        tokens.push_back(createHexNumberToken(pendingToken));
                        end = true;
                        break;
                    default:
                        throw ParserException(
                            pendingToken + string(1, character), lineNumber);
                }
                break;
            case BIN_NUMERIC_DETECTION:
                switch (character) {
                    case 'b':
                    case 'B':
                        pendingToken += character;
                        state = BIN_NUMERIC_DETECTED;
                        break;
                    case '0':
                    case '1':
                        pendingToken += character;
                        break;
                    default:
                        throw ParserException(
                            pendingToken + string(1, character), lineNumber);
                }
                break;
            case BIN_NUMERIC_DETECTED:
                switch (character) {
                    case ' ':
                    case '\t':
                        tokens.push_back(createBinaryNumberToken(pendingToken));
                        pendingToken.clear();
                        state = HUNTING;
                        break;
                    case ']':
                        tokens.push_back(createBinaryNumberToken(pendingToken));
                        pendingToken.clear();
                        state = CLOSED_BRACKETS_DETECTED;
                        break;
                    case '\0':
                        tokens.push_back(createBinaryNumberToken(pendingToken));
                        end = true;
                        break;
                    default:
                        throw ParserException(
                            pendingToken + string(1, character), lineNumber);
                }
                break;
            case CLOSED_BRACKETS_DETECTED:
                switch (character) {
                    case ' ':
                    case '\t':
                        state = HUNTING;
                        break;
                    case '\0':
                        end = true;
                        break;
                    default:
                        throw ParserException(string(1, character), lineNumber);
                }
                break;
            case LABEL_DETECTION:
                switch (character) {
                    case ';':
                    case '\0':
                        tokens.push_back(createLabelToken(pendingToken));
                        pendingToken.clear();
                        end = true;
                        break;
                    case ' ':
                    case '\t':
                        tokens.push_back(createLabelToken(pendingToken));
                        pendingToken.clear();
                        state = HUNTING;
                        break;
                    default:
                        throw ParserException(
                            pendingToken + ":" + string(1, character),
                            lineNumber);
                }
                break;
            default:
                throw SystemException("Invalid state detected");
        }
    }

    return tokens;
}

Token Tokenizer::createCharBasedToken(char value) const {
    switch (value) {
        case '$':
            return Token(Token::PC_RELATIVE_QUANT, "$");
        case '&':
            return Token(Token::IMMEDIATE_QUANT, "&");
        case '[':
            return Token(Token::OPEN_BRACKETS, "[");
        case ']':
            return Token(Token::CLOSED_BRACKETS, "]");
        case ',':
            return Token(Token::COMMA, ",");
    }
    return UNDEFINED_TOKEN;
}
