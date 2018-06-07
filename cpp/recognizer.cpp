#include "recognizer.h"
#include "command.h"
#include "exceptions_a.h"
#include "instruction.h"
#include "section.h"
#include "tokenizer.h"
#include "utils.h"

#include <string>
#include <vector>
using std::string;
using std::vector;

Recognizer::Recognizer() {
    sectionSpecifications.push_back(
        SectionSpecification(".data", Section::DATA));
    sectionSpecifications.push_back(
        SectionSpecification(".text", Section::TEXT));
    sectionSpecifications.push_back(SectionSpecification(".bss", Section::BSS));
    sectionSpecifications.push_back(
        SectionSpecification(".rodata", Section::RODATA));

    definitionSpecifications.push_back(DefinitionSpecification(".char", 1));
    definitionSpecifications.push_back(DefinitionSpecification(".word", 2));
    definitionSpecifications.push_back(DefinitionSpecification(".long", 4));

    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("add", 0x30));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("addeq", 0x00));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("addne", 0x10));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("addgt", 0x20));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("addal", 0x30));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("sub", 0x31));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("subeq", 0x01));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("subne", 0x11));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("subgt", 0x21));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("subal", 0x31));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("mul", 0x32));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("muleq", 0x02));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("mulne", 0x12));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("mulgt", 0x22));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("mulal", 0x32));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("div", 0x33));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("diveq", 0x03));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("divne", 0x13));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("divgt", 0x23));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("dival", 0x33));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("cmp", 0x34));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("cmpeq", 0x04));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("cmpne", 0x14));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("cmpgt", 0x24));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("cmpal", 0x34));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("and", 0x35));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("andeq", 0x05));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("andne", 0x15));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("andgt", 0x25));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("andal", 0x35));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("or", 0x36));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("oreq", 0x06));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("orne", 0x16));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("orgt", 0x26));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("oral", 0x36));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("not", 0x37));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("noteq", 0x07));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("notne", 0x17));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("notgt", 0x27));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("notal", 0x37));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("test", 0x38));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("testeq", 0x08));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("testne", 0x18));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("testgt", 0x28));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("testal", 0x38));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("mov", 0x3D));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("moveq", 0x0D));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("movne", 0x1D));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("movgt", 0x2D));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("moval", 0x3D));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("shl", 0x3E));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("shleq", 0x0E));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("shlne", 0x1E));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("shlgt", 0x2E));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("shlal", 0x3E));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("shr", 0x3F));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("shreq", 0x0F));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("shrne", 0x1F));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("shrgt", 0x2F));
    doubleAddressInstructionSpecs.push_back(
        InstructionSpecification("shral", 0x3F));

    singleAddressInstructionSpecs.push_back(
        InstructionSpecification("push", 0x39));
    singleAddressInstructionSpecs.push_back(
        InstructionSpecification("pusheq", 0x09));
    singleAddressInstructionSpecs.push_back(
        InstructionSpecification("pushne", 0x19));
    singleAddressInstructionSpecs.push_back(
        InstructionSpecification("pushgt", 0x29));
    singleAddressInstructionSpecs.push_back(
        InstructionSpecification("pushal", 0x39));
    singleAddressInstructionSpecs.push_back(
        InstructionSpecification("pop", 0x3A));
    singleAddressInstructionSpecs.push_back(
        InstructionSpecification("popeq", 0x0A));
    singleAddressInstructionSpecs.push_back(
        InstructionSpecification("popne", 0x1A));
    singleAddressInstructionSpecs.push_back(
        InstructionSpecification("popgt", 0x2A));
    singleAddressInstructionSpecs.push_back(
        InstructionSpecification("popal", 0x3A));
    singleAddressInstructionSpecs.push_back(
        InstructionSpecification("call", 0x3B));
    singleAddressInstructionSpecs.push_back(
        InstructionSpecification("calleq", 0x0B));
    singleAddressInstructionSpecs.push_back(
        InstructionSpecification("callne", 0x1B));
    singleAddressInstructionSpecs.push_back(
        InstructionSpecification("callgt", 0x2B));
    singleAddressInstructionSpecs.push_back(
        InstructionSpecification("callal", 0x3B));

    noAddressInstructionSpecs.push_back(InstructionSpecification("iret", 0x3C));
    noAddressInstructionSpecs.push_back(
        InstructionSpecification("ireteq", 0x0C));
    noAddressInstructionSpecs.push_back(
        InstructionSpecification("iretne", 0x1C));
    noAddressInstructionSpecs.push_back(
        InstructionSpecification("iretgt", 0x2C));
    noAddressInstructionSpecs.push_back(
        InstructionSpecification("iretal", 0x3C));

    retInstructionSpecs.push_back(InstructionSpecification("ret", 0x3A));
    retInstructionSpecs.push_back(InstructionSpecification("reteq", 0x0A));
    retInstructionSpecs.push_back(InstructionSpecification("retne", 0x1A));
    retInstructionSpecs.push_back(InstructionSpecification("retgt", 0x2A));
    retInstructionSpecs.push_back(InstructionSpecification("retal", 0x3A));

    jmpInstructionSpecs.push_back(InstructionSpecification("jmp", 0x30));
    jmpInstructionSpecs.push_back(InstructionSpecification("jmpeq", 0x00));
    jmpInstructionSpecs.push_back(InstructionSpecification("jmpne", 0x10));
    jmpInstructionSpecs.push_back(InstructionSpecification("jmpgt", 0x20));
    jmpInstructionSpecs.push_back(InstructionSpecification("jmpal", 0x30));
}

