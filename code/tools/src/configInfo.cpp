//
//  configInfo.cpp
//  myKconf
//
//  Created by david on 28/11/13.
//  Copyright (c) 2013 david. All rights reserved.
//

#include "configInfo.hpp"
#include <iomanip>
bool                                             configInfo::useXOR = false;
std::map<std::string, std::map<std::string, std::vector<selectHelper>>> configInfo::mapSelect;
std::map<std::string, std::list<configInfo*> >   configInfo::mapAlternative;
int                                              configInfo::artificialCount = 0;
std::map<std::string, std::set<std::string>>     configInfo::stringAlternatives;
std::map<std::string, std::set<std::string>>     configInfo::stringValues;
std::map<std::string, int>                       configInfo::timesProcessed;
std::map<std::pair<std::string, std::string>, int> configInfo::timesDependsOn;

synExp*     selectHelper::getExpression() { return expression->copy_exp(); }
synExp*     selectHelper::getDep()        { return c->getDependsVar()->copy_exp();        }
//synExp*     selectHelper::getDep()        { return dep->copy_exp();        }
//synExp*     selectHelper::getDep()        {
//    std::cerr << "In GetDep, c "    << (void*) c       << std::endl;
//    std::cerr << "In GetDep, name " << c->getName() << std::endl;
//
//    std::cerr << "In getDep, returning " << dep << " but could have been "
//                << c->getDependsVar()->copy_exp() << std::endl;
//    return dep->copy_exp();        }
std::string selectHelper::getVar()        { return variable;               }
configInfo* selectHelper::getConfig()     { return c;                      }



void configInfo::addAlternative(std::string name, configInfo* val) {
    mapAlternative[name].push_front(val);


}

void configInfo::addMapSelect(const std::string& x, synExp* e, synExp* dep) {
        configsISelect.insert(x);
        selectHelper h(name, e, dep, this);
        mapSelect[x][name].push_back(h);
}

std::list<synExp*> configInfo::giveConstraint() {
    timesProcessed[name]++;
    if (vartype == "boolean" || vartype == "tristate")
        return giveLogicalConstraint();
    else
        if (vartype == "string" || vartype == "int" || vartype == "hex")
            return giveStringConstraint();
    // If the config has no type, conf ignores it
    std::list<synExp*> empty;
    return empty;
}

std::list<synExp*> configInfo::choicesDefineAll() {
    std::list<synExp*> result;
    // This string value cannot be EMPTY because one of the
    // members of the choice must be true or none of them
    delValue("EMPTY");
    synExp* dep = makeImplies(new synSymbol(name), getDependsVar());
    result.push_back(dep);
    for(configInfo* c : mapAlternative[name]) {
        for(auto p : c->defaultList) {
            if (p.second->isString()) {
                std::string symName = c->buildNameValue(p.second->get_string());
                synExp* implication = makeImplies(p.first, new synSymbol(symName));
                std::cerr << "9 " << implication << std::endl;
                result.push_back(implication);
            }
            else
                if (p.second->isSymbol()) {
                    synExp *defsym = makeImplies(p.first, defaultWithSymbol(p.second->getSymbol()));
                    std::cerr << "0 " << defsym << std::endl;
                    result.push_back(defsym);
                }
        }
    }
    exactlyOneStringValue(name, result);
    valuesImplyVariable(name, result);
    return result;
}
// Checks if all declarations are controlled by boolean choices
bool configInfo::processAllDefiningChoices() {
    bool result = true;
    for(configInfo* c : mapAlternative[name])
        result = result && c->stringDefinedByChoice;
    
    return result;
}
std::list<synExp*> configInfo::giveStringConstraint() {
    std::list<synExp*> result;
    chi = processStringDefaults();
       // This goes away for now
    //if (chi != NULL) std::cerr << "chi depends " << chi->getDependsVar() << std::endl;
    //std::cerr << "dependsvar " << getDependsVar() << std::endl;
    //if (chi != NULL && equal(chi->getDependsVar(), getDependsVar()))
    //    stringDefinedByChoice = true;
    
    processRange();
    if (timesProcessed[name] != symbolTable::timesDeclared(name))
        return result;
    
    std::cerr << "------------------------------------------------" << std::endl;
    std::cerr << "Name " << name << " file definition " << getFile() << std::endl;
    
    synExp *dependencies      = processDependencies();
    synExp *userInput;
    std::cerr << "comesFromEnvironment " << fromEnvironment << std::endl;
    if (fromEnvironment) {
        userInput = synTrue;
        addValue("OTHERVAL");
    }
    else
        userInput = processPrompts();
    
    // This goes away for now
//    bool choiceDefined = processAllDefiningChoices();
//
//    if (userInput == synFalse && choiceDefined)
//        return choicesDefineAll();
    
    synExp *defaultsCondition = processAllStringDefaults();
    checkInternal();
    artificial = internal;
    std::cerr << "dependencies "        << dependencies      << std::endl;
    std::cerr << "userInput       "     << userInput         << std::endl;
    std::cerr << "defaultsCondition "   << defaultsCondition << std::endl;
    std::cerr << "depStack ";
    for(synExp *s : getDepStack())
        std::cerr << s << " ";
    std::cerr << std::endl;

    synExp* innerIf  = makeIfThenElse(userInput,
                                      new synSymbol(name),
                                      defaultsCondition);
    std::cerr << "innerIf " << innerIf << std::endl;
    synExp* outerIf = makeIfThenElse(dependencies, innerIf, makeNot(new synSymbol(name)));

   
    std::cerr << "outerIf " << outerIf << std::endl;
    
    if (outerIf != synTrue)
        result.push_back(outerIf);
    
    valuesImplyVariable(name, result);
    exactlyOneStringValue(name, result);
    
    return result;
}

