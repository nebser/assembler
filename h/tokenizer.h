#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <fstream>
#include <string>
#include <vector>
#include "exceptions_a.h"
#include "token.h"

class Feeder {
   public:
    Feeder(const std::string& input) : input(input), currentIndex(0) {}

    char feed() {
        if (currentIndex == input.size() || input[currentIndex] == ';') {
            return 0;
        }
        auto nextCharacter = input[currentIndex];
        currentIndex++;
        return nextCharacter;
    }

   private:
    std::string input;
    int currentIndex;
};

class TokenStream {
   public:
    TokenStream(const std::vector<Token>& tokens)
        : tokens(tokens), currentIndex(0) {}

    Token next() {
        if (currentIndex >= tokens.size()) {
            throw StreamException();
        }
        return tokens[currentIndex++];
    }

    Token peek() const {
        if (currentIndex >= tokens.size()) {
            throw StreamException();
        }
        return tokens[currentIndex];
    }

    void reset() { currentIndex = 0; }

    bool end() const { return currentIndex == tokens.size(); }

   private:
    std::vector<Token> tokens;
    int currentIndex;
};

class Tokenizer {
   public:
    std::vector<Token> parse(const std::string& input,
                             int lineNumber = 1) const;

    std::vector<Token> parse(std::ifstream& input) const;

   private:
    enum State {
        HUNTING,
        IDENTIFICATOR_DETECTION,
        ZERO_DETECTED,
        ONE_DETECTED,
        DEC_NUMERIC_DETECTION,
        HEX_NUMERIC_DETECTION,
        BIN_NUMERIC_DETECTION,
        BIN_NUMERIC_DETECTED,
        CLOSED_BRACKETS_DETECTED,
        LABEL_DETECTION
    };

    Token createCharBasedToken(char) const;

    Token createIdentificatorToken(const std::string& value) const {
        return Token(Token::IDENTIFICATOR, value);
    }

    Token createDecimalNumberToken(const std::string& value) const {
        return Token(Token::DEC_NUMBER, value);
    }

    Token createHexNumberToken(const std::string& value) const {
        return Token(Token::HEX_NUMBER, value);
    }

    Token createBinaryNumberToken(const std::string& value) const {
        return Token(Token::BIN_NUMBER, value);
    }

    Token createLabelToken(const std::string& value) const {
        return Token(Token::LABEL, value);
    }

    Token createNewLineToken() const {
        return Token(Token::LINE_DELIMITER, "\n");
    }
};

#endif