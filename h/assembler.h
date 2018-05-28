#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

#include <fstream>
#include <string>
#include "tokenizer.h"

class Assembler {
   public:
    Assembler() = default;

    Assembler(const Assembler&) = delete;
    Assembler(Assembler&&) = delete;
    Assembler& operator=(const Assembler&) = delete;
    Assembler& operator=(Assembler&&) = delete;

    void assembleFile(const std::string& inputFileName,
                      const std::string& outputFileName);

   private:
    void firstPass(TokenStream&);
    void secondPass(TokenStream&);
};

#endif