synExp *configInfo::allStringValuesFalse() {
    synExp* res = synTrue;
    for(std::string val : stringAlternatives[name])
        res = makeAnd(res, makeNot(new synSymbol(val)));
    return res;
}
void configInfo::valuesImplyVariable(std::string s, std::list<synExp*>& res) {
    for(std::string alt : stringAlternatives[s]) {
        synExp *ss = makeImplies(new synSymbol(alt), new synSymbol(name));
        std::cerr << "3 " << ss << std::endl;
        res.push_back(ss);
    }
}
    

void configInfo::exactlyOneStringValue(std::string s, std::list<synExp*>& res) {
    if (useXOR) {
        std::vector<synExp*> v;
        for(const std::string& st : stringAlternatives[s])
            v.push_back(new synSymbol(st));
        res.push_back(makeImplies(new synSymbol(s),makeXOR(v)));
        
    }
    else {
        bool first = true;
        synExp *theOr = synFalse;
        for(const std::string& alt : stringAlternatives[s]) {
            if (first)
                theOr = new synSymbol(alt);
            else
                theOr = makeOr(theOr, new synSymbol(alt));
            first = false;
        }
        res.push_back(makeImplies(new synSymbol(s), theOr));
            
        for(const std::string& alt1 : stringAlternatives[s]) {
            for(const std::string& alt2 : stringAlternatives[s]) {
                if (alt1 > alt2) {
                    synExp *notTwo = makeNot(makeAnd(new synSymbol(alt1),
                                                     new synSymbol(alt2)));
                    res.push_back(notTwo);
                    //               std::cerr << "11 " << notTwo << std::endl;
                }
            }
        }
    }
}
bool inExpSet(synExp* exp, std::set<synExp*> ss) {
    for(synExp* o : ss)
        if (equal(o, exp))
            return true;
    return false;
}

void flattenAnd(synExp* exp, std::set<synExp*>& ss) {
    if (exp->isAnd()) {
        flattenAnd(exp->first(), ss);
        flattenAnd(exp->second(), ss);
    }
    else
        ss.insert(exp);
}
void intersect(std::set<synExp*>& inter, std::set<synExp*> other) {
    std::set<synExp*>::iterator its = inter.begin();
    while (its != inter.end()) {
        if (!inExpSet(*its, other))
            its = inter.erase(its);
        else
            its++;
    }
}

std::set<synExp*> configInfo::intersectionSelectorSelectorDeps(std::string selector) {
    // Let's make the intersection of the selector's selectors
    std::set<synExp*> intersection;
    bool firstTime = true;
    for(std::pair<std::string, std::vector<selectHelper>> p : mapSelect[selector])
        for(selectHelper h : p.second) {
            std::set<synExp*> theOther;
            std::cerr << " selector " << selector << " selector " << p.first
            <<  " deps " << h.getDep() << std::endl;
            if (firstTime) {
                flattenAnd(h.getDep(), intersection);
                firstTime = false;
            }
            else {
                flattenAnd(h.getDep(), theOther);
                intersect(intersection, theOther);
            }
        }
    return intersection;
}


