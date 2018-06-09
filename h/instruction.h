#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

#include <iostream>
#include <string>
#include <vector>
#include "operand.h"
#include "symbol_table.h"
#include "tokenizer.h"

class WritableData {
   public:
    virtual int write(std::ostream&, int currentColumn) const = 0;
    virtual int getSize() const = 0;
    virtual ~WritableData() {}
};

class Instruction : public WritableData {
   public:
    virtual Instruction& decode(TokenStream&) = 0;
    // virtual Instruction& evaluate(const SymbolTable&) = 0;
    virtual ~Instruction() {}
};

class WritableDirective : public WritableData {
   public:
    virtual WritableDirective& decode(TokenStream&) = 0;
    virtual ~WritableDirective() {}
};

class Definition : public WritableDirective {
   public:
    Definition(const std::string& name, int multiplier)
        : name(name), multiplier(multiplier) {}

    WritableDirective& decode(TokenStream&) override;

    int write(std::ostream&, int currentColumn) const override;

    int getSize() const override {
        return (datas.size() == 0 ? multiplier : multiplier * datas.size()) * 8;
    }

   private:
    std::string name;
    int multiplier;
    std::vector<int> datas;
};

class SkipDirective : public WritableDirective {
   public:
    SkipDirective() { fill = 0; }

    WritableDirective& decode(TokenStream&) override;

    int write(std::ostream&, int currentColumn) const override;

    int getSize() const override { return size * 8; }

   private:
    int size;
    char fill;
};

class AlignDirective : public WritableDirective {
   public:
    AlignDirective() {
        fill = 0;
        maxPadd = int((unsigned int)~0 >> 1);
    }

    AlignDirective& decode(TokenStream&) override;

    AlignDirective& evaluate(int currentLocationCounter);

    int write(std::ostream&, int currentColumn) const override;

    int getSize() const override { return size * 8; }

   private:
    int padd;
    int fill;
    int maxPadd;
    int size;
};

class SingleAddressInstruction : public Instruction {
   public:
    SingleAddressInstruction(const std::string& name, unsigned char opcode)
        : name(name), opcode(opcode) {}

    Instruction& decode(TokenStream&) override;
    // Instruction& evaluate(const SymbolTable&) override;
    int getSize() const override { return 11 + operand.getSize(); }

    int write(std::ostream&, int currentColumn) const override;

   private:
    Operand operand;
    std::string name;
    unsigned char opcode;
};

class DoubleAddressInstruction : public Instruction {
   public:
    DoubleAddressInstruction(const std::string& name, unsigned char opcode)
        : name(name), opcode(opcode) {}

    Instruction& decode(TokenStream&) override;
    // Instruction& evaluate(const SymbolTable&) override;
    int getSize() const override { return 11 + dst.getSize() + src.getSize(); }

    int write(std::ostream&, int currentColumn) const override;

   private:
    std::string name;
    Operand dst, src;
    unsigned char opcode;
};

class NoAddressInstruction : public Instruction {
   public:
    NoAddressInstruction(const std::string& name, unsigned char opcode)
        : name(name), opcode(opcode) {}

    Instruction& decode(TokenStream& tokenStream) override {
        if (tokenStream.end()) {
            throw DecodingException("Invalid end of file at instruction " +
                                    name);
        }
        auto t = tokenStream.next();
        if (t.getType() != Token::LINE_DELIMITER) {
            throw DecodingException("Invalid token after instruction " + name +
                                    " " + t.getTypeDescription());
        }
        return *this;
    }

    int getSize() const override { return 16; }

    int write(std::ostream& os, int currentColumn) const override {
        return Utils::writeData(os, opcode << 11, 2, currentColumn);
    }

   private:
    std::string name;
    unsigned char opcode;
};

#endif