//
//  DynamicScorer.hpp
//  myKconf
//
//  Created by david on 12/01/16.
//  Copyright © 2016 david. All rights reserved.
//

#ifndef DynamicScorer_hpp
#define DynamicScorer_hpp

#include <stdio.h>
#include <vector>

#include "synExp.hpp"
class DynamicScorer {
public:
    DynamicScorer(int start, int length,
                  std::vector<int>& order,
                  std::vector<std::vector<synExp*> >& varConstr)
    : start(start), length(length), order(order), varConstr(varConstr) {};
    
    
private:
    
    int start, length;
    std::vector<int>& order;
    std::vector<std::vector<synExp*> >& varConstr;
};
#endif /* DynamicScorer_hpp */
