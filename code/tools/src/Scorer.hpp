//
//  Scorer.hpp
//  myKconf
//
//  Created by david on 03/11/15.
//  Copyright © 2015 david. All rights reserved.
//

#ifndef Scorer_hpp
#define Scorer_hpp

#include <stdio.h>  
#include <map>
#include <string>
#include <vector>
#include "synExp.hpp"

class Scorer {
    
    public :

        Scorer(const    std::map<std::string, int>& vM,
               const    std::vector<synExp*>&       pC,
                        std::vector<int>&           p2v,
                        std::vector<int>&           v2p) : varMap(vM), pendingConstraints(pC), pos2var(p2v), var2pos(v2p) {
            scoreTable.push_back(0);
            for(int i = 1; i <= var2pos.size();i++)
                scoreTable.push_back(i*log(i));
            
        };
    
        std::vector<int>::iterator                              begin() { return pos2var.begin(); }
        std::vector<int>::iterator                              end()   { return pos2var.end();   }
    virtual double                      computeOrderScore1();

    
    
    protected :
    
        std::vector<double>                 scoreTable;
        const std::map<std::string, int>&   varMap;
        const std::vector<synExp*>&         pendingConstraints;
        std::vector<int>&                   pos2var;
        std::vector<int>&                   var2pos;
    
        int                                 computeSpan();
        virtual double                      computeOrderScore2(const std::vector<synExp*>& v, const std::vector<synExp*>& w);
        virtual double                      computeExp(synExp *s) const;
        void check();
};

#endif /* Scorer_hpp */

