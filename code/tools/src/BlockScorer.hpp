//
//  BlockScorer.hpp
//  myKconf
//
//  Created by david on 03/11/15.
//  Copyright © 2015 david. All rights reserved.
//

#ifndef BlockScorer_hpp
#define BlockScorer_hpp

#include <stdio.h>
#include <vector>

#include "Scorer.hpp"
#include "MenuBlock.hpp"

class BlockScorer : public Scorer {
    
    public :
    
        BlockScorer(const   std::map<std::string, int>&    vM,
                    const   std::vector<synExp*>&          pC,
                    const   std::vector<MenuBlock*>&       mB,
                            std::vector<int>&              pos2var,
                            std::vector<int>& var2pos) : Scorer(vM, pC, pos2var, var2pos), menuBlocks(mB) {};
    
        BlockScorer(const   std::map<std::string, int>&    vM,
                    const   std::vector<synExp*>&          pC,
                            std::vector<int>&              pos2var,
                            std::vector<int>&              var2pos) : Scorer(vM, pC, pos2var, var2pos) {};

    
    protected :
    
        std::vector<MenuBlock*> menuBlocks;
        MenuBlock*              tree;
        void                    buildTreeOut();
        void                    buildTreeAll();
        void                    deleteTree(MenuBlock* tree);
        MenuBlock*              dropTree(MenuBlock *tree, MenuBlock *prev, MenuBlock* m);
        void                    blockdown(MenuBlock* left);

    


};

#endif /* BlockScorer_hpp */
