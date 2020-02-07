//
//  configOrder.h
//  myKconf
//
//  Created by david on 17/05/2017.
//  Copyright © 2017 david. All rights reserved.
//

#ifndef configOrder_h
#define configOrder_h

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <fstream>
#include <sstream>

#include "synExpDriver.hpp"

class ConfigOrder {
    
public:
    ConfigOrder(const std::string origVar, const std::string origExp,
                const std::string newVar , const std::string newExp);
    void    go();
    bool    nextGroup();
    
private:
    
    void    fillExpressionsVar();
    void    readInfo(const std::string& varFile, const std::string& expFile);
    void    writeInfo() ;
    void    addNewVariable(int v);
    void    addNewExpression(int e);
    bool    inNewVariables(int v);
    bool    inNewExpressions(int e);
    void    buildRelGroups();
    // We relate a set of ints (related variables) with the variable that relates:
    // if we have x -> y, x -> z, then {y,z} maps to {x}
    // if x1 -> y, x2 -> y, ..., xn -> y, then {y} maps to {x1, x2, ..., xn}
    std::map<std::set<int>, std::set<int> >         relGroups;
    std::map<std::set<int>, std::set<int> >         relExps;
    std::set<int>                                   blackList;
    std::vector<std::set<int> >                     relatedVars;
    std::vector<std::set<int> >                     expressions;
    std::vector< std::set<int> >                    expressionsVar;
    std::vector<std::string>                        variables;
    std::vector<int>                                pos2var, var2pos, newVariables, newExpressions;
    std::string                                     origVar, origExp, newVar, newExp;
    std::vector<synExp*>                            expList;
    std::map<std::string, int>                      varMap;
    int                                             ngroups;

};

class confOrderHelper {
public:
    confOrderHelper(std::set<int> first, std::set<int> second) :
        first(first), second(second){};
    std::set<int> first;
    std::set<int> second;
    bool operator<(const confOrderHelper& a) const {
        if (first.size() != a.first.size())
            return first.size() < a.first.size();
        return first < a.first;
    }
};
#endif /* configOrder_h */
