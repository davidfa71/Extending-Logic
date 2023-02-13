//
//  VariableEntry.hpp
//  Kconfig2Logic
//
//  Created by David Fernandez Amoros on 11/03/2021.
//  Copyright © 2021 David Fernandez Amoros. All rights reserved.
//

#ifndef VariableEntry_hpp
#define VariableEntry_hpp

#include <stdio.h>
#include <string>
#include "synExp.hpp"

class VariableEntry {
public:
    VariableEntry(const std::string& originalConfig,
                  synExp* value);
    ~VariableEntry();
    std::string getOriginalConfig();
    synExp*     getValue();
    void        setValue(synExp* val);
    void        incompatibleWith(const std::string& var);
    vSynExp     getIncompatible();
protected:
    std::string originalConfig;
    vSynExp     incompatible;
    synExp*     value;
};


#endif /* VariableEntry_hpp */