synExp*            configInfo::fixDeps(std::set<synExp*> inter, synExp* deps) {
    std::set<synExp*> setDeps;
    flattenAnd(deps, setDeps);
    std::set<synExp*>::iterator its = setDeps.begin();
    while (its != setDeps.end()) {
        if (inExpSet(*its, inter))
            its = setDeps.erase(its);
        else
            its++;
    }
    synExp *res = synTrue;
    for(synExp *bitDep : setDeps)
        if (!inExpSet(bitDep, inter))
            res = makeAnd(res, bitDep->copy_exp());
    return res;
}
synExp*            configInfo::processSelects() {
    // select inside a choice is ignored, as per original conf program
    if (isChoiceMember()) return synFalse;
    synExp*     orFactors = synFalse;
    for(std::pair<std::string, std::vector<selectHelper>> p : mapSelect[name]) {
        std::string selector = p.first;
        std::set<synExp*> intersectionSelectorsSelectorDeps = intersectionSelectorSelectorDeps(selector);
        
        std::cerr << "Intersection is ";
        bool first = true;
        for(synExp* xs : intersectionSelectorsSelectorDeps)
            if (first) {
                std::cerr << xs;
                first = false;
            }
            else
                std::cerr << " and " << xs;
        std::cerr << std::endl;
        
        synExp *    oneVarAlternatives = synFalse;
        bool        dropSelectorDeps       = false;
        bool        selectorSelectable = false;
        
        selectorSelectable = mapSelect[selector].size() > 0;
        bool allSelect = symbolTable::timesDeclared(selector) == mapSelect[name][selector].size();
        
        if (!selectorSelectable && allSelect)
            dropSelectorDeps = true;
        
        std::cerr   << "3.5 selector "          << selector
        << " declared "             << symbolTable::timesDeclared(selector) << " times "
        << " selectorSelectable "   << selectorSelectable
        << " dropSelectorDeps "            << dropSelectorDeps
        << " select size "          << mapSelect[name][selector].size() << std::endl;
        
        synExp*     selectorExp      = synTrue;
        synExp*     ifexpr        = synTrue;
        synExp*     expression    = synTrue;
        bool        dummySelector = false;
        configInfo *c = symbolTable::getSymbol(p.first);
        if (c != NULL) {
            c->checkInternal();
            c->checkSimple();
        }
        dummySelector = c->isDummy();
        
        std::cerr << " dummy "                << dummySelector << std::endl;

        // If the selector is dummy we don't use the name, only the dependencies and the guard
        // if dropSelectorDeps, we don't use the dependencies of the selector, only the name and the guard
        // Otherwise, we use the name, the dependencies and the select guard.
        if (dummySelector) {
            for(selectHelper h : p.second) {
                ifexpr            = h.getExpression();
                std::cerr << "selector dummy "
                            << " ifexpr " << ifexpr
                            << " deps " << h.getDep() << std::endl;
                orFactors = makeOr(orFactors, makeAnd(h.getDep(), ifexpr));
                std::cerr << "1 orFactors = " << orFactors << std::endl;
            }
        }
        else
        // We use the selector and the condition, but omit the dependency.
        if (dropSelectorDeps) {
            for(selectHelper h : p.second) {
                selectorExp  = new synSymbol(h.getConfig()->getName());
                ifexpr            = h.getExpression();
                std::cerr << "selectorExp " << selectorExp << " selector " << selector
                            << " ifexpr " << ifexpr << std::endl;
                orFactors = makeOr(orFactors, makeAnd(new synSymbol(selector), ifexpr));
                std::cerr << "2 orFactors = " << orFactors << std::endl;
            }
        }
        else {
            // Now we use the intersection of the selectors selector to try to trim the
            // dependencies
            
            selectorExp  = new synSymbol(selector);
            std::cerr << "selectorExp " << selectorExp << std::endl;
            for(selectHelper h : p.second) {
                synExp* dependencies = h.getDep();
                if (allSelect) 
                    dependencies = fixDeps(intersectionSelectorsSelectorDeps,
                                           h.getDep());
                std::cerr << "Dependencies before " << h.getDep() << ", after " << dependencies << std::endl;
                expression = h.getExpression();
                std::cerr << "Expression " << expression << std::endl;
                ifexpr            = makeAnd(expression, dependencies);
                std::cerr << "3.7 ifexpr " << ifexpr << std::endl;
                oneVarAlternatives = makeOr(oneVarAlternatives, ifexpr);
                std::cerr << "3.8 oneVarAlternatives " << oneVarAlternatives << std::endl;
                
            }
            
            oneVarAlternatives = makeAnd(selectorExp->copy_exp(), oneVarAlternatives);
            std::cerr << "3.9 oneVarAlternatives " << oneVarAlternatives << std::endl;
            
            orFactors                     = makeOr(orFactors, oneVarAlternatives);
            std::cerr << "3 orFactors = " << orFactors << std::endl;
        }
    }
    return orFactors;
}

synExp*            configInfo::processPrompts() {
    synExp* condition = synFalse;
    for(configInfo* c : mapAlternative[name]) {
            std::cerr   << " vis " << c->visibilityVar
                        << " prompt " << c->prompt
                        << " promptguard " << c->promptGuard 
                        << " dependencies " << c->getDependsVar() << std::endl;
            synExp* auxBit =  makeAnd(c->visibilityVar->copy_exp(),
                                               makeAnd(c->prompt->copy_exp(),
                                                       c->promptGuard->copy_exp()));
            synExp* otherBit;
                otherBit = makeAnd(c->getDependsVar()->copy_exp(), auxBit);
        
            std::cerr << "otherBit " << otherBit << std::endl;
            condition = makeOr(condition, otherBit);
            std::cerr << "condition for " << name << ": " << condition << std::endl;
        }
    return condition;
}

