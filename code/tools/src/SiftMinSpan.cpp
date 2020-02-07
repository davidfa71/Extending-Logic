//
//  SiftMinSpan.cpp
//  myKconf
//
//  Created by david on 21/06/16.
//  Copyright © 2016 david. All rights reserved.
//

#include "SiftMinSpan.hpp"



SiftMinSpan::SiftMinSpan(const std::string varFile, const std::string expFile) {
    // We read the variables in order
    std::ifstream vf(varFile.c_str());
    std::ifstream ef(expFile.c_str());
    std::string temp;
    while (vf >> temp)
        variables.push_back(temp);
    
    // We parse the expression file
    kconf::synExpDriver driver;
    driver.parse_file(expFile);
    std::vector<synExp*> expList  = driver.getConstraints();
    // We initialize the order and the inverse mapping
    for(int x = 0; x < variables.size(); x++) {
        var2pos.push_back(x);
        pos2var.push_back(x);
        varMap[variables[x]] = x;
    }
    // Now we go with the expressions
    for(synExp *s : expList) {
        s->computeIndices(varMap);
        std::vector<int> vtemp;
        for(int i : s->giveSymbolIndices())
            vtemp.push_back(i);
        expressions.push_back(vtemp);
    }
    makeExpressionsVar();
}
void SiftMinSpan::makeExpressionsVar() {
    // For each var we make a list of the related expressions
    expressionsVar.resize(variables.size());
    for(int exp = 0; exp < expressions.size(); exp++)
        for(auto var : expressions[exp]) {
            if (std::find(expressionsVar[var].begin(), expressionsVar[var].end(), exp) == expressionsVar[var].end())
                expressionsVar[var].push_back(exp);
        }
}

int SiftMinSpan::findMinPos(int exp) {
    int min = std::numeric_limits<int>::max();
    for(auto x : expressions[exp])
        if (var2pos[x] < min)
            min = var2pos[x];
    return min;
}

int SiftMinSpan::findMaxPos(int exp) {
    int max = -1;
    for(auto x : expressions[exp])
        if (var2pos[x] > max)
            max = var2pos[x];
    return max;
}

void SiftMinSpan::computeMaxSpan() {
    int x;
    max = -1;
    for(int s = 0; s < expressions.size(); s++) {
        if ((x = computeSpan(s)) > max) {
            max = x;
            maxSet.clear();
            maxSet.insert(s);
        }
        else
            if (x == max)
                maxSet.insert(s);
    }
}

int SiftMinSpan::findOrderMaxSpan(std::vector<int>& v2p) {
    int x, e;
    int mymax = -1;
    for(int s = 0; s < expressions.size(); s++) {
        if ((x = computeOrderSpan(v2p, s)) > mymax) {
            mymax = x;
            e = s;
        }
    }
    return mymax;
}

void SiftMinSpan::go() {
    for(;;) {
        int oldmax = max;
        for(int i = 0; i < 100; i++) {
            computeMaxSpan();
            siftSet();
            computeScore();
            std::cerr   << "max " << std::setw(10) << max
                        << " score " << std::setw(10) << score
                        << " iterations " << std::setw(4) << i << std::endl;

        }
        if (oldmax == max) break;
        std::cerr << "max " << max << std::endl;
    }
    
}

int SiftMinSpan::getMaxSpan() { return max; }

std::vector<std::string> SiftMinSpan::getVars() {
    std::vector<std::string> res;
    for(int i = 0; i < pos2var.size(); i++)
        res.push_back(variables[pos2var[i]]);
    return res;
}

std::vector<int> SiftMinSpan::getVar2pos() {
    return var2pos;
}

std::vector<int> SiftMinSpan::getPos2var() {
    return pos2var;
}

std::vector<std::vector<int> > SiftMinSpan::getExps() {
    return expressions;
}

void SiftMinSpan::writeResults() {
    for(int i = 0; i < pos2var.size(); i++)
        std::cout << variables[pos2var[i]] << " ";
    std::cout << std::endl;
}

int SiftMinSpan::computeSpan(int exp) {
    int smax = -1;
    int smin = std::numeric_limits<int>::max();
    for(auto v : expressions[exp]) {
        if (var2pos[v] < smin) smin = var2pos[v];
        if (var2pos[v] > smax) smax = var2pos[v];
    }
    return smax - smin;
}

int SiftMinSpan::computeOrderSpan(const std::vector<int>& v2p, int exp) {
    int smax = -1;
    int smin = std::numeric_limits<int>::max();
    for(auto v : expressions[exp]) {
        if (v2p[v] < smin) smin = v2p[v];
        if (v2p[v] > smax) smax = v2p[v];
    }
    return smax - smin;
}

bool SiftMinSpan::tooBigSpan(const std::vector<int>& v2p, int var1, int var2) {
    for(auto exp : expressionsVar[var1])
        if (computeOrderSpan(v2p, exp) > max)
            return true;
    
    for(auto exp : expressionsVar[var2] )
        if (std::find(expressions[exp].begin(), expressions[exp].end(), var1) == expressions[exp].end())
            if (computeOrderSpan(v2p, exp) > max)
                return true;
    return false;
}

