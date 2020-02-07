//
//  forceBlockScorer.hpp
//  myKconf
//
//  Created by david on 03/11/15.
//  Copyright © 2015 david. All rights reserved.
//

#ifndef forceBlockScorer_hpp
#define forceBlockScorer_hpp

#include <stdio.h>
#include <map>
#include <vector>

#include "BlockScorer.hpp"
#include "MenuBlock.hpp"
#include "humanNums.hpp"


class forceBlockScorer : public BlockScorer {
    
    public :
    
        forceBlockScorer(const std::map<std::string, int>&        vM,
                         const std::vector<synExp*>               pC,
                         const std::vector<MenuBlock*>            mB,
                               std::vector<int>&                  p2v,
                               std::vector<int>&                  v2p) : BlockScorer(vM, pC, mB, p2v, v2p) {};
    
        void forceBlocks();
  
    protected :
    
    void forceTree(MenuBlock *top);
    void forceSequence(const std::list<MenuBlock*>& seq, int min);
};

#endif /* forceBlockScorer_hpp */
