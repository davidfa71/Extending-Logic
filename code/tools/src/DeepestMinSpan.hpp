//
//  DeepestMinSpan.hpp
//  myKconf
//
//  Created by david on 20/12/2016.
//  Copyright © 2016 david. All rights reserved.
//

#ifndef DeepestMinSpan_h
#define DeepestMinSpan_h

#include "SiftMinScore.hpp"
#include "humanNums.hpp"

class DeepestMinSpan : public SiftMinScore {
public:
    
    DeepestMinSpan(const std::string& varFile, const std::string& expFile);
    void go();
    bool sift(int var);
    
protected:
    utScore minScore;
    int     minVar, minPos;
};
#endif /* DeepestMinSpan_h */