bool            configInfo::checkInternal() {
    internal = true;
    for(configInfo* c : mapAlternative[name])
        if (c->prompt != synFalse)
            internal = false;
    return internal;
}
synExp*            configInfo::processAllStringDefaults() {
    // We need a list with the following things:
    // Dependencies, list of pairs <guard, value>

    std::vector<synExp*>              dep;
    std::vector<std::vector<synExp*>> guard;
    std::vector<std::vector<synExp*>> value;
    synExp* lastDep    = synFalse;
    bool lastIgnored   = false;
    bool firstTime     = true;
    bool emptyDefaults = false;
    for(configInfo* c : mapAlternative[name]) {
        std::vector<synExp*> oneGuard, oneValue;
        // User interface, i.e. usable prompt
        synExp *ui = makeAnd(c->visibilityVar->copy_exp(),
                             makeAnd(c->prompt->copy_exp(),
                                     c->promptGuard->copy_exp()));
        // If the prompt exists, we add the value OTHERVAL
        if (c->prompt->copy_exp() == synTrue) addValue("OTHERVAL");
        // If this config has a viable prompt, the defaults will never be used
        // so we just pass on them
        if (ui != synTrue) {
            std::cerr << "Depends " << c->getDependsVar() << std::endl;
            // If there is a streak of the same dependencies, we concatenate
            // the default values and conditions to the same entry
            if (equal(c->getDependsVar(), lastDep) && !lastIgnored && !firstTime) {
                for(auto p : c->defaultList) {
                    guard.back().push_back(p.first);
                    value.back().push_back(p.second);
                }
                std::cerr << "Now, dep " << dep.back() << " ";
                for(int zz = 0; zz < guard.back().size(); zz++)
                    std::cerr << " guard " << guard.back()[zz]
                    << " value " << value.back()[zz] << std::endl;
            }
            else   {
                std::cerr << "defaultList size is " << c->defaultList.size() << std::endl;
                 for(auto p : c->defaultList) {
                    oneGuard.push_back(p.first);
                    oneValue.push_back(p.second);
                }
                lastDep = c->getDependsVar();
                // If a config has no defaults, we ignore it
                if (c->defaultList.size() > 0) {
                    guard.push_back(oneGuard);
                    value.push_back(oneValue);
                    dep.push_back(c->getDependsVar()->copy_exp());
                    lastIgnored = false;
                }
                else {
                    lastIgnored   = true;
                    emptyDefaults = true;
                }
        
            }
            firstTime = false;
        }
        ui->destroy();
    }
  
    // Now for a bit of improvement.
    // If the defaults are the same we also join the dependencies
    for(int x = 1; x < dep.size(); x++) {
        bool sameDef = true;
        if (guard[x].size() == guard[x-1].size()) {
            for(int z = 0; sameDef && z < guard[x].size(); z++)
                sameDef &= (equal(guard[x][z], guard[x-1][z]) &&
                            equal(value[x][z], value[x-1][z]));
            if (sameDef) {
                // Entries x and x-1 have the same defaults
                // We join them
                std::cerr << "We join two entries with the same defaults and dependencies "
                    << dep[x-1] << ", " << dep[x];
                dep[x-1] = makeOr(dep[x-1], dep[x]);
                std::cerr << " to " << dep[x-1] << std::endl;
                dep.erase(dep.begin()+x);
                guard.erase(guard.begin()+x);
                value.erase(value.begin()+x);
                x = x-1;
            }
        }
    }

    // Now, a propositional formula is created from the info gathered above
    // in dep, guard and value
    synExp* condition = makeNot(new synSymbol(name));
    bool lastConfig = true;
   
    for(int q = 0; q < dep.size(); q++) {
        synExp* orGuards = synFalse;
        synExp* def = synTrue;
        if (lastConfig) {
            lastConfig = false;
            def = makeNot(new synSymbol(name));
        }
        for(int w = 0; w < guard[q].size(); w++) {
            orGuards = makeOr(orGuards, guard[q][w]->copy_exp());
            synExp *defvalue;
            std::string sval = value[q][w]->get_string();
            // If the defining value is a string literal...
            if (value[q][w]->get_type() == synExp_String)
                defvalue = defaultWithString(sval);
            else
                // We have a symbol as defining value
                if (value[q][w]->get_type() == synExp_Symbol)
                    defvalue = defaultWithSymbol(value[q][w]->getSymbol());
                else
                    throw std::logic_error("Don't know what to do with default "+sval+"\n");
            def = makeIfThenElse(guard[q][w],
                                 defvalue->copy_exp(),
                                 def->copy_exp());
        }
        // If there is only one entry there's no need to repeat the dependencies
        // unless of course there was at least one declaration with no defaults.
        // In that case, we need to be able to distinguish between the two cases

        if (dep.size() == 1 && !emptyDefaults)
            condition = def->copy_exp();
        else
            condition = makeIfThenElse(makeAnd(dep[q]->copy_exp(), orGuards),
                                       def,
                                       condition);
    }
    return condition;
}

bool          configInfo::checkSimple() {
    simple = true;
    for(configInfo* c : mapAlternative[name]) {
        if (c->defaultList.size() == 0)
            simple = false;
        for(std::list<std::pair<synExp*, synExp*> >::iterator itl = c->defaultList.begin();
            itl != c->defaultList.end(); itl++) {
            if (itl->second != synTrue || itl->first != synTrue)
                simple = false;
        }
    }
    return simple;
}


