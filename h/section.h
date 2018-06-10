#ifndef SECTION_H_
#define SECTION_H_

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

    void addIstruction(WritableData* instruction) {
        if (type == BSS &&
            dynamic_cast<WritableDirective*>(instruction)->initialized()) {
            throw DecodingException(
                "BSS section can only contain uninitalized data");
        }
        instructions.push_back(instruction);
    }

   private:
    Type type;
    std::string name;
    unsigned int address;

    std::vector<WritableData*> instructions;
};

#endif