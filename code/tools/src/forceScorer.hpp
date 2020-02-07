//
//  forceScorer.hpp
//  myKconf
//
//  Created by david on 03/11/15.
//  Copyright © 2015 david. All rights reserved.
//

#ifndef forceScorer_hpp
#define forceScorer_hpp

#include <stdio.h>
#include "Scorer.hpp"
#include "humanNums.hpp"

class forceScorer : public Scorer {
    
    public :
    
    forceScorer(const std::map<std::string, int>& vM,
                const std::vector<synExp*>&       pC,
                      std::vector<int>&           p2v,
                      std::vector<int>&           v2p) : Scorer(vM, pC, p2v, v2p) {};
    
        void force();
    
    protected :
    
    
};
#endif /* forceScorer_hpp */