synExp*            configInfo::processAllLogicalDefaults() {
    // We need a list with the following things:
    // Dependencies, list of pairs <guard, value>

    std::vector<synExp*>              dep;
    std::vector<std::vector<synExp*>> guard;
    std::vector<std::vector<synExp*>> value;
    
    synExp* lastDep = synFalse;
    bool lastIgnored = false;
    for(configInfo* c : mapAlternative[name]) {
        std::vector<synExp*> oneGuard, oneValue;
        // If there is a streak of the same dependencies, we join them
        if (equal(c->getDependsVar(), lastDep) && !lastIgnored && !guard.empty()) {
            //std::cerr << "2 Joining two dependencies 1 (" << c->getDependsVar() << ") and 2 (" << lastDep << ")" << std::endl;
            for(std::list<std::pair<synExp*, synExp*> >::iterator itl = c->defaultList.begin();
                itl != c->defaultList.end(); itl++) {
                guard.back().push_back(itl->first);
                value.back().push_back(itl->second);
            }
            
            //if (dep.size() > 0 ) {
            //    std::cerr << "Now, dep " << dep.back() << " ";
            //for(int zz = 0; zz < guard.back().size(); zz++)
            //    std::cerr << " guard " << guard.back()[zz]
            //              << " value " << value.back()[zz] << std::endl;
            //}
        }
        else {
        for(std::list<std::pair<synExp*, synExp*> >::iterator itl = c->defaultList.begin();
            itl != c->defaultList.end(); itl++) {
                oneGuard.push_back(itl->first);
                oneValue.push_back(itl->second);
            }
            lastDep = c->getDependsVar();
            // If a config has no defaults, we ignore it
            if (c->defaultList.size() > 0) {
                guard.push_back(oneGuard);
                value.push_back(oneValue);
                dep.push_back(c->getDependsVar()->copy_exp());
                lastIgnored = false;
            }
            else lastIgnored = true;

        }
    }
    // Now we need to see if a dependency is repeated
    //for(int x = 0; x < dep.size(); x++) {
    //    bool repeated = false;
    //    int y;
    //    for(y = x+1; !repeated && y < dep.size(); y++)
    //        repeated = equal(dep[x], dep[y]);
        // If it is, we add the guards to the dependency
        // so the later occurrence has a chance.
    //    if (repeated) {
    //        y--;
    //        std::cerr << "A dependency is repeated " << dep[y] << std::endl;
    //        synExp *orGuards = synFalse;
    //        for(synExp* g : guard[y])
    //            orGuards = makeOr(orGuards, g->copy_exp());
            // If we cover all the cases the repeated dependency is useless
            // and it should be delete
    //        if (orGuards == synTrue) {
    //            std::cerr << "But the former occurrence covers all cases" << std::endl;
    //            dep.erase(dep.begin()+x);
    //            guard.erase(guard.begin()+x);
    //            value.erase(value.begin()+x);
    //        }
    //        else
    //            dep[y] = makeAnd(dep[y], orGuards);
    //    }
    //}
    
    // Now for a bit of improvement.
    // If the defaults are the same we also join the dependencies
    for(int x = 1; x < dep.size(); x++) {
        bool sameDef = true;
        if (guard[x].size() == guard[x-1].size()) {
            for(int z = 0; sameDef && z < guard[x].size(); z++)
                sameDef &= (equal(guard[x][z], guard[x-1][z]) &&
                            equal(value[x][z], value[x-1][z]));
            if (sameDef) {
            // Entries x and x-1 have the same defaults
            // We join them
            //std::cerr << "We join two entries with the same defaults and dependencies "
            //    << dep[x-1] << ", " << dep[x];
            dep[x-1] = makeOr(dep[x-1], dep[x]);
            //std::cerr << " to " << dep[x-1] << std::endl;

            dep.erase(dep.begin()+x);
            guard.erase(guard.begin()+x);
            value.erase(value.begin()+x);
            x = x-1;
            }
        }
    }
    if (dep.size() == 1 && guard[0].size() == 1 && guard[0][0] == synTrue && value[0][0] == synTrue)
        simple = true;
    else
        simple = false;
  
    synExp* condition = makeNot(new synSymbol(name));
    std::cerr << "*******" << std::endl;
    for(int q = 0; q < dep.size(); q++) {
        synExp* orGuards = synFalse;
        synExp* def = makeNot(new synSymbol(name));
        for(int w = 0; w < guard[q].size(); w++) {
            orGuards = makeOr(orGuards, guard[q][w]->copy_exp());
            def = makeIfThenElse(guard[q][w], // last time we use it
                                 makeEqual(new synSymbol(name), value[q][w]),
                                 def);
        }
        condition = makeIfThenElse(makeAnd(dep[q]->copy_exp(), orGuards),
                                   def->copy_exp(),
                                   condition);
    }
    return condition;
}

