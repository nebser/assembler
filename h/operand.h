#ifndef OPERAND_H
#define OPERAND_H

#include <string>
#include <vector>
#include "data.h"
#include "symbol_table.h"
#include "token.h"

enum AddressMode {
    IMMEDIATE_SYMBOL,
    IMMEDIATE_CONSTANT,
    PSW,
    REG_DIRECT,
    MEMORY_SYMBOL,
    MEMORY_CONSTANT,
    REG_INDIRECT_W_DISPL,
    PC_RELATIVE
};

class Operand {
   public:
    // Operand() : constantDataRaw(UNDEFINED_TOKEN), constantData(0) {}

    Operand(const std::vector<Token>&,
            const std::vector<AddressMode>& invalidAddressModes = {});

    Operand(const Token& token,
            const std::vector<AddressMode>& invalidAddressModes = {});

    AddressMode getAddressMode() const { return addressMode; }

    int getSize() const {
        switch (addressMode) {
            case IMMEDIATE_SYMBOL:
            case IMMEDIATE_CONSTANT:
            case MEMORY_SYMBOL:
            case MEMORY_CONSTANT:
            case REG_INDIRECT_W_DISPL:
            case PC_RELATIVE:
                return 21;
            case PSW:
            case REG_DIRECT:
                return 5;
        }
    }

    int getCode() const {
        switch (addressMode) {
            case IMMEDIATE_CONSTANT:
            case IMMEDIATE_SYMBOL:
                return constantData;
            case PSW:
                return 0x07;
            case REG_DIRECT:
                return 0x08 | registryData;
            case MEMORY_SYMBOL:
            case MEMORY_CONSTANT:
                return (0x02 << 19) | constantData;
            case REG_INDIRECT_W_DISPL:
                return (0x03 << 19) | (registryData << 16) | constantData;
            case PC_RELATIVE:
                return (0x1F << 16) | (0xFFFF & constantData);
        }
    }

    int getRegData() const {
        auto size = getSize();
        auto code = getCode();
        return size == 5 ? code : code >> 16;
    }

    int getConstantData() const { return constantData & 0xFFFF; }

    int getFullConstantData() const { return constantData; }

    RelocationData* evaluate(const SymbolTable&, int myLocation,
                             int nextInstructionLocation,
                             const std::string& mySection);

   private:
    struct Registry {
        std::string name;
        unsigned char code;

        Registry(const std::string& name, unsigned char code)
            : name(name), code(code) {}
    };

    static const std::vector<Registry> registries;
    static std::vector<Registry> constructRegistries();
    static int getRegistry(const std::string&);

    bool isIllegalAddressMode(const std::vector<AddressMode>&) const;

    void determineOperand(const std::vector<Token>&);

    AddressMode addressMode;
    int registryData;
    int constantData;
    Token constantDataRaw;
};

#endif