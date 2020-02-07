//
//  OtherScorer.hpp
//  myKconf
//
//  Created by david on 04/11/15.
//  Copyright © 2015 david. All rights reserved.
//

#ifndef OtherScorer_hpp
#define OtherScorer_hpp

#include <stdio.h>

#include "BlockScorer.hpp"
#include "humanNums.hpp"

class OtherScorer : public BlockScorer {
    
    public :
    
    OtherScorer(const std::map<std::string, int>&        vM,
                const std::vector<synExp*>&              pC,
                const std::vector<MenuBlock*>&           mB,
                      std::vector<int>&                  p2v,
                      std::vector<int>&                  v2p) : BlockScorer(vM, pC, mB, p2v, v2p) {
    };
    
    void        otherHeuristic(double threshold);
    
    protected :
    
    std::vector<int> probTable, invOrder;
    MenuBlock*  permutate(MenuBlock* top);
    MenuBlock*  swapBlocks(MenuBlock* left, MenuBlock *right);
    
};

#endif /* OtherScorer_hpp */