void configInfo::processRangeMember(synExp* exp) {
    std::cerr << "Processing range member " << exp << std::endl;
    if (exp->isString()) {
        std::cerr << "It is a string" << std::endl;
        addValue(exp->get_string());
        return;
    }
    // This may not work if the symbol is not declared yet
    if (exp->isSymbol()) {
        std::cerr << "It is a symbol" << std::endl;
        for(std::string val : stringValues[exp->getSymbol()]) {
            std::cerr << "Adding symbol alternative " << val << std::endl;
            addValue(val);
        }
        return;
    }
    std::cerr << "It is nothing!! " << exp->get_type() <<  std::endl;
}

void configInfo::processRange() {
    for(auto exp : range)
        processRangeMember(exp);
}

synExp*            configInfo::processDependencies() {
    synExp* condition = synFalse;
    for(configInfo* c : mapAlternative[name])
        condition = makeOr(condition, c->getDependsVar()->copy_exp());
    
    return condition;
}

bool configInfo::checkRecursiveSelects() {
    std::vector<std::string> traversal;
    traversal.push_back(name);
    return checkRecursiveSelectsRecur(traversal);
}
bool configInfo::checkRecursiveSelectsRecur(std::vector<std::string> vec) {
    // We take the last element in the backtracking
    std::string sel = vec.back();
    // Recover the selects
    if (configsISelect.empty())
        return false;
    configInfo *c      = symbolTable::getSymbol(sel);
    if (c == NULL)
        throw std::logic_error("Error looking for symbol "+sel);
    
    std::set<std::string> varsels;
    for(std::string varsel : varsels) {
        if (std::find(vec.begin(), vec.end(), varsel) != vec.end()) {
            std::cerr << "Recursive select detected for config " << name << ":" << std::endl;
            bool firstTime = true;
            for(std::string s : vec)
                if (firstTime) {
                    firstTime = false;
                    std::cerr << "config " << s << " selects ";
                }
                else
                    std::cerr << s << " which in turn selects on ";
            std::cerr << varsel << std::endl;
            return true;
        }
        std::vector<std::string> localCopy = vec;
        localCopy.push_back(varsel);
        if (checkRecursiveSelectsRecur(localCopy))
            return true;
    }
    return false;
}

   
bool configInfo::checkRecursiveDeps() {
    std::vector<std::string> traversal;
    traversal.push_back(name);
    return checkRecursiveDepsRecur(traversal);
}

bool configInfo::checkRecursiveDepsRecur(std::vector<std::string> vec) {
    // We take the last element in the backtracking
    std::string dep = vec.back();
    // Recover the dependencies
    std::set<std::string> vardeps = symbolTable::getSymbol(dep)->getDependsVar()->giveSymbols();
    if (vardeps.empty())
        return false;
    
    for(std::string vardep : vardeps) {
        if (std::find(vec.begin(), vec.end(), vardep) != vec.end()) {
            std::cerr << "Recursive dependency detected for config " << name << ":" << std::endl;
            bool firstTime = true;
            for(std::string s : vec)
                if (firstTime) {
                    firstTime = false;
                    std::cerr << "config " << s << " depends on ";
                }
                else
                    std::cerr << s << " which in turn depends on ";
            std::cerr << vardep << std::endl;
            return true;
        }
        std::vector<std::string> localCopy = vec;
        localCopy.push_back(vardep);
        if (checkRecursiveDepsRecur(localCopy))
            return true;
    }
    return false;
}

std::list<synExp*> configInfo::giveLogicalConstraint() {
    std::list<synExp*> result;
    //mapAlternative[name].push_front(this);
    //processLogicalDefaults();

    if (timesProcessed[name] != symbolTable::timesDeclared(name))
        return result;
    

    std::cerr << "------------------------------------------------"  << std::endl;
    std::cerr << "Name " << name << " file definition " << getFile() << std::endl;
    std::cerr << "mapAlternative size " << mapAlternative[name].size() << std::endl;

    checkRecursiveSelects();
    
    synExp* innerIf = synFalse;
   
        checkInternal();
        checkSimple();
        //synExp *dependencies      = processDependencies();
        synExp *selectCondition   = processSelects();
        synExp *userInput;
        std::cerr << "comesFromEnvironment " << fromEnvironment << std::endl;
        if (fromEnvironment)
            userInput = synTrue;
        else
            userInput = processPrompts();
    
        synExp *defaultsCondition = processAllLogicalDefaults();
        artificial = internal;
        std::cerr << "Internal? " << internal << " simple? " << simple
        << " dummy? " << isDummy()  << " artificial? " << artificial << std::endl;
        
        //std::cerr << "dependencies    "   << dependencies      << std::endl;
        std::cerr << "selectCondition "   << selectCondition   << std::endl;
        std::cerr << "userInput       "   << userInput         << std::endl;
        std::cerr << "defaultsCondition " << defaultsCondition << std::endl;
        std::cerr << "depStack ";
        
        for(synExp *s : getDepStack())
            std::cerr << s << " ";
        std::cerr << std::endl;
    
        if (checkRecursiveDeps())
            innerIf = makeNot(new synSymbol(name));
        else
            innerIf  = makeIfThenElse(userInput, synTrue, defaultsCondition);
    
    std::cerr << "innerIf  " << innerIf  << std::endl;
    //synExp* middleIf = makeIfThenElse(dependencies, innerIf, makeNot(new synSymbol(name)));
    //std::cerr << "middleIf " << middleIf << std::endl;
    synExp* outerIf  = makeIfThenElse(selectCondition,
                                      new synSymbol(name),
     //                                 makeEqual(new synSymbol(name), synTrue),
                                      innerIf);
    std::cerr << "outerIf  " << outerIf  << std::endl;
    
    if (outerIf != synTrue)
        result.push_back(outerIf);
    return result;
}


