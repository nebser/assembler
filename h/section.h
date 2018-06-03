#ifndef SECTION_H_
#define SECTION_H_

#include <string>
#include <vector>
#include "command.h"
#include "instruction.h"

class Section {
   public:
    enum Type { RODATA, DATA, TEXT, BSS };

    Section(const std::string& name, Type type) : name(name), type(type) {}

    Type getType() const { return type; }

    std::string getName() const { return name; }

    bool isValidCommand(const Command& comm) const {
        switch (type) {
            case RODATA:
            case DATA:
            case BSS:
                if (comm.type != Command::SKIP_DIR &&
                    comm.type != Command::ALIGN_DIR &&
                    comm.type != Command::DEFINITION &&
                    comm.type != Command::LABEL) {
                    return false;
                }
                break;
            case TEXT:
                if (comm.type != Command::INSTRUCTION &&
                    comm.type != Command::LABEL &&
                    comm.type != Command::ALIGN_DIR &&
                    comm.type != Command::SKIP_DIR) {
                    return false;
                }
                break;
        }
        return true;
    }

    void addIstruction(WritableData* instruction) {
        instructions.push_back(instruction);
    }

   private:
    Type type;
    std::string name;

    std::vector<WritableData*> instructions;
};

#endif