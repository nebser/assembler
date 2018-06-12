#ifndef DATA_H_
#define DATA_H_

#include <iostream>
#include <string>

struct Command {
    enum Type {
        GLOBAL_DIR,
        INSTRUCTION,
        END_DIR,
        DEFINITION,
        SECTION,
        ALIGN_DIR,
        SKIP_DIR,
        LABEL,
        EMPTY
    };

    std::string name;
    Type type;

    Command(const std::string& name, Type type) : name(name), type(type) {}
};

const auto DUMMY_COMMAND = Command("empty", Command::EMPTY);

class RelocationData {
   public:
    enum Type { APSOLUTE, RELATIVE };

    RelocationData(unsigned int offset, Type type, unsigned int value,
                   unsigned int nextInstructionAddress = 0)
        : offset(offset),
          type(type),
          value(value),
          nextInstructionAddress(nextInstructionAddress) {}

    unsigned int getOffset() const { return offset; }

    Type getType() const { return type; }

    std::string getTypeDescription() const {
        return type == APSOLUTE ? "R_386_32" : "R_386_PC32";
    }

    unsigned int getValue() const { return value; }

    friend std::ostream& operator<<(std::ostream& os,
                                    const RelocationData& relData) {
        os << std::hex << "0x" << relData.offset << '\t'
           << relData.getTypeDescription() << '\t' << std::dec << relData.value;
        if (relData.type == RelocationData::RELATIVE) {
            os << '\t' << std::hex << "0x" << relData.nextInstructionAddress;
        }
        os << std::endl;
        return os;
    }

   private:
    unsigned int offset;
    Type type;
    unsigned int value;
    unsigned int nextInstructionAddress;
};

#endif