void SiftMinSpan::siftSet() {
    std::set<int> varPack;
    for(auto exp : maxSet) {
        for(int j : expressions[exp])
            varPack.insert(j);
    }
    siftDown(varPack);
    siftUp(varPack);
}

void SiftMinSpan::exchange(int pos1, int pos2) {
    int var1 = pos2var[pos1];
    int var2 = pos2var[pos2];
    
    var2pos[var1]            = pos2;
    var2pos[var2]            = pos1;
    pos2var[pos1]            = var2;
    pos2var[pos2]            = var1;
}

void SiftMinSpan::siftUp(std::set<int> varPack) {
    std::set<int>   varPos;
    int             rollcounter = 0;
    for(int x : varPack)
        varPos.insert(var2pos[x]);
    
    int posmaxblock         =   *std::max_element(varPos.begin(), varPos.end());
    int oldmax;
    for(;;) {
        int posminblock         =   posmaxblock;
        while (varPack.find(pos2var[posminblock-1]) != varPack.end())
            posminblock--;
        
        oldmax = max;
        // Now move block to the left
        if (posminblock > 0) {
            for(int x = posminblock; x <= posmaxblock; x++)
                exchange(x-1, x);
                
            posmaxblock--;
            posminblock--;
        }
        else
            break;
        
        computeMaxSpan();
        if (max > oldmax) {
            // If the span is now higher, we roll back last block movement
            for(int x = posmaxblock; x >= posminblock; x--)
                exchange(x, x+1);
            
            computeMaxSpan();
            posmaxblock++;
            posminblock++;
            if (max != oldmax) {
                std::cerr << "After rollback, max " << max << " oldmax " << oldmax << std::endl;
                exit(-1);
            }
            if (rollcounter++ < maxRollBacks)
                posminblock--;
            else
                break;
        }
        else
            rollcounter = 0;
        //else //If we're stuck
        //    if (max == oldmax) {
        //        std::cerr << " stuck " << std::endl;
        //        break;
        //    }

    }
    return;
}



void SiftMinSpan::siftDown(std::set<int> varPack) {
    std::set<int>   varPos;
    int             rollcounter = 0;
    for(int x : varPack)
        varPos.insert(var2pos[x]);
    
    int oldmax;
    int posminblock         =   *std::min_element(varPos.begin(), varPos.end());
    for(;;) {
        int posmaxblock         =   posminblock;
        while (varPack.find(pos2var[posmaxblock+1]) != varPack.end())
            posmaxblock++;
        
        oldmax = max;
        // Now move block to the right
        if (posmaxblock < var2pos.size() - 1) {
            for(int x = posmaxblock; x >= posminblock; x--)
                exchange(x, x+1);
            
            posminblock++;
            posmaxblock++;
        }
        else
            break;
        
        computeMaxSpan();
        // If the span is now higher, we roll back last block movement
        if (max > oldmax) {
            for(int x = posminblock; x <= posmaxblock; x++)
                exchange(x-1, x);
            
            computeMaxSpan();
            posminblock--;
            posmaxblock--;
            if (max != oldmax) {
                std::cerr << "After rollback, max " << max << " oldmax " << oldmax << std::endl;
                exit(-1);
            }

            if (rollcounter++ < maxRollBacks)
                posmaxblock++;
            else
                break;
        }
        else
            rollcounter = 0;
        //else //If we're stuck
        //    if (oldmax == max) {
        //        std::cerr << " stuck " << std::endl;
        //        break;
        //   }

    }
    return;
}


int SiftMinSpan::computeOrderSpan(const std::vector<int>& v2p) {
    int max = -1;
    for(int exp = 0; exp < expressions.size(); exp++) {
        int span = computeOrderSpan(v2p, exp);
        //std::cerr << "expression " << exp << " span " << span << std::endl;
        if  (span > max)
                max = span;
    }
    return max;
}

int SiftMinSpan::computeScore2(const std::vector<int>& v2p, int var1, int var2) {
    int max  = -1;
    for(auto exp : expressionsVar[var1]) {
        int span = computeOrderSpan(v2p, exp);
        if (span > max)
            max = span;
    }
    for(auto exp : expressionsVar[var2] )
        if (std::find(expressions[exp].begin(), expressions[exp].end(), var1) == expressions[exp].end()) {
            int span = computeOrderSpan(v2p, exp);
            if (span > max)
                max = span;

        }

    return max;
}

void SiftMinSpan::computeScore() {
    double tot = 0;
    for(int exp = 0; exp < expressions.size(); exp++) {
        tot += computeScore(exp);
    }
    
    score = tot;
    
}

double SiftMinSpan::computeScore(int exp) {
    int smax = -1;
    int smin = std::numeric_limits<int>::max();
    for(auto v : expressions[exp]) {
        if (var2pos.at(v) < smin) smin = var2pos.at(v);
        if (var2pos.at(v) > smax) smax = var2pos.at(v);
    }
    return log(smax - smin +1);
}
