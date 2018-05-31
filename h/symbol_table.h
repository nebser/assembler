#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

#include <iostream>
#include <string>
#include <vector>

class SymbolTable {
   public:
    enum Scope { GLOBAL, LOCAL };

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

        Section(const std::string& name, int size, unsigned int number)
            : name(name), size(size), number(number) {}
    };

    SymbolTable() { lastSection = -1; }

    void putSection(const std::string& name);
    void putSymbol(const std::string& name, int address, Scope scope = LOCAL,
                   int section = -1);

    bool updateScope(const std::string& name, Scope newScope);
    bool symbolExists(const std::string& name) const;
    bool sectionExists(const std::string& name) const;

    void updateSectionSize(const std::string& sectionName, int sectionSize);

    friend std::ostream& operator<<(std::ostream& os,
                                    const SymbolTable& symbolTable);

   private:
    std::vector<Symbol> symbols;
    std::vector<Section> sections;
    int lastSection;
};

#endif