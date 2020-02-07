//
//  SiftScorer.hpp
//  myKconf
//
//  Created by david on 03/11/15.
//  Copyright © 2015 david. All rights reserved.
//

#ifndef SiftScorer_hpp
#define SiftScorer_hpp

#include <stdio.h>

#include "humanNums.hpp"
#include "BlockScorer.hpp"

class SiftScorer : public BlockScorer {
    
    public :
    
        SiftScorer(const std::map<std::string, int>&         vM,
                   const std::vector<synExp*>&               pC,
                   const std::vector<MenuBlock*>&            mB,
                   std::vector<int>&                         pos2var,
                   std::vector<int>&                         var2pos) : BlockScorer(vM, pC, mB, pos2var, var2pos) {};
    
        void      siftGo(double threshold);
    
    protected :
    double        update_best_pos(MenuBlock* blk, int middlePos, double score);
    MenuBlock*    sift_sequence(MenuBlock* top, double& sc);
};
#endif /* SiftScorer_hpp */
