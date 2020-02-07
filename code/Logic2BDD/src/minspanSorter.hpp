//
//  minspanSorter.h
//  myKconf
//
//  Created by David on 05/06/15.
//  Copyright (c) 2015 david. All rights reserved.
//

#ifndef __myKconf__minspanSorter__
#define __myKconf__minspanSorter__

#include <stdio.h>
#include <vector>
#include <map>
#include <string>
#include <sstream>

#include "constraintSorter.hpp"
#include "synExp.hpp"

class minspanSorter : public constraintSorter {
    
public:
    
    minspanSorter(cuddAdapter* a,
                  std::vector<synExp*> c) : constraintSorter(a, c) {};
    
    bool operator()(const synExp *a, const synExp *b) {
//        int ssa = a->giveSymbols().size();
//        int ssb = b->giveSymbols().size();
//        if (ssa != ssb)
//            return ssa < ssb;

        int c = a->getMax();
        int d = b->getMax();
        int x = c - a->getMin();
        int y = d - b->getMin();
        
        if ( x != y )
            return x < y;
        else
            return c < d;
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
        ost << "[" << std::setw(5) <<  s->getMax() - s->getMin();
        //ost << " " << std::setw(5) <<  s->getMax();
        ost << " " << std::setw(4) << adapter->getNumComponents();
        ost << " " << std::setw(4) << adapter->getMaxComponent();
        ost << "]";
        return ost.str();
    }
};
#endif /* defined(__myKconf__minspanSorter__) */
