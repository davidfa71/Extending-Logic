//
//  configOrder.cpp
//  myKconf
//
//  Created by david on 17/05/2017.
//  Copyright © 2017 david. All rights reserved.
//

#include <stdio.h>
#include "ConfigOrder.hpp"


ConfigOrder::ConfigOrder(const std::string SorigVar, const std::string SorigExp,
                         const std::string SnewVar , const std::string SnewExp) : origVar(SorigVar), origExp(SorigExp), newVar(SnewVar), newExp(SnewExp) {
    std::cerr << "SorigVar " << SorigVar <<  std::endl;
    std::cerr << "origVar " << origVar <<  std::endl;
    std::cerr << "origExp " << origExp << std::endl;
       readInfo(origVar, origExp);
};

void ConfigOrder::writeInfo() {
    std::ofstream vo(newVar.c_str());
    std::ofstream eo(newExp.c_str());
    for(int v : newVariables)
        vo << variables.at(v) << " ";
    for(int e : newExpressions)
        eo << expList.at(e) << std::endl;
    vo.close();
    eo.close();
}

void ConfigOrder::readInfo(const std::string& varFile, const std::string& expFile) {
    // We read the variables in order
    std::cerr << "Opening file " << varFile << std::endl;
    std::ifstream vf(varFile.c_str());
    std::cerr << "Opening file " << expFile << std::endl;
    std::ifstream ef(expFile.c_str());
    std::string temp;
    while (vf >> temp)
        variables.push_back(temp);
    
    // We parse the expression file
    kconf::synExpDriver driver;
    driver.parse_file(expFile);
    expList  = driver.getConstraints();
    // We initialize the order and the inverse mapping
    for(int x = 0; x < variables.size(); x++) {
        var2pos.push_back(x);
        pos2var.push_back(x);
        varMap[variables.at(x)] = x;
    }
    //std::cerr << "Read " << variables.size() << " variables " << std::endl;
    //exit(-1);
    // Now we go with the expressions
    for(synExp *s : expList) {
        s->computeIndices(varMap);
        std::set<int> vtemp;
        for(int i : s->giveSymbolIndices())
            vtemp.insert(i);
        expressions.push_back(vtemp);
    }
    fillExpressionsVar();
    buildRelGroups();
}

void ConfigOrder::fillExpressionsVar() {
    // For each var we make a list of the related expressions and variables
    expressionsVar.resize(variables.size());
    relatedVars.resize(variables.size());

    for(int exp = 0; exp < expressions.size(); exp++) {
        for(auto var : expressions.at(exp))
            expressionsVar.at(var).insert(exp);
        for(auto var1 : expressions.at(exp))
            for(auto var2 : expressions.at(exp))
                if (var1 != var2) {
                    relatedVars[var1].insert(var2);
                    relatedVars[var2].insert(var1);
                }
        }
}

bool    ConfigOrder::inNewVariables(int v) {
    return std::find(newVariables.begin(), newVariables.end(), v) != newVariables.end();
}
bool    ConfigOrder::inNewExpressions(int e) {
    return std::find(newExpressions.begin(), newExpressions.end(), e) != newExpressions.end();
}
void    ConfigOrder::addNewVariable(int v) {
    if (!inNewVariables(v))
        newVariables.push_back(v);
}
void    ConfigOrder::addNewExpression(int e) {
    if (!inNewExpressions(e))
        newExpressions.push_back(e);
}

