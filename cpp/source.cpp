#include <fstream>
#include <iostream>
#include "assembler.h"
using std::cout;
using std::endl;
using std::ifstream;
using std::vector;

int main() {
    auto inputFileName = "./input/parser_test.txt";
    auto outputFileName = "./output/smth.txt";
    Assembler as;
    as.assembleFile(inputFileName, outputFileName);
}