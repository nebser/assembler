#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

#include <iostream>
#include <string>
#include <vector>
#include "data.h"
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
    virtual RelocationData* evaluate(const SymbolTable&,
                                     int instructionLocation,
                                     const std::string& mySection) = 0;
    virtual ~Instruction() {}
};

class WritableDirective : public WritableData {
   public:
    virtual WritableDirective& decode(TokenStream&) = 0;
    virtual bool initialized() const = 0;
    virtual ~WritableDirective() {}
};

class Definition : public WritableDirective {
   public:
    Definition(const std::string& name, int multiplier)
        : name(name), multiplier(multiplier) {}

    WritableDirective& decode(TokenStream&) override;

    std::vector<RelocationData> evaluate(const SymbolTable&,
                                         int locationCounter,
                                         const std::string& section);

    bool initialized() const override { return datas.size() != 0; }

    int write(std::ostream&, int address) const override;

    int getSize() const override {
        return (datas.size() == 0 ? multiplier : multiplier * datas.size()) * 8;
    }

   private:
    std::string name;
    int multiplier;
    std::vector<Operand> datas;
};

class SkipDirective : public WritableDirective {
   public:
    SkipDirective() { fill = 0; }

    WritableDirective& decode(TokenStream&) override;

    bool initialized() const override { return fill != 0; }

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

    bool initialized() const override { return fill != 0; }

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
    SingleAddressInstruction(const std::string& name, unsigned char opcode,
                             bool dstExists)
        : name(name), opcode(opcode), dstExists(dstExists), operand(nullptr) {}

    ~SingleAddressInstruction() { free(); }

    SingleAddressInstruction(const SingleAddressInstruction& sai) { copy(sai); }

    SingleAddressInstruction(SingleAddressInstruction&& sai) { move(sai); }

    SingleAddressInstruction& operator=(const SingleAddressInstruction& sai) {
        if (&sai != this) {
            free();
            copy(sai);
        }
        return *this;
    }

    SingleAddressInstruction& operator=(SingleAddressInstruction&& sai) {
        if (&sai != this) {
            free();
            move(sai);
        }
        return *this;
    }

    Instruction& decode(TokenStream&) override;
    RelocationData* evaluate(const SymbolTable& symbolTable,
                             int instructionLocation,
                             const std::string& mySection) override {
        return operand->evaluate(symbolTable, instructionLocation + 2,
                                 instructionLocation + 4, mySection);
    }
    int getSize() const override { return 11 + operand->getSize(); }

    int write(std::ostream&, int currentColumn) const override;

   private:
    void copy(const SingleAddressInstruction& sai) {
        operand = new Operand(*sai.operand);
        name = sai.name;
        opcode = sai.opcode;
        dstExists = sai.dstExists;
    }

    void move(SingleAddressInstruction& sai) {
        operand = sai.operand;
        name = sai.name;
        opcode = sai.opcode;
        dstExists = sai.dstExists;
        sai.operand = nullptr;
    }

    void free() {
        delete operand;
        operand = nullptr;
    }

    Operand* operand;
    std::string name;
    unsigned char opcode;
    bool dstExists;
};

class DoubleAddressInstruction : public Instruction {
   public:
    DoubleAddressInstruction(const std::string& name, unsigned char opcode)
        : name(name), opcode(opcode), dst(nullptr), src(nullptr) {}

    ~DoubleAddressInstruction() { free(); }

    DoubleAddressInstruction(const DoubleAddressInstruction& dai) { copy(dai); }

    DoubleAddressInstruction(DoubleAddressInstruction&& dai) { move(dai); }

    DoubleAddressInstruction& operator=(const DoubleAddressInstruction& dai) {
        if (&dai != this) {
            free();
            copy(dai);
        }
        return *this;
    }

    DoubleAddressInstruction& operator=(DoubleAddressInstruction&& dai) {
        if (&dai != this) {
            free();
            move(dai);
        }
        return *this;
    }

