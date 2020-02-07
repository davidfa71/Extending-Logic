//
//  RememberSorter.h
//  myKconf
//
//  Created by David on 05/06/15.
//  Copyright (c) 2015 david. All rights reserved.
//

#ifndef __myKconf__RememberSorter__
#define __myKconf__RememberSorter__

#include <stdio.h>
#include <vector>
#include <map>
#include <string>
#include <sstream>

#include "constraintSorter.hpp"
#include "synExp.hpp"

class RememberSorter : public constraintSorter {
    
private:
    std::vector<int> low, high;
    std::list<synExp*> plist;
    void processExp(synExp* s) {
        int     smax    = -1;
        int     smin    = std::numeric_limits<int>::max();
        
          if (s->giveSymbolIndices().size() == 2) {
            for(auto v : s->giveSymbolIndices()) {
                if (low[v]  > smax)  smax = low[v];
                if (high[v] < smin)  smin = high[v];
            }
            for(auto v : s->giveSymbolIndices()) {
                //if (smin < low[v])  low[v]  = smin;
                if (smax > high[v]) high[v] = smax;
            }
        }
    };
    void computeRememberSpan(synExp* s, double& rem, int &span) {
        int     smax    = -1;
        int     smin    = std::numeric_limits<int>::max();
        int     ssmax   = -1;
        int     ssmin   = std::numeric_limits<int>::max();
        std::vector<int> var2pos = adapter->var2pos();
        for(auto v : s->giveSymbolIndices()) {
            if (var2pos.at(v) > ssmax) ssmax = var2pos.at(v);
            if (var2pos.at(v) < ssmin) ssmin = var2pos.at(v);
        }
        if (s->giveSymbolIndices().size() == 2) {
            for(auto v : s->giveSymbolIndices()) {
                if (low[v]  > smax)  smax = low[v];
                if (high[v] < smin)  smin = high[v];
            }
            //for(auto v : s.getIndices()) {
            //    if (smin < low[v])  low[v]  = smin;
            //    if (smax > high[v]) high[v] = smax;
            //}
            if (smin > smax) smin = smax;
        }
        else {
            smin = ssmin;
            smax = ssmax;
        }
        rem  =  giveScore(smax, smin);
        span =  ssmax - ssmin;
        //std::cerr << "s = " << s << " giveScore(" << std::setw(5) << smax << ", " << std::setw(5) << smin << ") = " << rem << " span " << ssmax - ssmin << std::endl;
        //<< xx << std::endl;
    }
    
    double giveScore(int a, int b) {
        if (a == b) return 0;
        return (a-b)*log(a-b);
        
        if (a > b)
            return (a-b)*log(a-b);
        else
            return (b-a)*log(b-a);
    }

public:
    
    RememberSorter(cuddAdapter*  adapter,
                  const         std::vector<synExp*>&       c) : constraintSorter(adapter,c) {
        low  = adapter->var2pos();
        high = adapter->var2pos();
    };
    
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
        double minRemember = std::numeric_limits<int>::max();
        double rem;
        int    minSpan     = std::numeric_limits<int>::max();
        int    span;
        std::vector<synExp*>::iterator minIt;
        if (lastOrder) {
            low  = adapter->var2pos();
            high = adapter->var2pos();
            for(auto s : plist)
            processExp(s);
        }
        for(std::vector<synExp*>::iterator its = beg; its != end; its++) {
            computeRememberSpan(*its, rem, span);
            //            if ((*its)->giveSymbolIndices().size() < minVar) {
            //                minVar      = (*its)->giveSymbolIndices().size();
            //                minRemember = rem;
            //                minIt       = its;
            //            }
            //            else
            if (rem < minRemember) {
                minRemember = rem;
                minSpan     = span;
                minIt       = its;
            }
            else
            if (rem == minRemember && span < minSpan) {
                minSpan = span;
                minIt   = its;
            }
        }
        //std::cerr << "min rem is " << minRemember << " and minspan " << minSpan
        //          << "in expression " << *minIt << std::endl;
        synExp* temp = *beg;
        *beg = *minIt;
        *minIt = temp;
        //std::swap(beg, minIt);
    }

    
    std::string giveInfo(synExp* s) {
        double r;
        int    sp;
        computeRememberSpan(s, r, sp);
        std::ostringstream ost;
        ost << "[" << std::setw(5) <<  (int)r;
        ost << " " << std::setw(5) <<  sp;
        ost << " " << std::setw(4) << adapter->getNumComponents();
        ost << " " << std::setw(4) << adapter->getMaxComponent();
        ost << "]";
        return ost.str();
    }
    
    void thisOneIsProcessed(synExp *s) {
        plist.push_back(s);
        processExp(s);
    }
};
#endif /* defined(__myKconf__RememberSorter__) */
