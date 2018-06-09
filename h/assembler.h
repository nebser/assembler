#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

#include <fstream>
#include <string>
#include "recognizer.h"
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
                      const std::string& outputFileName, int startAddress);

   private:
    SymbolTable firstPass(TokenStream&, const Recognizer& recognizer,
                          int startAddress) const;
    void secondPass(TokenStream&);

    bool isSequenceValid(const Command& previousCommand,
                         const Command& currenctCommand) const;
};

#endif