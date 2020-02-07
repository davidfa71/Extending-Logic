//
//  forceScorer.cpp
//  myKconf
//
//  Created by david on 03/11/15.
//  Copyright © 2015 david. All rights reserved.
//

#include "forceScorer.hpp"

bool orderpos (const std::pair<double, int>& a,
               const std::pair<double, int>& b) {
    return (a.first < b.first); }

void forceScorer::force() {
    int span, newSpan;
    std::map<synExp*, double> cog;
    std::map<int, double> pos;
    int origScore = computeSpan();
    std::cout << "Span Score: " << showHuman(origScore) << std::endl;
    // Step one, build a list of a expressions for a given variable
    std::vector<std::list<synExp*> > mVE(pos2var.size());
    for(synExp *s : pendingConstraints)
        for(int index : s->giveSymbolIndices())
            mVE[index].push_back(s);
    // Step two, compute "span"
    do {
        span = computeSpan();
        for(synExp *s : pendingConstraints) {
            cog[s] = 0.0;
            for(int index: s->giveSymbolIndices())
                cog[s] += var2pos[index];
            
            if (s->giveSymbolIndices().size() != 0)
                cog[s] = cog[s] / s->giveSymbolIndices().size();
        }
        
        for(int x = 0; x < pos2var.size(); x++) {
            pos[x] = 0.0;
            for(synExp *s : mVE.at(x))
                pos[x] += cog.at(s);
            
            if (mVE.at(x).size() != 0)
                pos[x] = pos[x] / mVE.at(x).size();
            
            //invPos[pos.at(ss)] = ss;
        }
        std::vector<std::pair<double, int> > temp;
        for(std::pair<int, double> p : pos)
            temp.push_back(std::pair<double, int>(p.second, p.first));
        
        std::sort(temp.begin(), temp.end(), orderpos);
        int counter = 0;
        for(std::pair<double, int> p : temp) {
            pos2var[counter] = p.second;
            var2pos[p.second] = counter++;
        }
        
        newSpan = computeSpan();
    } while (newSpan < span);
    int other = computeSpan();
    std::cout.precision(2);
    std::cout << "Final score: " << showHuman(other)
              << " reduction "   << 100.0*(origScore - other)/origScore << "%" << std::endl;
}
