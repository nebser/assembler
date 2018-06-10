#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

#include <iostream>
#include <string>
#include <vector>
#include "exceptions_a.h"
#include "utils.h"

class SymbolTable {
   public:
    enum Scope { GLOBAL, LOCAL };

    static const int UNKNOWN_SECTION;
    static const int UNKNOWN_ADDRESS;

    struct Symbol {
        std::string name;
        int section;
        Scope scope;
        int address;
        unsigned int number;

        Symbol(const std::string& name, int section, Scope scope, int address,
               unsigned int number)
            : name(name),
              section(section),
              scope(scope),
              address(address),
              number(number) {}
    };

    struct Section {
        std::string name;
        int size;
        int number;
        unsigned int address;

        Section(const std::string& name, unsigned int address,
                unsigned int number, int size)
            : name(name), address(address), size(size), number(number) {}
    };

    SymbolTable() { lastSection = 0; }

    void putSection(const std::string& name, unsigned int address);
    void putSymbol(const std::string& name, int address, Scope scope = LOCAL,
                   int section = -2);

    bool updateScope(const std::string& name, Scope newScope);
    bool symbolExists(const std::string& name) const;
    bool sectionExists(const std::string& name) const;

    const Symbol& getSymbol(const std::string&) const;
    const Section& getSection(const std::string&) const;

    const Section& getSection(unsigned int num) const {
        if (num > sections.size()) {
            throw SystemException("No section with number " +
                                  Utils::convertToString(num));
        }
        return sections[num];
    }

    void updateSectionSize(const std::string& sectionName, int sectionSize);
    int getCummulativeSectionSize() const;

    void setSymbolNumbers();

    friend std::ostream& operator<<(std::ostream& os,
                                    const SymbolTable& symbolTable);

   private:
    std::string getScopeDescription(Scope scope) const {
        switch (scope) {
            case LOCAL:
                return "L";
            case GLOBAL:
                return "G";
            default:
                return "UND";
        }
    }

    std::vector<Symbol> symbols;
    std::vector<Section> sections;
    int lastSection;
};

#endif