Command Recognizer::recognizeCommand(TokenStream& tokenStream) const {
    auto token = tokenStream.next();
    while (token.getType() == Token::LINE_DELIMITER) {
        token = tokenStream.next();
    }
    if (token.getType() == Token::LABEL) {
        return Command(token.getValue(), Command::LABEL);
    }
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
    if (isInstruction(token)) {
        return Command(token.getValue(), Command::INSTRUCTION);
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

bool Recognizer::isInstruction(const Token& token) const {
    if (token.getType() != Token::IDENTIFICATOR) {
        return false;
    }
    auto key = token.getValue();
    for (auto&& sais : singleAddressInstructionSpecs) {
        if (key == sais.name || key == Utils::uppercaseString(sais.name)) {
            return true;
        }
    }
    for (auto&& dais : doubleAddressInstructionSpecs) {
        if (key == dais.name || key == Utils::uppercaseString(dais.name)) {
            return true;
        }
    }
    for (auto&& nais : noAddressInstructionSpecs) {
        if (key == nais.name || key == Utils::uppercaseString(nais.name)) {
            return true;
        }
    }
    for (auto&& retis : retInstructionSpecs) {
        if (key == retis.name || key == Utils::uppercaseString(retis.name)) {
            return true;
        }
    }
    for (auto&& jmpis : jmpInstructionSpecs) {
        if (key == jmpis.name || key == Utils::uppercaseString(jmpis.name)) {
            return true;
        }
    }
    return false;
}

Section* Recognizer::recognizeSection(const Command& comm,
                                      TokenStream& tokenStream) const {
    auto t = tokenStream.next();
    if (t.getType() != Token::LINE_DELIMITER) {
        throw DecodingException("Invalid character " + t.getValue() +
                                " after section definition");
    }
    for (auto&& ss : sectionSpecifications) {
        if (ss.name == comm.name ||
            Utils::uppercaseString(ss.name) == comm.name) {
            return new Section(comm.name, ss.type);
        }
    }
    throw DecodingException("Unknown section name " + comm.name);
}

vector<string> Recognizer::recognizeGlobalSymbols(
    TokenStream& tokenStream) const {
    auto firstToken = tokenStream.next();
    auto secondToken = tokenStream.next();
    vector<string> symbols;
    while (true) {
        if (firstToken.getType() != Token::IDENTIFICATOR) {
            throw DecodingException("Invalid character " +
                                    firstToken.getValue() +
                                    " in global symbol decl");
        }
        symbols.push_back(firstToken.getValue());
        if (secondToken.getType() == Token::LINE_DELIMITER) {
            return symbols;
        }
        if (secondToken.getType() != Token::COMMA) {
            throw DecodingException("Invalid character " +
                                    secondToken.getValue() +
                                    " in global symbol declaration");
        }
        firstToken = tokenStream.next();
        secondToken = tokenStream.next();
    }
}

Definition Recognizer::recognizeDefinition(const Command& comm) const {
    if (comm.type != Command::DEFINITION) {
        throw SystemException(
            "Invalid command type for definition for command " + comm.name);
    }
    for (auto&& d : definitionSpecifications) {
        if (d.name == comm.name ||
            Utils::uppercaseString(d.name) == comm.name) {
            return Definition(d.name, d.size);
        }
    }
    throw SystemException("Unknown definition name " + comm.name);
}

Instruction* Recognizer::recognizeInstruction(const Command& comm) const {
    if (comm.type != Command::INSTRUCTION) {
        throw SystemException(
            "Invalid command type for instruction for command " + comm.name);
    }
    for (auto&& sais : singleAddressInstructionSpecs) {
        if (sais.name == comm.name ||
            Utils::uppercaseString(sais.name) == comm.name) {
            return new SingleAddressInstruction(sais.name, sais.opcode);
        }
    }
    for (auto&& dais : doubleAddressInstructionSpecs) {
        if (dais.name == comm.name ||
            Utils::uppercaseString(dais.name) == comm.name) {
            return new DoubleAddressInstruction(dais.name, dais.opcode);
        }
    }
    for (auto&& nais : noAddressInstructionSpecs) {
        if (nais.name == comm.name ||
            Utils::uppercaseString(nais.name) == comm.name) {
            return new NoAddressInstruction(nais.name, nais.opcode);
        }
    }
    throw SystemException("No instruction found with name " + comm.name);
}