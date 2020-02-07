//
//  varInfo.cpp
//  myKconf
//
//  Created by david on 12/11/13.
//  Copyright (c) 2013 david. All rights reserved.
//

#include "varInfo.hpp"
#include "configInfo.hpp"

void varInfo::simplifyDependency() {
    int counter = 0;
    synExp* depBit = synTrue;
    for(synExp* s : dependStack) {
        std::cerr << "simplifyDependency " <<  getName() << " file definition " << getFile() << " is Symbol " << s->isSymbol() << " type is " << s->get_type() <<   " depstack size " << dependStack.size() << std::endl;
        if (s->isSymbol() && dependStack.size() > 1) {
            std::string sym = s->getSymbol();
            //std::cerr << "sym " << sym << " depended on " << configInfo::timesDependsOn[std::pair<std::string, std::string>(getName(), sym)] << " times, counter " << counter << " selectable "
            //<< configInfo::isSelectable(sym) << std::endl;
            if (configInfo::timesDependsOn[std::pair<std::string, std::string>(getName(), sym)] == 1 && !configInfo::isSelectable(sym) &&
                counter+1 != dependStack.size()) {
                synExp *newDep = synTrue;
                std::list<synExp*>::iterator itl = dependStack.begin();
                for(int x = 0; x <= counter; x++) {
                    depBit = *itl++;
                    newDep = makeAnd(depBit->copy_exp(), newDep);
                }
                newDep = makeAnd(newDep, innerDep);
                std::cerr   << "In " << std::setw(40)       << getName()
                << " with depStack size "       << dependStack.size()
                << " replacing dependency "     << std::setw(20) << getDependsVar()
                << " with " << std::setw(20)    << newDep << std::endl;

                dependsVar->destroy();
                dependsVar = newDep;
                return;
            }
        }
        counter++;
    }
    //std::cerr << "finished simplifyDependency " << name << std::endl;
    
}

std::ostream& operator<<(std::ostream& os, varInfo* ps) {
    if (ps == NULL) {
        os << "NULL";
        return os;
    }
    ps->print(os);
    os << "Menu:         "     << ps->menuName         << std::endl;
    os << "File:         "     << ps->file             << std::endl;
    os << "DependsVar:   "     << ps->dependsVar       << std::endl;
    os << "-----------------------------------------------------" << std::endl;
    return os;
}

void varInfo::print(std::ostream& os) {
    os << "Menu:         "     << menuName         << std::endl;
    os << "File:         "     << file             << std::endl;
    os << "DependsVar:   "     << dependsVar       << std::endl;
}

