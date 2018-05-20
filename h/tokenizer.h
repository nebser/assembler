#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <vector>
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

class Tokenizer {
   public:
    std::vector<Token> parse(const std::string& input,
                             int lineNumber = 1) const;

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
        CLOSED_BRACKETS_DETECTED
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
};

#endif