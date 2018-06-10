#include "symbol_table.h"
#include <string>
#include <vector>
#include "exceptions_a.h"
using std::endl;
using std::ostream;
using std::string;
using std::vector;

const int SymbolTable::UNKNOWN_SECTION = 0;
const int SymbolTable::UNKNOWN_ADDRESS = 0;

void SymbolTable::putSection(const string& name, unsigned int address) {
    for (auto&& section : sections) {
        if (section.name == name) {
            throw SymbolAlreadyDefinedException(section.name);
        }
    }
    sections.push_back(Section(name, address, sections.size() + 1, 0));
    lastSection++;
}

void SymbolTable::putSymbol(const string& name, int address, Scope scope,
                            int section) {
    if (section == -2) {
        if (lastSection == 0) {
            throw NoSectionDefined(name);
        }
        section = lastSection;
    }
    for (auto&& symbol : symbols) {
        if (symbol.name == name) {
            throw SymbolAlreadyDefinedException(symbol.name);
        }
    }
    symbols.push_back(Symbol(name, section, scope, address, symbols.size()));
}

bool SymbolTable::updateScope(const string& name, Scope newScope) {
    for (auto& symbol : symbols) {
        if (symbol.name == name) {
            symbol.scope = newScope;
            return true;
        }
    }
    return false;
}

bool SymbolTable::symbolExists(const string& name) const {
    for (auto&& symbol : symbols) {
        if (symbol.name == name) {
            return true;
        }
    }
    return false;
}

bool SymbolTable::sectionExists(const string& name) const {
    for (auto&& section : sections) {
        if (section.name == name) {
            return true;
        }
    }
    return false;
}

const SymbolTable::Symbol& SymbolTable::getSymbol(const string& name) const {
    for (auto&& symbol : symbols) {
        if (symbol.name == name) {
            return symbol;
        }
    }
    throw SymbolNotDefined(name);
}

const SymbolTable::Section& SymbolTable::getSection(const string& name) const {
    for (auto&& section : sections) {
        if (section.name == name) {
            return section;
        }
    }
    throw SymbolNotDefined(name);
}

void SymbolTable::updateSectionSize(const string& sectionName,
                                    int sectionSize) {
    for (auto&& section : sections) {
        if (section.name == sectionName) {
            section.size = sectionSize;
            return;
        }
    }
}

int SymbolTable::getCummulativeSectionSize() const {
    auto sum = 0;
    for (auto&& section : sections) {
        sum += section.size;
    }
    return sum;
}

void SymbolTable::setSymbolNumbers() {
    auto currentNumber = sections.size() + 1;
    for (auto&& symbol : symbols) {
        symbol.number = currentNumber;
        currentNumber++;
    }
}

ostream& operator<<(ostream& os, const SymbolTable& symbolTable) {
    os << "#tabela simbola" << endl;
    os << "#rbr\ttip\time\tsek\tvr\tvid\tvel" << endl;
    for (auto&& section : symbolTable.sections) {
        os << section.number << "\tSEK\t" << section.name << '\t'
           << section.number << '\t' << section.address << "\tL\t"
           << section.size << endl;
    }
    auto numOfSections = symbolTable.sections.size();
    for (auto&& symbol : symbolTable.symbols) {
        os << symbol.number << "\tSIM\t" << symbol.name << '\t'
           << symbol.section << '\t' << symbol.address << '\t'
           << symbolTable.getScopeDescription(symbol.scope) << endl;
    }
    return os;
}