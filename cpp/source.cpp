#include <fstream>
#include <iostream>
#include <vector>
#include "tokenizer.h"
using std::cout;
using std::endl;
using std::ifstream;
using std::vector;

int main() {
    ifstream inputStream;
    inputStream.exceptions(ifstream::badbit);
    auto fileName = "./input/parser_test.txt";
    try {
        inputStream.open(fileName);
        Tokenizer t;
        auto parsedInput = t.parse(inputStream);
        for (auto&& element : parsedInput) {
            cout << element.getTypeDescription() << " " << element.getValue()
                 << endl;
        }

    } catch (const ifstream::failure& f) {
        cout << "Error while opening file " << fileName << " " << f.what()
             << endl;
    } catch (const AssemblerException& ae) {
        cout << "Error: " << ae.error();
    }
}