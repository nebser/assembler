#include "section.h"
#include <ostream>
using std::endl;
using std::ostream;

const Section& Section::writeRelData(ostream& os) const {
    if (type == BSS) {
        return *this;
    }
    os << "#.rel" << name << endl << "#ofset\ttip\tvrednost" << endl;
    for (auto&& r : relocations) {
        os << r;
    }
    return *this;
}

const Section& Section::writeContent(ostream& os) const {
    if (type == BSS) {
        return *this;
    }
    os << '#' << name << endl;
    auto counter = 0;
    for (auto&& ins : instructions) {
        counter = ins->write(os, counter);
    }
    if (counter % 16) {
        os << endl;
    }
    return *this;
}

Section::~Section() {
    for (auto&& ins : instructions) {
        delete ins;
    }
}