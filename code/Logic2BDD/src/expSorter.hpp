//
//  expSorter.h
//  myKconf
//
//  Created by David on 13/08/15.
//  Copyright (c) 2015 david. All rights reserved.
//

#ifndef __myKconf__expSorter__
#define __myKconf__expSorter__

#include <stdio.h>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>

#include "humanNums.hpp"
#include "constraintSorter.hpp"
#include "synExp.hpp"
#include "cuddAdapter.hpp"

class expSorter : public constraintSorter {
    
public:
    
    cuddAdapter* factory;
    
    expSorter(cuddAdapter* a,
              std::vector<synExp*> c) : constraintSorter(a,c) {};
    
    bool operator()(const synExp *a, const synExp *b) {
        int x = a->getScore();
        int y = b->getScore();
        if (x != y)
            return x < y;
        
        int c = a->getMax() - a->getMin();
        int d = b->getMax() - b->getMin();
        if (c != d)
            return c < d;
        
        return a->getMax() < b->getMax();
    }
    
    void reorderConstraints(std::vector<synExp*>::iterator beg, std::vector<synExp*>::iterator end, bool lastReorder) {
        //factory->computeLevelNodes();
        for(std::vector<synExp*>::iterator t = beg; t != end; t++) {
            synExp *s = *t;
            s->computeMaxMin(adapter->var2pos());
            double score = 0;
//            for(int l = s->getMin(); l != s->getMax()+1; l++)
//                score += factory->getLevelNodes(l);
            
            s->setScore(score);
        }
        std::vector<synExp*>::iterator theMin = std::min_element(beg, end, *this);
        if (theMin != beg) {
            synExp *temp    = *theMin;
            *theMin         = *beg;
            *beg            = temp;
        }
    };
    
    std::string giveInfo(synExp* s) {
        std::ostringstream ost;
        ost << "[" << std::setw(8) <<  showHuman((int)(s->getScore()));
        ost << " " << std::setw(5) << s->getMax() - s->getMin();
        ost << "]";
        return ost.str();
    }
    

};

#endif /* defined(__myKconf__expSorter__) */
