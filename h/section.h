#ifndef SECTION_H_
#define SECTION_H_

#include <ostream>
#include <string>
#include <vector>
#include "data.h"
#include "exceptions_a.h"
#include "instruction.h"

class Section {
   public:
    enum Type { RODATA, DATA, TEXT, BSS };

    Section(const std::string& name, Type type, unsigned int address)
        : name(name), type(type), address(address) {}

    Type getType() const { return type; }

    std::string getName() const { return name; }

    void addIstruction(const WritableData* instruction) {
        if (type == BSS && dynamic_cast<const WritableDirective*>(instruction)
                               ->initialized()) {
            throw DecodingException(
                "BSS section can only contain uninitalized data");
        }
        instructions.push_back(instruction);
    }

    void addRelocationData(const RelocationData& relData) {
        relocations.push_back(relData);
    }

    const Section& writeRelData(std::ostream&) const;
    const Section& writeContent(std::ostream&) const;

   private:
    Type type;
    std::string name;
    unsigned int address;

    std::vector<const WritableData*> instructions;
    std::vector<RelocationData> relocations;
};

#endif