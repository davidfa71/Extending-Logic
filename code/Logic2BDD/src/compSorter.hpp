//
//  compSorter.hpp
//  myKconf
//
//  Created by david on 08/01/16.
//  Copyright © 2016 david. All rights reserved.
//

#ifndef compSorter_hpp
#define compSorter_hpp

#include <stdio.h>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <algorithm>

#include "humanNums.hpp"
#include "constraintSorter.hpp"
#include "synExp.hpp"

class compSorter : public constraintSorter {
    
private:
    std::map<const synExp*, int>    scoreComps, difComps;
    std::vector<int>                myVar2pos;
    int                             lastScore;
    std::vector<std::set<synExp*> > expressionVar;
    synExp*                         lastExpression;
    
public:
    
    compSorter(cuddAdapter* a,
               std::vector<synExp*> c) : constraintSorter(a,c) {
        lastScore = 1;
        lastExpression = synTrue;
        expressionVar.resize(adapter->getNumVars());
        myVar2pos = adapter->var2pos();
        for(synExp *s : c) {
            for(int i : s->giveSymbolIndices())
                expressionVar[i].insert(s);

            computeScore(s);
        }
    };
    
    bool operator()(const synExp *a, const synExp *b) {
        int na, nb, sa, sb;
        
        
        sa = scoreComps[a];
        sb = scoreComps[b];
        
        if (sa != sb)
            return sa < sb;
        
        na = difComps[a];
        nb = difComps[b];
        
        if (na != nb)
            return na < nb;
        
        int x = a->getMax() - a->getMin();
        int y = b->getMax() - b->getMin();
        
        //if ( x != y )
        return x < y;
        
        return a->getMax() - b->getMax();
        
    }
    
    virtual void reorderConstraints(std::vector<synExp*>::iterator beg, std::vector<synExp*>::iterator end,
                                    bool lastReorder) {
        if (lastScore != 0 || lastReorder) {
            // Now we fix the score from the last time
            myVar2pos = adapter->var2pos();
            for(std::vector<synExp*>::iterator it = beg; it != end; it++)
                (*it)->computeMaxMin(myVar2pos);
            std::set<synExp*> theSet;
            for(int i : lastExpression->giveSymbolIndices())
                for(synExp* t : expressionVar[i])
                    theSet.insert(t);
            for(synExp* u : theSet)
                computeScore(u);
        }

        std::vector<synExp*>::iterator theMin = std::min_element(beg, end, *this);
        lastScore      = scoreComps[*theMin];
        lastExpression = *theMin;
        if (theMin != beg) {
            synExp *temp    = *theMin;
            *theMin         = *beg;
            *beg            = temp;
        }
        
    }
    
    void thisOneIsProcessed(synExp* s) {
        for(int x : s->giveSymbolIndices())
            expressionVar[x].erase(s);
    }
    
    std::string giveInfo(synExp* s) {
        std::ostringstream ost;
        ost << "[";
        ost << " " << std::setw(7) << showHuman(scoreComps[s]);
        ost << " " << std::setw(2) << difComps[s];
        ost << " " << std::setw(4) << s->getMax() - s->getMin();
        ost << " " << std::setw(4) << adapter->getNumComponents();
        ost << " " << std::setw(4) << adapter->getMaxComponent();
        ost << "]";
        return ost.str();
    }
    
    void computeScore(synExp* s) {
        s->computeMaxMin(myVar2pos);
        //std::cerr << "Expresion " << *it << std::endl;
        int score = 0, total = 0;
        std::set<int> theComps;
        // Which different components relate to this constraint
        for(int x : s->giveSymbolIndices())
            theComps.insert(adapter->getComponent(x));
        if (theComps.size() == 0) {
            std::cerr << "Expression " << s << " has zero components" << std::endl;
            exit(-1);
        }
        difComps[s] = 0;
        //std::cerr << "Components: ";
        for(int x : theComps) {
            //std::cerr << "c " << x << " size " << comp.getCompSize(x) << " ";
            difComps[s]++;
            int t =  adapter->getCompSize(x);
            if (t == 0) {
                std::cerr << "Component of " << x << " has zero size" << std::endl;
                exit(-1);
            }
            total += t;
            score -= t*t;
        }
        //std::cerr << " total " << total << std::endl;
        score += total*total;
        if (total == 0) {
            std::cerr << "Expression " << s << " has zero score" << std::endl;
            exit(-1);
        }
        scoreComps[s] =  score;
    }
};

#endif /* compSorter_hpp */