    Instruction& decode(TokenStream&) override;
    RelocationData* evaluate(const SymbolTable& symbolTable,
                             int instructionLocation,
                             const std::string& mySection) override {
        return dst->getSize() > src->getSize()
                   ? dst->evaluate(symbolTable, instructionLocation + 2,
                                   instructionLocation + 4, mySection)
                   : src->evaluate(symbolTable, instructionLocation + 2,
                                   instructionLocation + 4, mySection);
    }
    int getSize() const override { return 6 + dst->getSize() + src->getSize(); }

    int write(std::ostream&, int currentColumn) const override;

   private:
    void copy(const DoubleAddressInstruction& dai) {
        dst = new Operand(*dai.dst);
        src = new Operand(*dai.src);
        opcode = dai.opcode;
        name = dai.name;
    }

    void move(DoubleAddressInstruction& dai) {
        dst = dai.dst;
        src = dai.src;
        opcode = dai.opcode;
        name = dai.name;
        dai.dst = nullptr;
        dai.src = nullptr;
    }

    void free() {
        delete src;
        src = nullptr;
        delete dst;
        dst = nullptr;
    }

    std::string name;
    Operand* dst;
    Operand* src;
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

    RelocationData* evaluate(const SymbolTable&, int instructionLocation,
                             const std::string& mySection) override {
        return nullptr;
    }

    int getSize() const override { return 16; }

    int write(std::ostream& os, int currentColumn) const override {
        return Utils::writeInstruction(os, opcode << 26, 2, currentColumn);
    }

   private:
    std::string name;
    unsigned char opcode;
};

class RetInstruction : public Instruction {
   public:
    RetInstruction(const std::string& name, unsigned char opcode)
        : name(name), opcode(opcode) {}

    Instruction& decode(TokenStream& tokenStream) override {
        if (tokenStream.next().getType() != Token::LINE_DELIMITER) {
            throw DecodingException("Invalid format for ret instruction");
        }
        return *this;
    }

    RelocationData* evaluate(const SymbolTable&, int instructionLocation,
                             const std::string& mySection) override {
        return nullptr;
    }

    int write(std::ostream& os, int currentColumn) const override {
        return Utils::writeInstruction(os, opcode << 26 | 0xF << 21, 2,
                                       currentColumn);
    }

    int getSize() const override { return 16; }

   private:
    std::string name;
    unsigned char opcode;
};

class JmpInstruction : public Instruction {
   public:
    JmpInstruction(const std::string& name, unsigned char prefix)
        : name(name), prefix(prefix), opcode(0) {}

    ~JmpInstruction() { delete operand; }

    JmpInstruction(const JmpInstruction& jmpi) { copy(jmpi); }

    JmpInstruction(JmpInstruction&& jmpi) { move(jmpi); }

    JmpInstruction& operator=(const JmpInstruction& jmpi) {
        if (&jmpi != this) {
            free();
            copy(jmpi);
        }
        return *this;
    }

    JmpInstruction& operator=(JmpInstruction&& jmpi) {
        if (&jmpi != this) {
            free();
            move(jmpi);
        }
        return *this;
    }

    Instruction& decode(TokenStream&) override;

    RelocationData* evaluate(const SymbolTable& symbolTable,
                             int instructionLocation,
                             const std::string& mySection) override {
        return operand->evaluate(symbolTable, instructionLocation + 2,
                                 instructionLocation + 4, mySection);
    }

    int write(std::ostream&, int currentColumn) const override;

    int getSize() const override { return operand->getSize() + 11; }

   private:
    void copy(const JmpInstruction& jmpi) {
        operand = new Operand(*jmpi.operand);
        name = jmpi.name;
        opcode = jmpi.opcode;
        prefix = jmpi.prefix;
    }

    void move(JmpInstruction& jmpi) {
        operand = jmpi.operand;
        name = jmpi.name;
        opcode = jmpi.opcode;
        prefix = jmpi.prefix;
        jmpi.operand = nullptr;
    }

    void free() {
        delete operand;
        operand = nullptr;
    }

    std::string name;
    unsigned char prefix;
    unsigned char opcode;
    Operand* operand;
};

#endif