#include "assembler.h"
#include <fstream>
#include <iostream>
#include <string>
#include "exceptions_a.h"
#include "tokenizer.h"
using std::cout;
using std::ifstream;
using std::ofstream;
using std::string;

void Assembler::assembleFile(const string& inputFileName,
                             const string& outputFileName) {
    ifstream input;
    input.exceptions(ifstream::badbit);
    try {
        input.open(inputFileName.c_str());
        Tokenizer tokenizer;
        auto tokenStream = TokenStream(tokenizer.parse(input));
        input.close();

        firstPass(tokenStream);
        tokenStream.reset();
        secondPass(tokenStream);

        ofstream output;
        output.open(outputFileName.c_str());

        output.close();
    } catch (const ifstream::failure& f) {
        cout << f.what();
    } catch (const AssemblerException& ae) {
        cout << ae.error() << std::endl;
    }
}

void Assembler::firstPass(TokenStream& tokenStream) {}

void Assembler::secondPass(TokenStream& tokenStream) {}