#include "assembler.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "data.h"
#include "exceptions_a.h"
#include "instruction.h"
#include "recognizer.h"
#include "section.h"
#include "symbol_table.h"
#include "tokenizer.h"
using std::cout;
using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;

// Address space size is 2^16
const int Assembler::MEMORY_SIZE = 0x10000;

void Assembler::assembleFile(const string& inputFileName,
                             const string& outputFileName,
                             int startAddress) const {
    if (startAddress > MEMORY_SIZE || startAddress < 0) {
        throw MemoryException("Invalid start address " +
                              Utils::convertToString(startAddress));
    }

    ifstream input;
    input.exceptions(ifstream::badbit);
    input.open(inputFileName.c_str());
    Tokenizer tokenizer;
    auto tokenStream = TokenStream(tokenizer.parse(input));
    input.close();

    Recognizer recognizer;

    auto symbolTable = firstPass(tokenStream, startAddress);
    cout << "PASSED" << std::endl;
    cout << symbolTable;
    tokenStream.reset();
    secondPass(tokenStream, startAddress, symbolTable);

    ofstream output;
    output.open(outputFileName.c_str());

    output.close();
}

SymbolTable Assembler::firstPass(TokenStream& tokenStream,
                                 int startAddress) const {
    SymbolTable symbolTable;
    auto locationCounter = startAddress;
    auto previousCommand = DUMMY_COMMAND;
    Section* currentSection = nullptr;
    vector<string> globalSymbols;
    auto endDetected = false;

    while (!tokenStream.end() && !endDetected) {
        auto command = recognizer.recognizeCommand(tokenStream);
        if (!isSequenceValid(previousCommand, command)) {
            throw InvalidInstructionSequence(previousCommand.name,
                                             command.name);
        }
        if (currentSection && !isValidForSection(command, *currentSection)) {
            throw DecodingException("Invalid command " + command.name +
                                    " for section " +
                                    currentSection->getName());
        }
        switch (command.type) {
            case Command::GLOBAL_DIR: {
                auto symbols = recognizer.recognizeGlobalSymbols(tokenStream);
                globalSymbols.insert(globalSymbols.end(), symbols.begin(),
                                     symbols.end());
                break;
            }
            case Command::END_DIR:
                if (currentSection == nullptr) {
                    throw NoSectionDefined(command.name);
                }
                symbolTable.updateSectionSize(
                    currentSection->getName(),
                    locationCounter - symbolTable.getCummulativeSectionSize() -
                        startAddress);
                delete currentSection;
                endDetected = true;
                break;
            case Command::SECTION:
                if (currentSection) {
                    symbolTable.updateSectionSize(
                        currentSection->getName(),
                        locationCounter -
                            symbolTable.getCummulativeSectionSize() -
                            startAddress);
                    delete currentSection;
                }
                currentSection = recognizer.recognizeSection(
                    command, tokenStream, locationCounter);
                symbolTable.putSection(currentSection->getName(),
                                       locationCounter);
                break;
            case Command::LABEL:
                symbolTable.putSymbol(command.name, locationCounter);
                break;
            case Command::DEFINITION:
                locationCounter += recognizer.recognizeDefinition(command)
                                       .decode(tokenStream)
                                       .getSize() /
                                   8;
                break;
            case Command::ALIGN_DIR:
                locationCounter += AlignDirective()
                                       .decode(tokenStream)
                                       .evaluate(locationCounter)
                                       .getSize() /
                                   8;
                break;

            case Command::SKIP_DIR:
                locationCounter +=
                    SkipDirective().decode(tokenStream).getSize() / 8;
                break;
            case Command::INSTRUCTION: {
                auto instruction = recognizer.recognizeInstruction(command);
                locationCounter +=
                    instruction->decode(tokenStream).getSize() / 8;
                delete instruction;
                break;
            }
            default:
                throw SystemException("Unknown command type " + command.name);
        }
        previousCommand = command;
    }

    if (!endDetected) {
        throw DecodingException("No .end directive detected");
    }

    for (auto&& g : globalSymbols) {
        if (!symbolTable.updateScope(g, SymbolTable::GLOBAL)) {
            symbolTable.putSymbol(g, SymbolTable::UNKNOWN_ADDRESS,
                                  SymbolTable::GLOBAL,
                                  SymbolTable::UNKNOWN_SECTION);
        }
    }

    symbolTable.setSymbolNumbers();
    return symbolTable;
}

bool Assembler::isSequenceValid(const Command& previousCommand,
                                const Command& currentCommand) const {
    // Global directive can only be found at the beginning of a file
    if (currentCommand.type == Command::GLOBAL_DIR &&
        previousCommand.type != Command::GLOBAL_DIR &&
        previousCommand.type != Command::EMPTY) {
        return false;
    }

    // Only one label can be defined in a single row
    if (currentCommand.type == Command::LABEL &&
        previousCommand.type == Command::LABEL) {
        return false;
    }

    // A section must be defined before any Instruction or directive
    if ((currentCommand.type == Command::INSTRUCTION ||
         currentCommand.type == Command::DEFINITION ||
         currentCommand.type == Command::SKIP_DIR ||
         currentCommand.type == Command::ALIGN_DIR) &&
        (previousCommand.type == Command::GLOBAL_DIR ||
         previousCommand.type == Command::EMPTY)) {
        return false;
    }

    return true;
}

vector<Section*> Assembler::secondPass(TokenStream& tokenStream,
                                       int startAddress,
                                       const SymbolTable& symbolTable) const {
    Section* currentSection = nullptr;
    auto locationCounter = startAddress;
    auto previousCommand = DUMMY_COMMAND;
    auto endDetected = false;
    vector<Section*> sections;

    while (!tokenStream.end() && !endDetected) {
        auto command = recognizer.recognizeCommand(tokenStream);
        switch (command.type) {
            case Command::GLOBAL_DIR:
                recognizer.recognizeGlobalSymbols(tokenStream);
                break;
            case Command::END_DIR:
                endDetected = true;
                break;
            case Command::SECTION:
                currentSection = recognizer.recognizeSection(
                    command, tokenStream, locationCounter);
                sections.push_back(currentSection);
                break;
            case Command::LABEL:
                break;
            case Command::DEFINITION: {
                auto definition =
                    new Definition(recognizer.recognizeDefinition(command));
                locationCounter +=
                    definition->decode(tokenStream).getSize() / 8;
                currentSection->addIstruction(definition);
                break;
            }
            case Command::ALIGN_DIR: {
                auto alignDir = new AlignDirective();
                locationCounter += alignDir->decode(tokenStream)
                                       .evaluate(locationCounter)
                                       .getSize() /
                                   8;
                currentSection->addIstruction(alignDir);
                break;
            }
            case Command::SKIP_DIR: {
                auto skipDir = new SkipDirective();
                locationCounter += skipDir->decode(tokenStream).getSize() / 8;
                currentSection->addIstruction(skipDir);
                break;
            }
            default:
                throw SystemException("Unknown command " + command.name);
        }
    }

    return sections;
}