void ConfigOrder::go() {
    ngroups = 0;
    while (nextGroup());
    std::cout << "We found " << ngroups << " groups" << std::endl;
    writeInfo();

}
bool ConfigOrder::nextGroup() {
    // Find variable with the most constraints
    int max = -1, size;
    int maxVar;
    for(int i = 0; i < variables.size(); i++)
        if (! inNewVariables(i) &&
            blackList.find(i) == blackList.end() &&
            (size = expressionsVar.at(i).size()) > max) {
            max = size;
            maxVar = i;
        }
    if (max == -1 || max == 0) return false;

    std::cout << "The variable with most constraints is " << std::setw(5) << maxVar
    << " " << std::setw(40) << variables.at(maxVar) << " with " << max << " constraints. ";
    int easyExp   = 0;
    int easyVar   = 0;
    int mediumVar = 0;
    int mediumExp = 0;
    int hardExp   = 0;
    int tempVar   = 0;
    
    for(int e : expressionsVar.at(maxVar)) {
        bool ok = true;
        tempVar =0;
        for(int v : expressions.at(e))
            if (v != maxVar) {
                if (expressionsVar.at(v).size() == 1)
                    tempVar++;
                else
                    ok = false;
            }
        
        if (ok) {
            easyExp++;
            easyVar += tempVar;
            addNewExpression(e);
            for(int v : expressions.at(e))
                if (v != maxVar)
                    addNewVariable(v);
        }
    }
    std::cout << "Found " << easyExp << " easy constraints involving "
    << easyVar << " easy vars" << std::endl;
    
//    for(int e : expressionsVar.at(maxVar)) {
//        bool ok = true;
//        tempVar =0;
//        if (!inNewExpressions(e)) {
//            for(int v : expressions.at(e))
//                if (v != maxVar) {
//                    int notRelated = 0;
//                    bool related = true;
//                    for(auto ee : expressionsVar.at(v))
//                        for(int vv : expressions.at(ee))
//                            if (vv != maxVar && relatedVars.at(vv).find(maxVar) == relatedVars.at(vv).end()) {
//                                notRelated++;
//                                related = false;
//                            }
//                    if (related) {
//                        newExpressions.push_back(e);
//                        mediumExp++;
//                        for(auto v : expressions.at(e)) {
//                            if (!inNewVariables(v))
//                                mediumVar++;
//                            
//                            addNewVariable(v);
//                            }
//                        }
//                    else {
//                        //std::cout << "Hard constraint with " << notRelated << " unrelated variables: " << expList.at(e) << std::endl;
//                        hardExp++;
//                    }
//                    
//            }
//        }
//    }
    

 //   std::cerr << "Found " << mediumExp << " medium constraints involving "
 //             << mediumVar << " medium variables and "  << hardExp << " hard constraints" << std::endl;
    if (easyVar > 0 || mediumVar > 0 || expressionsVar[maxVar].size() < 2 ) {
        newVariables.push_back(maxVar);
        ngroups++;
        //std::cerr << "* ngroups" << ngroups << std::endl;
        return true;
    }
    //if (expressionsVar.at(maxVar).size() < 3) {
    //    for
    //}

    //std::cerr << "Blacklisting" << std::endl;
    blackList.insert(maxVar);
    return true;
}

void    ConfigOrder::buildRelGroups() {
    std::cout << "Building relGroups" << std::endl;
    for(int variable = 0; variable < variables.size(); variable++) {
        relGroups[relatedVars.at(variable)].insert(variable);
        relExps[relatedVars.at(variable)];
        for(auto exp : expressionsVar.at(variable))
            relExps[relatedVars.at(variable)].insert(exp);
    }
    
    std::list<confOrderHelper> o;
    for(std::pair<std::set<int> , std::set<int> > p : relGroups) {
        confOrderHelper coh(p.first,  p.second);
        o.push_back(coh);
    }
    o.sort();
    int streak = 0;
    int last   = -1;
    for(auto p : o) {
        if (p.first.size() != last) {
            std::cout << "streak of size " << last << " is " << streak << std::endl;
            last   = p.first.size();
            streak = p.second.size();
        }
        else streak += p.second.size();
        std::cout << "size " << p.first.size() << " relSize " << p.second.size() << std::endl;
    }
    std::cout << "Found " << o.size() << " related groups" << std::endl;
    for(auto p : o) {
        for(auto v : p.second)
            newVariables.push_back(v);
        try {
        for(auto e : relExps.at(p.first))
            newExpressions.push_back(e);
        }
        catch(std::exception e) {
            std::cerr << "Error looking for related expressions of first: ";
            for(auto v : p.first)
                std::cerr << v << " ";
            std::cerr << std::endl;
        }
    }
    writeInfo();
    exit(0);
}
