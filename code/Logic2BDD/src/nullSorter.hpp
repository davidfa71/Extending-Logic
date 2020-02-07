//
//  nullSorter.hpp
//  myKconf
//
//  Created by David Fernandez Amoros on 11/06/2018.
//  Copyright © 2018 david. All rights reserved.
//

#ifndef nullSorter_hpp
#define nullSorter_hpp

#include <stdio.h>
#include <vector>
#include <map>
#include <string>
#include <sstream>

#include "constraintSorter.hpp"
#include "synExp.hpp"

class nullSorter : public constraintSorter {
    
public:
    
    nullSorter(cuddAdapter *a,
               std::vector<synExp*> c) : constraintSorter(a,c) {};
    
    bool    operator()(const synExp *a, const synExp *b) { return false; };

    void reorderConstraints(std::vector<synExp*>::iterator beg, std::vector<synExp*>::iterator end, bool lastOrder) {
        return;
    }
    
    
    std::string giveInfo(synExp* s) {
        //std::ostringstream ost;
        //ost.precision(2);
        //ost << "[ " << std::setw(4) << adapter->getNumComponents();
        //ost << " " << std::setw(4)  << adapter->getMaxComponent();
        //ost << " " << std::setw(4)  << adapter->getComponentEntropy();
        //ost << "]";
        //return ost.str();
        return "";
    }
};
#endif /* nullSorter_hpp */
