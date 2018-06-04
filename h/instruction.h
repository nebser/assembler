#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

#include <iostream>
#include <string>
#include <vector>
#include "symbol_table.h"
#include "tokenizer.h"

class WritableData {
   public:
    virtual int write(std::ostream&, int currentColumn) const = 0;
    virtual int getSize() const = 0;
};

class Instruction : public WritableData {
   public:
    virtual Instruction* decode(TokenStream&) = 0;
    virtual Instruction* evaluate(TokenStream&, const SymbolTable&) = 0;
    virtual const std::string& getName() const = 0;
};

class WritableDirective : public WritableData {
   public:
    virtual WritableDirective* decode(TokenStream&);
    virtual int write(std::ostream&, int currentColumn) const;
};

class Definition : public WritableDirective {
   public:
    Definition(const std::string& name, int multiplier)
        : name(name), multiplier(multiplier) {}

    WritableDirective* decode(TokenStream&) override;

    int write(std::ostream&, int currentColumn) const override;

    int getSize() const override {
        return datas.size() == 0 ? multiplier : multiplier * datas.size();
    }

   private:
    std::string name;
    int multiplier;
    std::vector<int> datas;
};

class SkipDirective : public WritableDirective {
   public:
    SkipDirective() { fill = 0; }

    WritableDirective* decode(TokenStream&) override;

    int write(std::ostream&, int currentColumn) const override;

    int getSize() const override { return size; }

   private:
    int size;
    char fill;
};

#endif