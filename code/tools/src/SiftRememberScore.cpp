//
//  SiftRememberScore.cpp
//  myKconf
//
//  Created by david on 22/07/16.
//  Copyright © 2016 david. All rights reserved.
//

#include "SiftRememberScore.hpp"

SiftRememberScore::SiftRememberScore(const std::string& varFile, const std::string& expFile) :
        SiftMinScore(varFile, expFile) {
            low.resize(variables.size());
            high.resize(variables.size());
};
                            

bool SiftRememberScore::stopCondition(const std::vector<int>& o, int pos1, int pos2) { return false; }
void SiftRememberScore::computeScore() {
    utScore tot = 0;
    low  = var2pos;
    high = var2pos;
    
    for(int exp = 0; exp < expressions.size(); exp++) {
        tot += computeScore(exp);
        //std::cerr << "computeScore " << std::setw(5) << exp << " tot " << tot << std::endl;

    }
    score = tot;
}

utScore SiftRememberScore::computeScore(int exp) {
    return auxCompute(exp, var2pos);
}


utScore SiftRememberScore::auxCompute(int exp, const std::vector<int>& w) {
    utScore  tot    = 0;
    int     smax    = -1;
    int     smin    = std::numeric_limits<int>::max();

    if (expressions[exp].size() == 2) {
        for(auto v : expressions[exp]) {
            if (low[v]  > smax) smax = low[v];
            if (high[v] < smin) smin = high[v];
        }
        for(auto v : expressions[exp]) {
           // if (smin < low[v])  low[v]  = smin;
            if (smax > high[v]) high[v] = smax;
        }
        if (smin > smax) smin = smax;
    }
    else {
        for(auto v : expressions[exp]) {
            if (w.at(v) > smax) smax = w.at(v);
            if (w.at(v) < smin) smin = w.at(v);
        }
    }
    return giveScore(smax, smin);
    //std::cerr << "\t += giveScore(" << std::setw(5) << smax << ", " << std::setw(5) << smin << ") = "
    //<< xx << std::endl;
    tot += xx;
    return tot;
}
//utScore SiftRememberScore::auxCompute(int exp, const std::vector<int>& w) {
//    int     smax    = -1;
//    int     smin    = std::numeric_limits<int>::max(), submin = std::numeric_limits<int>::max();
//    int     vmin, vsubmin;
//    if (expressions[exp].size() == 2) {
//        
//        for(auto v : expressions[exp]) {
//            if (w[v]  > smax)
//                smax   = w[v];
//            
//            if (high[v] < smin) {
//                submin  = smin;
//                vsubmin = vmin;
//                smin    = high[v];
//                vmin    = v;
//            }
//            else if (high[v] < submin) {
//                submin  = high[v];
//                vsubmin = v;
//            }
//        }
//        //std::cerr << "min var " << vmin << " pos " << smin << " submin var " << vsubmin
//        //<< " pos " << submin << " max pos " << smax;
//        
//        if (high[vmin] > smax)
//            smax = high[vmin];
//        //std::cerr << " score giveScore " << smax << ", " << high[vmin] << std::endl;
//        utScore s = giveScore(smax, smin);
//        if (high[vmin] < submin)
//            high[vmin] = submin;
//        return s;
//    }
//    else {
//        for(auto v : expressions[exp]) {
//            if (w.at(v) > smax) smax = w.at(v);
//            if (w.at(v) < smin) smin = w.at(v);
//        }
//    }
//    return giveScore(smax, smin);
//}


utScore SiftRememberScore::computeOrderScore(const std::vector<int>& v2p) {
    utScore tot = 0;
    low   = v2p;
    high  = v2p;
    for(int exp = 0; exp < expressions.size(); exp++) {
        tot += computeOrderScore(v2p, exp);
          //std::cerr << "computeOrderScore " << std::setw(5) << exp << " tot " << tot << std::endl;
    }

   // if (tot < min) {
        //std::cerr << "computeOrderScore for localScore is " << tot << std::endl;
   //     min = tot;
   // }
     //std::cerr << std::setw(5) << " Final value for tot " << tot << " o: ";
     //for(auto a : v2p) std::cerr << a << " "; std::cerr << std::endl;
    return tot;
}

utScore SiftRememberScore::computeOrderScore(const std::vector<int>& v2p, int exp) {
    return auxCompute(exp, v2p);
}

void SiftRememberScore::exchangeUpScore(std::vector<int>& v2p,
                                        std::vector<int>& p2v,
                                        std::vector<int>& eMin,
                                        std::vector<int>& eMax,
                                        std::vector<utScore>& eScore,
                                        std::vector<std::vector<bool> >& eps,
                                        int pos, utScore& localScore) {
    exchange(v2p, p2v, pos, pos-1);
    localScore          = computeOrderScore(v2p);
}

void SiftRememberScore::exchangeDownScore(std::vector<int>& v2p,
                                          std::vector<int>& p2v,
                                          std::vector<int>& eMin,
                                          std::vector<int>& eMax,
                                          std::vector<utScore>& eScore,
                                          std::vector<std::vector<bool> >& eps,
                                          int pos, utScore& localScore) {
    exchange(v2p, p2v, pos, pos+1);
    localScore          = computeOrderScore(v2p);
}

bool SiftRememberScore::sift(int var) {
    siftRes resUp, resDown;
    bool change = false;
    getBounds(var);
    std::cerr << "Sift " << std::setw(6) << var << " score " << score << std::endl;
    resUp   = siftUp  (score, var2pos, pos2var, expMin, expMax, expScore, mbExpPosSet, var);
    resDown = siftDown(score, var2pos, pos2var, expMin, expMax, expScore, mbExpPosSet, var);
    
    int pos = var2pos[var];
    if ((resUp.score < resDown.score) && resUp.score < score ) {
        while (pos != resUp.pos) {
            exchange(var2pos, pos2var, pos, pos-1);
            pos--;
        }
        score = resUp.score;
        change  = true;
    }
    else if (resDown.score < score) {
        while (pos != resDown.pos) {
            exchange(var2pos, pos2var, pos, pos+1);
            pos++;
         }
        score = resDown.score;
        change  = true;
    }
    return change;
}
