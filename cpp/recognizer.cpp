#include "recognizer.h"
#include "exceptions_a.h"
#include "tokenizer.h"
#include "utils.h"

#include <string>
#include <vector>
using std::string;
using std::vector;

Recognizer::Recognizer() {
    sectionSpecifications.push_back(SectionSpecification(".data"));
    sectionSpecifications.push_back(SectionSpecification(".text"));
    sectionSpecifications.push_back(SectionSpecification(".bss"));
    sectionSpecifications.push_back(SectionSpecification(".rodata"));

    definitionSpecifications.push_back(DefinitionSpecification(".char", 1));
    definitionSpecifications.push_back(DefinitionSpecification(".word", 2));
    definitionSpecifications.push_back(DefinitionSpecification(".long", 4));
}

Command Recognizer::recognizeCommand(TokenStream& tokenStream) const {
    auto token = tokenStream.next();
    if (isGlobalDirective(token)) {
        return Command(token.getValue(), Command::GLOBAL_DIR);
    }
    if (isEndDirective(token)) {
        return Command(token.getValue(), Command::END_DIR);
    }
    if (isAlignDirective(token)) {
        return Command(token.getValue(), Command::ALIGN_DIR);
    }
    if (isSkipDirective(token)) {
        return Command(token.getValue(), Command::SKIP_DIR);
    }
    if (isSection(token)) {
        return Command(token.getValue(), Command::SECTION);
    }
    if (isDefinition(token)) {
        return Command(token.getValue(), Command::DEFINITION);
    }

    throw UnknownCommandException(token.getValue());
}

bool Recognizer::isGlobalDirective(const Token& token) const {
    auto v = token.getValue();
    return token.getType() == Token::IDENTIFICATOR &&
           (v == ".global" || v == ".GLOBAL");
}

bool Recognizer::isEndDirective(const Token& token) const {
    auto v = token.getValue();
    return token.getType() == Token::IDENTIFICATOR &&
           (v == ".end" || v == ".END");
}

bool Recognizer::isAlignDirective(const Token& token) const {
    auto v = token.getValue();
    return token.getType() == Token::IDENTIFICATOR &&
           (v == ".align" || v == ".ALIGN");
}

bool Recognizer::isSkipDirective(const Token& token) const {
    auto v = token.getValue();
    return token.getType() == Token::IDENTIFICATOR &&
           (v == ".skip" || v == ".SKIP");
}

bool Recognizer::isSection(const Token& token) const {
    if (token.getType() != Token::IDENTIFICATOR) {
        return false;
    }
    auto v = token.getValue();
    for (auto&& sp : sectionSpecifications) {
        if (v == sp.name || v == Utils::uppercaseString(sp.name)) {
            return true;
        }
    }
    return false;
}

bool Recognizer::isDefinition(const Token& token) const {
    if (token.getType() != Token::IDENTIFICATOR) {
        return false;
    }
    auto v = token.getValue();
    for (auto&& df : definitionSpecifications) {
        if (v == df.name || v == Utils::uppercaseString(df.name)) {
            return true;
        }
    }
    return false;
}