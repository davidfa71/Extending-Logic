//
//  minmaxSorter.h
//  myKconf
//
//  Created by David on 05/06/15.
//  Copyright (c) 2015 david. All rights reserved.
//

#ifndef __myKconf__minmaxSorter__
#define __myKconf__minmaxSorter__

#include <stdio.h>
#include <vector>
#include <map>
#include <string>
#include <sstream>

#include "constraintSorter.hpp"
#include "synExp.hpp"

class minmaxSorter : public constraintSorter {
    
public:
    
    minmaxSorter(cuddAdapter* a,
                std::vector<synExp*> c) : constraintSorter(a, c) {};
    
    bool operator()(const synExp *a, const synExp *b) {
        int c = a->getMax();
        int d = b->getMax();
        int x = c - a->getMin();
        int y = d - b->getMin();
        
        if ( c != d )
            return c < d;
        else
            return x < y;
    }
    void reorderConstraints(std::vector<synExp*>::iterator beg, std::vector<synExp*>::iterator end, bool lastOrder) {
        if (lastOrder) {
            // Now that we have the order, we perform some operations on the expressions
            std::vector<int> myVar2pos = adapter->var2pos();
            for(std::vector<synExp*>::iterator t = beg; t != end; t++) {
                (*t)->computeMaxMin(myVar2pos);
                
            }
            std::sort(beg, end, *this);
            //            if (theMin != beg) {
            //                synExp *temp    = *theMin;
            //                *theMin         = *beg;
            //                *beg            = temp;
            //            }
        }
    }
    std::string giveInfo(synExp* s) {
        std::ostringstream ost;
        ost << "["          << std::setw(5)     <<  s->getMax() << "]";
        return ost.str();
    }
};
#endif /* defined(__myKconf__minmaxSorter__) */
