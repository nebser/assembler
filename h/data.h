#ifndef DATA_H_
#define DATA_H_

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

struct RelocationData {};

#endif