#include "symbol_table.h"
#include <string>
#include <vector>
#include "exceptions_a.h"
using std::endl;
using std::ostream;
using std::string;
using std::vector;

const int SymbolTable::UNKNOWN_SECTION = -1;
const int SymbolTable::UNKNOWN_ADDRESS = -1;

void SymbolTable::putSection(const string& name) {
    for (auto&& section : sections) {
        if (section.name == name) {
            throw SymbolAlreadyDefinedException(section.name);
        }
    }
    sections.push_back(Section(name, 0, sections.size()));
    lastSection++;
}

void SymbolTable::putSymbol(const string& name, int address, Scope scope,
                            int section) {
    if (section == -2) {
        if (lastSection == -1) {
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

void SymbolTable::updateSectionSize(const string& sectionName,
                                    int sectionSize) {
    for (auto&& section : sections) {
        if (section.name == sectionName) {
            section.size = sectionSize;
            return;
        }
    }
}

ostream& operator<<(ostream& os, const SymbolTable& symbolTable) {
    os << "#tabela simbola" << endl;
    os << "#rbr\ttip\time\tsek\tvel|vr\tvid" << endl;
    for (auto&& section : symbolTable.sections) {
        os << section.number << "\tSEK\t" << section.name << '\t'
           << section.number << '\t' << section.size << "\tL" << endl;
    }
    auto numOfSections = symbolTable.sections.size();
    for (auto&& symbol : symbolTable.symbols) {
        os << (symbol.number + numOfSections) << "\tSIM\t" << symbol.name
           << '\t' << symbol.section << '\t' << symbol.address << '\t'
           << symbolTable.getScopeDescription(symbol.scope) << endl;
    }
    return os;
}