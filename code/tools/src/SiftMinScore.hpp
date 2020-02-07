//
//  SiftMinScore.hpp
//
//
//  Created by david on 21/06/16.
//  Copyright © 2016 david. All rights reserved.
//

#ifndef SiftMinScore_hpp
#define SiftMinScore_hpp

#include <stdio.h>
#include <set>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>

#include "MultiComponents.hpp"
#include "humanNums.hpp"
#include "synExpDriver.hpp"

//typedef unsigned long long utScore ;
//typedef long long tScore;
typedef double utScore;
typedef double tScore;

struct siftRes {
    siftRes() {
    score = std::numeric_limits<int>::max();
    };
    siftRes(utScore s, int pos) : score(s), pos(pos) {};
    utScore score;
    int pos;
};

class SiftMinScore {
    
    public :
    
    SiftMinScore(const std::string& varFile, const std::string& expFile);
    
    SiftMinScore(bool verbose, double threshold, int maxRounds, const std::string& varFile, const std::string& expFile);
    virtual void go();
    
    protected :
    
            double   factor = 2;
            utScore  giveScore(int a, int b);
            void     fillExpressionsVar();
            void     readInfo(const std::string& varFile, const std::string& expFile);
    virtual bool     sift(int var);
            int      computeSpan(int exp);
            int      upperBound, lowerBound;
            int      weight(int v);
    virtual void     computeScore();
    virtual utScore  computeScore(int exp);
    virtual utScore  computeSetScore(const std::vector<int>& v2p, std::set<int> exps);
    virtual utScore  computeOrderScore(const std::vector<int>& v2p, int exp, bool verbose);
    virtual utScore  computeOrderScore(const std::vector<int>& v2p);
            int      computeOrderSpan(const std::vector<int>& v2p, int exp);
    virtual bool     stopCondition(const std::vector<int>& o, int pos1, int pos2);
    
            void     siftSet();
    virtual siftRes  siftUp  (utScore startScore, std::vector<int> v2p, std::vector<int> p2v,
                             std::vector<int> eMin,
                             std::vector<int> eMax,
                             std::vector<utScore> eScore,
                             std::vector<std::vector<bool> > eps,
                             int var);
    virtual siftRes siftDown(utScore startScore, std::vector<int> v2p, std::vector<int> p2v,
                             std::vector<int> eMin,
                             std::vector<int> eMax,
                             std::vector<utScore> eScore,
                             std::vector<std::vector<bool> > eps,
                             int var);

    virtual void    exchangeUpScore  (std::vector<int>& v2p,
                                      std::vector<int>& p2v,
                                      std::vector<int>& eMin,
                                      std::vector<int>& eMax,
                                      std::vector<utScore>& eScore,
                                      std::vector<std::vector<bool> >& eps,
                                      int pos, utScore& localScore);
    virtual void    exchangeDownScore(std::vector<int>& v2p,
                                      std::vector<int>& p2v,
                                      std::vector<int>& eMin,
                                      std::vector<int>& eMax,
                                      std::vector<utScore>& eScore,
                                      std::vector<std::vector<bool> >& eps,
                                      int pos, utScore& localScore);
    
    
    
    virtual utScore  computeScore2(const std::vector<int>& v2p, int var1, int var2);
    
            void    wrapUp();
            void    exchange(std::vector<int>& v2p, std::vector<int>& p2v, int pos1, int pos2);
    virtual void    getBounds(int var);
    void    check(std::string message) {
        xx = score;
        computeScore();
        if (score  != xx) {
            std::cerr << message << " score " << score << " xx " << xx << std::endl;
            exit(-1);
        }

    }
    std::map<std::string, int>                      varMap;
    std::vector<std::set<int> >                     expressions;
    std::vector< std::set<int> >                    expressionsVar;
    std::vector<std::string>                        variables;
    std::vector<int>                                pos2var, var2pos;
    int                                             max;
    utScore                                         score, lastScore, xx;
    std::set<int>                                   maxSet;
    int                                             maxRollBacks = 10;
    std::vector<synExp*>                            expList;
    std::vector<int>                                expMin;
    std::vector<int>                                expMax;
    std::vector<utScore>                            expScore;
    std::vector<std::vector<bool> >                 mbExpPosSet;
    bool                                            verbose;
    int                                             maxRounds;
    double                                          threshold;

    
    
};
#endif /* SiftMinScore_hpp */
