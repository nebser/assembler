#include "symbol_table.h"
#include <string>
#include <vector>
#include "exceptions_a.h"
using std::string;
using std::vector;

void SymbolTable::putSection(const string& name) {
    for (auto&& section : sections) {
        if (section.name == name) {
            throw SymbolAlreadyDefinedException(section.name);
        }
    }
    sections.push_back(Section(name, 0));
    lastSection++;
}

void SymbolTable::putSymbol(const string& name, int address, Scope scope,
                            int section = -1) {
    if (section == -1) {
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
    symbols.push_back(Symbol(name, address, scope, section));
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
    for (auto& section : sections) {
        if (section.name == sectionName) {
            section.size = sectionSize;
            return;
        }
    }
}