void configInfo::processLogicalDefaults() {
    // In principle there is not a default value...
    std::cerr << "processDefaults " << name << " defaultList size " << defaultList.size() << std::endl;
        synExp *temp = synFalse;
        for(std::list<std::pair<synExp*, synExp*> >::iterator itl = defaultList.begin();
            itl != defaultList.end(); itl++) {
            synExp *guard = itl->first;
            synExp *value = itl->second;
            std::cerr << "guard " << guard << " value " << value << std::endl;
            temp = makeIfThenElse(guard->copy_exp(),
                                  value->copy_exp(),
                                  temp->copy_exp());
            std::cerr << "temp is " << temp << std::endl;
        }
        std::cerr << " 1 logicalDefault = " << temp << std::endl;
        logicalDefault = temp;
    
}

synExp* configInfo::defaultWithString(std::string sval) {
    
    synExp *defvalue = synTrue;
    
    // Sometimes default values include the value
    // of another string config in the form $(NAME) or $NAME or NAME
    
    // We visit the alternatives for $NAME so we can build
    // the alternatives for this config
    
    // We will save a pair of the implier and the value
    // As in XX_EQ_A -> C_EQ_BEFORE_A_AFTER
    
    std::vector<std::pair<synExp*,std::string>> stack;
    if (sval == "") {
        sval = "EMPTY";
        addValue(sval);
        return new synSymbol(buildNameValue(sval));
    }
    
    stack.push_back(std::pair<synExp*, std::string>(synTrue,sval));
    synExp *exp = synTrue;
    while (!stack.empty()) {
        exp  = stack.back().first;
        sval = stack.back().second;
        
        stack.pop_back();
        
        std::size_t dollar, leftParen, rightParen;
        // If the value IS a reference to another symbol's value
        // we investigate the default values to expand the possibilities
        
        // However, the value cannot CONTAIN a reference to another value
        // with more things.
        
        // default A # where A is a string config OK
        // default xx$(A)xx # not OK
        
        dollar     = sval.find('$');
        rightParen = sval.find(')');
        leftParen  = sval.find('(');
        
        
        if (dollar == 1) {
            std::string otherVar;
            if (leftParen == 1 && rightParen == sval.length()-1)
                otherVar = sval.substr(dollar+2, rightParen - dollar -2);
            else
                if (leftParen == std::string::npos && rightParen == std::string::npos)
                    otherVar = sval.substr(dollar+1, sval.length() - dollar -1);
            
            configInfo* other    = symbolTable::getSymbol(otherVar);
            
            // This is duplicate code with the next function defaultWithSymbol
            
            // If the other symbol's info has not been
            // filled in yet...
            if (other != NULL && configInfo::stringValues[otherVar].empty())
                other->processAllStringDefaults();
            
            for(const std::string& osa : configInfo::stringValues[otherVar]) {
                addValue(osa);
                synExp *implier = new synSymbol(other->buildNameValue(osa));
                synExp *implied = new synSymbol(buildNameValue(osa));
                
                defvalue = makeAnd(defvalue, makeImplies(implier, implied));
            }
            defvalue = makeAnd(defvalue, makeImplies(makeNot(new synSymbol(other->name)),
                                                     addValue("EMPTY")->value));
        }
        // If not, any possible reference to another string config
        // has been processed
        else {
            defvalue = makeAnd(defvalue, new synSymbol(buildNameValue(sval)));
            addValue(sval);
        }
    }
    return defvalue;
}

synExp* configInfo::defaultWithSymbol(std::string sval) {
    synExp *defvalue = synTrue;
    configInfo* confsym = symbolTable::getSymbol(sval);
    // If the other symbol's info has not been
    // filled in yet...
    if (confsym != NULL && configInfo::stringValues[sval].empty())
        confsym->processAllStringDefaults();
    
    if (confsym == NULL)
        throw std::logic_error("Symbol "+sval+" not found in symbol table");
    
    // We access all the different values for the symbol
    synExp *translation = synTrue;
    for(std::string theVal : stringValues[sval]) {
        synExp *implier = new synSymbol(confsym->buildNameValue(theVal));
        synExp *implied = new synSymbol(buildNameValue(theVal));
        translation = makeAnd(translation,
                              makeImplies(implier, implied));
        addValue(theVal);
    }
    // And a last bit. If the symbol is negated, we add the value EMPTY and imply it
    synExp *emptybit = makeImplies(makeNot(new synSymbol(confsym->name)),
                                   addValue("EMPTY")->value);
    // We override the value
    defvalue = makeAnd(translation, emptybit);
    return defvalue;
}

