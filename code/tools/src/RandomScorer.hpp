//
//  RandomScorer.hpp
//  myKconf
//
//  Created by david on 03/11/15.
//  Copyright © 2015 david. All rights reserved.
//

#ifndef RandomScorer_hpp
#define RandomScorer_hpp

#include "BlockScorer.hpp"
#include "humanNums.hpp"
#include <stdio.h>

class RandomScorer : public BlockScorer {
    
    public :
    
        RandomScorer(const std::map<std::string, int>&        vM,
                     const std::vector<synExp*>&              pC,
                     const std::vector<MenuBlock*>&           mB,
                           std::vector<int>&                  p2v,
                           std::vector<int>&                  v2p) : BlockScorer(vM, pC, mB, p2v, v2p) {};
    
        void        randomHeuristic(double threshold);

    protected :
    
        void        randomHelper(MenuBlock *ptree, double threshold);
        MenuBlock*  permutate(MenuBlock* top);
        MenuBlock*  swapBlocks(MenuBlock* left, MenuBlock *right);
    
};
#endif /* RandomScorer_hpp */
