#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

#include <fstream>
#include <string>
#include <vector>
#include "recognizer.h"
#include "section.h"
#include "symbol_table.h"
#include "tokenizer.h"

class Assembler {
   public:
    static const int MEMORY_SIZE;
    Assembler() = default;

    Assembler(const Assembler&) = delete;
    Assembler(Assembler&&) = delete;
    Assembler& operator=(const Assembler&) = delete;
    Assembler& operator=(Assembler&&) = delete;

    void assembleFile(const std::string& inputFileName,
                      const std::string& outputFileName,
                      int startAddress) const;

   private:
    SymbolTable firstPass(TokenStream&, int startAddress) const;
    std::vector<Section*> secondPass(TokenStream&, int startAddress,
                                     const SymbolTable& symbolTable) const;

    bool isSequenceValid(const Command& previousCommand,
                         const Command& currenctCommand) const;

    bool isValidForSection(const Command& comm, const Section& sec) const {
        switch (sec.getType()) {
            case Section::RODATA:
            case Section::DATA:
            case Section::BSS:
                switch (comm.type) {
                    case Command::SKIP_DIR:
                    case Command::ALIGN_DIR:
                    case Command::DEFINITION:
                    case Command::LABEL:
                    case Command::SECTION:
                    case Command::END_DIR:
                        return true;
                    default:
                        return false;
                }
            case Section::TEXT:
                switch (comm.type) {
                    case Command::INSTRUCTION:
                    case Command::LABEL:
                    case Command::SECTION:
                    case Command::END_DIR:
                        return true;
                    default:
                        return false;
                }
        }
    }

    Recognizer recognizer;
};

#endif