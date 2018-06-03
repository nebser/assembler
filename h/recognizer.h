#ifndef RECOGNIZER_H_
#define RECOGNIZER_H_

#include <string>
#include <vector>
#include "command.h"
#include "section.h"
#include "tokenizer.h"

class Recognizer {
   public:
    Recognizer();
    Command recognizeCommand(TokenStream&) const;
    Section* recognizeSection(const Command& comm, TokenStream&) const;
    std::vector<std::string> recognizeGlobalSymbols(TokenStream&) const;

   private:
    struct SectionSpecification {
        std::string name;
        Section::Type type;

        SectionSpecification(const std::string& name, Section::Type type)
            : name(name), type(type) {}
    };

    struct InstructionSpecification {
        std::string name;
        unsigned char opcode;

        InstructionSpecification(const std::string& name, unsigned char opcode)
            : name(name), opcode(opcode) {}
    };

    struct DefinitionSpecification {
        std::string name;
        int size;

        DefinitionSpecification(const std::string name, int size)
            : name(name), size(size) {}
    };

    bool isGlobalDirective(const Token&) const;
    bool isEndDirective(const Token&) const;
    bool isAlignDirective(const Token&) const;
    bool isSkipDirective(const Token&) const;
    bool isSection(const Token&) const;
    bool isDefinition(const Token&) const;
    bool isInstruction(const Token&) const;

    std::vector<SectionSpecification> sectionSpecifications;
    std::vector<InstructionSpecification> singleAddressInstructionSpecs;
    std::vector<InstructionSpecification> doubleAddressInstructionSpecs;
    std::vector<InstructionSpecification> noAddressInstructionSpecs;
    std::vector<InstructionSpecification> retInstructionSpecs;
    std::vector<InstructionSpecification> jmpInstructionSpecs;
    std::vector<DefinitionSpecification> definitionSpecifications;
};

#endif