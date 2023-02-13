//
//  VariableTable.hpp
//  Kconfig2Logic
//
//  Created by David Fernandez Amoros on 11/03/2021.
//  Copyright © 2021 David Fernandez Amoros. All rights reserved.
//

#ifndef VariableTable_hpp
#define VariableTable_hpp

#include <stdio.h>
#include <map>

#include "VariableEntry.hpp"
class VariableTable {
public:
    static void         addVariable(const std::string& variable,
                                    const std::string& originalConfig);
    static std::string  getOriginalConfig(const std::string& variable);
    static synExp*      getValue(const std::string& variable);
    static void         setValue(const std::string& variable,
                                 synExp* value);
    static void         destroyTable();
    static std::unordered_set<std::string> getVariables();
    static bool         isDefined(const std::string& var);
    
    static void         add2Positive(const std::string& s,
                                     synExp* exp);
    static void         add2Negative(const std::string& s,
                                     synExp* exp);
    static void         del2Positive(const std::string& s,
                                     synExp* exp);
    static void         del2Negative(const std::string& s,
                                     synExp* exp);
    static std::unordered_set<synExp*>& getPositive(const std::string& s);
    static std::unordered_set<synExp*>& getNegative(const std::string& s);
    
    static void         addIncompatible(const std::string& var,
                                        const std::string& inc);
    static vSynExp      getIncompatible(const std::string& var);

protected:
    
    static std::unordered_map<std::string, VariableEntry*>           theMap;
    static std::unordered_set<synExp*>                                         emptySet;
    static std::unordered_map<std::string, std::unordered_set<synExp*>>        positive;
    static std::unordered_map<std::string, std::unordered_set<synExp*>>        negative;
};
#endif /* VariableTable_hpp */
