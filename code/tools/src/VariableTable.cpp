//
//  VariableTable.cpp
//  Kconfig2Logic
//
//  Created by David Fernandez Amoros on 11/03/2021.
//  Copyright © 2021 David Fernandez Amoros. All rights reserved.
//


#include "VariableTable.hpp"

std::unordered_map<std::string, VariableEntry*>                     VariableTable::theMap;
std::unordered_set<synExp*>                                         VariableTable::emptySet;
std::unordered_map<std::string, std::unordered_set<synExp*>>        VariableTable::positive;
std::unordered_map<std::string, std::unordered_set<synExp*>>        VariableTable::negative;

bool         VariableTable::isDefined(const std::string& var) {
    return theMap.find(var) != theMap.end();
}
void         VariableTable::addVariable(const std::string& variable,
                                        const std::string& originalConfig) {
    //std::cerr << "Adding variable " << variable << " to VariableTable" << std::endl;
    if (theMap.find(variable) != theMap.end())
        delete theMap[variable];
    theMap[variable] = new VariableEntry(originalConfig,
                                         new synSymbol(variable));
}
std::string  VariableTable::getOriginalConfig(const std::string& variable) {
    if (theMap.find(variable) != theMap.end())
        return theMap[variable]->getOriginalConfig();
    else {// It is an original string or bitmap config or a choice
        return variable;
    }
}
synExp*      VariableTable::getValue(const std::string& variable) {
    if (theMap.find(variable) != theMap.end())
        return theMap[variable]->getValue();
    else
        return NULL;
}
void         VariableTable::setValue(const std::string& variable,
                                     synExp* value) {
    if (theMap.find(variable) != theMap.end()) {
        if ((theMap[variable]->getValue() == synTrue && value == synFalse) ||
            (theMap[variable]->getValue() == synFalse && value == synTrue))
            throw std::logic_error(variable+" is true and false at the same time");
    }
#ifdef _DEBUG_
    std::cerr << "Setting " << variable << " to " << value << " in variable table " << std::endl;
#endif
    try {
      theMap.at(variable)->setValue(value);
    }
    catch (std::exception e) {
      std::ostringstream ost;
      ost << "Variable " << variable << " was not found at theMap " << std::endl;
      throw std::logic_error(ost.str());
    }
} 
void  VariableTable::destroyTable() {
    for(auto p : theMap) {
        delete p.second;
    }
}

std::unordered_set<std::string>   VariableTable::getVariables() {
    std::unordered_set<std::string> res;
    for(auto p : theMap)
        res.insert(p.first);
    return res;
}

void   VariableTable::add2Positive(const std::string& s,
                                   synExp* exp) {
    bool found = false;
    for(auto x : positive[s])
        if (equal(x, exp)) {
            found = true;
            break;
        }
    if (!found)
        positive[s].insert(exp);
}
void   VariableTable::add2Negative(const std::string& s,
                                   synExp* exp) {
    bool found = false;
       for(auto x : negative[s])
           if (equal(x, exp)) {
               found = true;
               break;
           }
       if (!found)
           negative[s].insert(exp);
    
}
void  VariableTable::del2Positive(const std::string& s,
                                  synExp* exp) {
    for(auto x : positive[s])
        if (equal(x, exp)) {
            positive[s].erase(x);
            return;
        }
}
void   VariableTable::del2Negative(const std::string& s,
                                   synExp* exp) {
    for(auto x : negative[s])
        if (equal(x, exp)) {
            negative[s].erase(x);
            return;
        }
    
}
std::unordered_set<synExp*>& VariableTable::getPositive(const std::string& s) {
    std::unordered_map<std::string, std::unordered_set<synExp*>>::iterator
    itm = positive.find(s);
    if (itm != positive.end())
        return itm->second;
    
    return emptySet;
}
std::unordered_set<synExp*>& VariableTable::getNegative(const std::string& s) {
    std::unordered_map<std::string, std::unordered_set<synExp*>>::iterator
    itm = negative.find(s);
    if (itm != negative.end())
        return itm->second;
    
    return emptySet;
}

void   VariableTable::addIncompatible(const std::string& var,
                                      const std::string& inc) {
    if (theMap.find(var) != theMap.end())
        theMap[var]->incompatibleWith(inc);
    else {
        std::ostringstream ost;
        ost << "Variable " << var << " is not created in VariableTable" << std::endl;
        throw std::logic_error(ost.str());
    }
}

vSynExp VariableTable::getIncompatible(const std::string& var) {
    return theMap[var]->getIncompatible();
}
