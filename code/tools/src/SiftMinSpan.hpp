//
//  SiftMinSpan.hpp
//  myKconf
//
//  Created by david on 21/06/16.
//  Copyright © 2016 david. All rights reserved.
//

#ifndef SiftMinSpan_hpp
#define SiftMinSpan_hpp

#include <stdio.h>
#include <set>
#include <fstream>
#include <vector>
#include <map>

#include "humanNums.hpp"
#include "synExpDriver.hpp"

class SiftMinSpan {
    
    public :
    
    SiftMinSpan(const std::string varFile, const std::string expFile);
    
    void go();
    std::vector<std::string>                    getVars();
    std::vector<std::vector<int> >              getExps();
    std::vector<int>                            getPos2var();
    std::vector<int>                            getVar2pos();

    void                                        writeResults();
    int                                         getMaxSpan();
    
    
    protected :
    
    void    makeExpressionsVar();
    void    findMaxSpan();
    int     findOrderMaxSpan(std::vector<int>& v2p);
    int     findMinPos(int exp);
    int     findMaxPos(int exp);
    //      Maximum span considering all expressions
    void    computeMaxSpan();

    int     computeSpan(int exp);
    //      Span for this particular ordering and expression
    int     computeOrderSpan(const std::vector<int>& o, int exp);
    int     computeOrderSpan(const std::vector<int>& o);

    bool    tooBigSpan(const std::vector<int>& o, int pos1, int pos2);
    
    void    siftSet();
    void    siftUp  (std::set<int> varPack);
    void    siftDown(std::set<int> varPack);
    
    void    computeScore();
    int     computeScore2(const std::vector<int>& o, int pos, int otherpos);
    double  computeScore(int exp);
    
    void exchange(int pos1, int pos2);

    std::map<std::string, int>                      varMap;
    std::vector<std::vector<int> >                  expressions;
    std::vector< std::vector<int> >                 expressionsVar;
    std::vector<std::string>                        variables;
    std::vector<int>                                pos2var, var2pos;
    int                                             max;
    double                                          score;
    std::set<int>                                   maxSet;
    int                                             maxRollBacks = 10;
    
    
    
};
#endif /* SiftMinSpan_hpp */
