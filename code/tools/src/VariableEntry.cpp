//
//  VariableEntry.cpp
//  Kconfig2Logic
//
//  Created by David Fernandez Amoros on 11/03/2021.
//  Copyright © 2021 David Fernandez Amoros. All rights reserved.
//

#include "VariableEntry.hpp"

VariableEntry::VariableEntry(const std::string& originalConfig, synExp* value) :
    originalConfig(originalConfig), value(value) {}

VariableEntry::~VariableEntry() {
    for(auto x : incompatible)
        x->destroy();
    value->destroy();
    incompatible.clear();
}
std::string VariableEntry::getOriginalConfig()   { return originalConfig; }
synExp*     VariableEntry::getValue()            { return value;          }
void        VariableEntry::setValue(synExp* val) {
    value->destroy();
    value = val;           }

void  VariableEntry::incompatibleWith(const std::string& var) {
    incompatible.push_back(new synCompound(synNot,
                                           new synSymbol(var)));
}
vSynExp VariableEntry::getIncompatible() {
    return incompatible;
}
