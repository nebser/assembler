#include "assembler.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "exceptions_a.h"
#include "recognizer.h"
#include "symbol_table.h"
#include "tokenizer.h"
using std::cout;
using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;

void Assembler::assembleFile(const string& inputFileName,
                             const string& outputFileName) {
    ifstream input;
    input.exceptions(ifstream::badbit);
    try {
        input.open(inputFileName.c_str());
        Tokenizer tokenizer;
        auto tokenStream = TokenStream(tokenizer.parse(input));
        input.close();

        Recognizer recognizer;

        firstPass(tokenStream, recognizer);
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

SymbolTable Assembler::firstPass(TokenStream& tokenStream,
                                 const Recognizer& recognizer) const {
    SymbolTable symbolTable;
    auto locationCounter = 0;
    auto previousCommand = DUMMY_COMMAND;
    Section* currentSection = nullptr;
    vector<string> globalSymbols;
    auto endDetected = false;

    while (!tokenStream.end() && !endDetected) {
        auto command = recognizer.recognizeCommand(tokenStream);
        if (!isSequenceValid(previousCommand, command)) {
            throw InvalidInstructionSequence();
        }
        switch (command.type) {
            case Command::GLOBAL_DIR: {
                auto symbols = recognizer.recognizeGlobalSymbols(tokenStream);
                globalSymbols.insert(globalSymbols.end(), symbols.begin(),
                                     symbols.end());
                break;
            }
            case Command::END_DIR:
                endDetected = true;
                break;
            case Command::SECTION:
                if (currentSection) {
                    symbolTable.updateSectionSize(currentSection->getName(),
                                                  locationCounter);
                    delete currentSection;
                }
                currentSection =
                    recognizer.recognizeSection(command, tokenStream);
                symbolTable.putSection(currentSection->getName());
                locationCounter = 0;
                break;
            case Command::LABEL:
                symbolTable.putSymbol(command.name, locationCounter);
                break;
            
        }
        previousCommand = command;
    }

    if (!endDetected) {
        throw DecodingException("No .end directive detected");
    }
    return symbolTable;
}

bool Assembler::isSequenceValid(const Command& previousCommand,
                                const Command& currentCommand) const {
    if (currentCommand.type == Command::GLOBAL_DIR &&
        previousCommand.type != Command::GLOBAL_DIR &&
        previousCommand.type != Command::EMPTY) {
        return false;
    }
    if (currentCommand.type == Command::LABEL &&
        previousCommand.type == Command::LABEL) {
        return false;
    }
    if (currentCommand.type == Command::INSTRUCTION &&
        (previousCommand.type == Command::GLOBAL_DIR ||
         previousCommand.type == Command::EMPTY)) {
        return false;
    }
    return true;
}

void Assembler::secondPass(TokenStream& tokenStream) {}