#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

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

        Symbol(const std::string& name, int section, Scope scope, int address)
            : name(name), section(section), scope(scope), address(address) {}
    };

    struct Section {
        std::string name;
        int size;

        Section(const std::string& name, int size) : name(name), size(size) {}
    };

    SymbolTable() { lastSection = -1; }

    void putSection(const std::string& name);
    void putSymbol(const std::string& name, int address, Scope scope = LOCAL,
                   int section = -1);

    bool updateScope(const std::string& name, Scope newScope);
    bool symbolExists(const std::string& name) const;
    bool sectionExists(const std::string& name) const;

    void updateSectionSize(const std::string& sectionName, int sectionSize);

   private:
    std::vector<Symbol> symbols;
    std::vector<Section> sections;
    int lastSection;
};

#endif