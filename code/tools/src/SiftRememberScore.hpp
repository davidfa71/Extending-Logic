
//
//  SiftRememberScore.hpp
//  myKconf
//
//  Created by david on 22/07/16.
//  Copyright © 2016 david. All rights reserved.
//

#ifndef SiftRememberScore_hpp
#define SiftRememberScore_hpp

#include <stdio.h>

#include "SiftMinScore.hpp"

class SiftRememberScore : public SiftMinScore {
    public:
    utScore min = std::numeric_limits<utScore>::max();;
    SiftRememberScore(const std::string& varFile, const std::string& expFile);
    SiftRememberScore(std::vector<std::string> vars, std::vector<std::vector<int> > exps,
                 std::vector<int> var2pos,      std::vector<int> pos2var,
                 int maxspan);
    bool        stopCondition(const std::vector<int>& o, int pos1, int pos2);
    void        computeScore();
    utScore     computeScore(int exp);
    utScore     computeOrderScore(const std::vector<int>& v2p);
    utScore     computeOrderScore(const std::vector<int>& v2p, int exp);
    void        exchangeUpScore(std::vector<int>& v2p,
                                std::vector<int>& p2v,
                                std::vector<int>& eMin,
                                std::vector<int>& eMax,
                                std::vector<utScore>& eScore,
                                std::vector<std::vector<bool> >& eps,

                                int pos, utScore& localScore);
    void        exchangeDownScore(std::vector<int>& v2p,
                                  std::vector<int>& p2v,
                                  std::vector<int>& eMin,
                                  std::vector<int>& eMax,
                                  std::vector<utScore>& eScore,
                                  std::vector<std::vector<bool> >& eps,
                                  int pos, utScore& localScore);
    
    bool sift(int var);
    
    protected:
    
    std::vector<int> low, high;
    utScore auxCompute(int exp, const std::vector<int>& w);
    
    
};
#endif /* SiftRememberScore_hpp */
