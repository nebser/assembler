#include <fstream>
#include <iostream>
#include <string>
#include "assembler.h"
using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::vector;

int main(int argc, char** argv) {
    // Arguments check
    if (argc < 3 || argc > 4) {
        cout << "\nCall to the program must be in format [OPTIONAL]:\n\n\t "
                "assembler.out INPUT_FILE OUTPUT_FILE [START_ADDRESS]\n"
             << std::endl;
        return -1;
    }

    try {
        // Argument unwrapping
        auto inputFileName = argv[1];
        auto outputFileName = argv[2];
        auto startAddress = 0;
        if (argc == 4) {
            startAddress = std::stoi(argv[3], 0, 0);
        }

        // Assemly of a file
        Assembler as;
        as.assembleFile(inputFileName, outputFileName, startAddress);
        cout << "FILE ASSEMBLY SUCCESSFULL" << endl;

    } catch (const ifstream::failure& f) {
        cout << std::endl << f.what() << std::endl << std::endl;
        return -2;
    } catch (const AssemblerException& ae) {
        cout << std::endl << ae.error() << std::endl << std::endl;
        return -3;
    } catch (std::invalid_argument& iv) {
        cout << "\nStart address must be an integer value\n" << std::endl;
        return -4;
    }

    return 0;
}