#ifndef RECOGNIZER_H_
#define RECOGNIZER_H_

#include <string>
#include <vector>
#include "tokenizer.h"

struct Command {
    enum Type {
        GLOBAL_DIR,
        INSTRUCTION,
        END_DIR,
        DEFINITION,
        SECTION,
        ALIGN_DIR,
        SKIP_DIR
    };

    std::string name;
    Type type;

    Command(const std::string& name, Type type) : name(name), type(type) {}
};

class Recognizer {
   public:
    Recognizer();
    Command recognizeCommand(TokenStream&) const;

   private:
    struct SectionSpecification {
        std::string name;

        SectionSpecification(const std::string& name) : name(name) {}
    };

    struct InstructionSpecification {
        std::string name;
        std::string opcode;

        InstructionSpecification(const std::string& name,
                                 const std::string& opcode)
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

    std::vector<SectionSpecification> sectionSpecifications;
    std::vector<InstructionSpecification> instructionSpecifications;
    std::vector<DefinitionSpecification> definitionSpecifications;
};

#endif