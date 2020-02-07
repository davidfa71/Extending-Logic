//
//  PermScorer.hpp
//  myKconf
//
//  Created by david on 10/03/19.
//  Copyright © 2015 david. All rights reserved.
//

#ifndef PermScorer_hpp
#define PermScorer_hpp

#include <stdio.h>

#include "humanNums.hpp"
#include "BlockScorer.hpp"

class PermScorer : public BlockScorer {
    
    public :
    
        PermScorer(const std::map<std::string, int>&         vM,
                   const std::vector<synExp*>&               pC,
                   const std::vector<MenuBlock*>&            mB,
                   std::vector<int>&                         pos2var,
                   std::vector<int>&                         var2pos,
                   int num,
                   double threshold) : BlockScorer(vM, pC, mB, pos2var, var2pos) {
            this->num       = num;
            this->threshold = threshold;
            std::vector<int> work;
            for(int i = 0; i < num; i++) {
                work.push_back(i+1);
                if (i > 0)
                    firstPerm.push_back(work);
            }
            generateSequence(num);
        };
    
    void                permGo();
    
    protected :
    
    MenuBlock*          processTree(MenuBlock *tree);
    MenuBlock*          applyBestPerm(MenuBlock *before, MenuBlock *after,
                                      std::vector<MenuBlock*>& vec);
    std::vector<int>    applyPerms(std::vector<MenuBlock*> vec);
    void                generateSequence(int n);
    void                swapBlocks(MenuBlock* left, MenuBlock* right);
    MenuBlock*          slidingWindow(MenuBlock* sequence);


    int num;
    double score, threshold;
    std::vector<std::vector<int> >  firstPerm;
    std::vector<std::vector<int> >  permSeq;
};
#endif /* PermScorer_hpp */