// We process here the string defaults working from the defaultList
// which is a list if pairs <value, guard>

// The defining values may be string literals or symbols

// Also, we check if the guards are ALL the members of the same choice
//so we can simplify the translation

choiceInfo* configInfo::processStringDefaults() {
    // In principle there is not a default value...
    synExp* temp = makeNot(value->copy_exp());
    std::set<choiceInfo*> chiSet;
    std::set<std::string> membersSet;
    bool allChoiceMembers = true;
    for(std::pair<synExp*, synExp*> pair : defaultList) {
            synExp *guard = pair.first;
            synExp *defvalue = synTrue;
        if (guard->isSymbol()) {
            configInfo *ci = symbolTable::getSymbol(guard->getSymbol());
            if (ci->isChoiceMember()) {
                chiSet.insert(choiceInfo::getChoiceFromMember(ci));
                membersSet.insert(ci->getName());
            }
            else allChoiceMembers = false;
        }
        else allChoiceMembers = false;
            
            std::string sval = pair.second->get_string();
            // If the defining value is a string literal...
            if (pair.second->get_type() == synExp_String)
                    defvalue = defaultWithString(sval);
            else
                // We have a symbol as defining value
                // we need to take the values for the symbol
                // and add them as possibilities for this symbol
                if (pair.second->get_type() == synExp_Symbol)
                    defvalue = defaultWithSymbol(pair.second->getSymbol());
                else
                    throw std::logic_error("Don't know what to do with default "+sval+"\n");
            temp = makeIfThenElse(guard->copy_exp(),
                                  defvalue->copy_exp(),
                                  temp->copy_exp());
        }
        stringDefault = temp;
    choiceInfo* chi = NULL;
    if (chiSet.size() == 1)
        chi = *chiSet.begin();
    
    if (allChoiceMembers && chi != NULL) {
        std::set<std::string> choiceMembers = chi->getMemberNames();
        if (membersSet == choiceMembers)
            return chi;
    }
    return NULL;
}

std::string configInfo::buildNameValue(std::string s) {
    std::string newName;
    std::string two;
    if (s.length() > 2)
        two = std::string(s, 0, 2);
    if (s == "") s = "EMPTY";
    if (vartype == "hex" && s != "EMPTY" && s != "OTHERVALUE") {
        if (two != "0x" && two != "0X") {
            s = "0x"+s;
        }
    }
    if (s != "")
        newName = name + "_EQ_" + s;
    else
        newName = name + "_EQ_EMPTY";
    
    std::replace(newName.begin(), newName.end(), ' ', '_');
    std::replace(newName.begin(), newName.end(), '(', '_');
    std::replace(newName.begin(), newName.end(), ')', '_');
    //std::replace(newName.begin(), newName.end(), '{', '_');
    //std::replace(newName.begin(), newName.end(), '}', '_');
    return newName;
}


void configInfo::delValue(const std::string& s) {
    stringValues[name].erase(s);
    stringAlternatives[name].erase(buildNameValue(s));
    //symbolTable::deleteSymbol(buildNameValue(s));
}
configInfo* configInfo::addValue(std::string s) {
    std::string two;
    if (s.length() > 2)
        two = std::string(s, 0, 2);
    if (s == "") s = "EMPTY";
    if (vartype == "hex" && s != "EMPTY" && s != "OTHERVALUE") {
        if (two != "0x" && two != "0X") {
            s = "0x"+s;
            std::cerr << "XXX new s " << s << std::endl;
        }
    }
    std::string newName;
    stringValues[name].insert(s);
    newName = buildNameValue(s);
    stringAlternatives[name].insert(newName);
    return  defineName(newName);
}

configInfo* configInfo::defineName(std::string newName) {
    if (symbolTable::getSymbol(newName) == NULL) {
        synSymbol* variant  = new synSymbol(newName);
        configInfo *c = new configInfo();
        c->setName(newName);
        c->setVartype("boolean");
        c->setValue(variant);
        c->setIsStringValue();
        c->artificial = true;
        symbolTable::declareSymbol(newName, c);
    }
    return symbolTable::getSymbol(newName);
}
void configInfo::print(std::ostream& os) {
    os << "-----------------------------------------------------" << std::endl;
    os << "Config" << std::endl;
    os << "Name:         "     << name             << std::endl;
    os << "Prompt text:  "     << promptext        << std::endl;
    os << "Prompt Guard: "     << promptGuard      << std::endl;
    os << "Type:         "     << vartype          << std::endl;
 //   os << static_cast<varInfo*>(this);
    os << "Prompt:       "     <<  prompt           << std::endl;
    os << "Default:      "     << logicalDefault         << std::endl;
    os << "Select:       "     << std::endl;
    
    os << "Value:        "      << value                 << std::endl;
    os << "Visibility:   "      << visibilityVar         << std::endl;
    std::list<synExp*> theList = giveConstraint();
    os << "Constraint:   " << std::endl;
    for(std::list<synExp*>::iterator itl = theList.begin(); itl != theList.end(); itl++)
        os << *itl << std::endl;
}

