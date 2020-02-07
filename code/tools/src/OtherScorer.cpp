//
//  OtherScorer.cpp
//  myKconf
//
//  Created by david on 04/11/15.
//  Copyright © 2015 david. All rights reserved.
//

#include "OtherScorer.hpp"

void OtherScorer::otherHeuristic(double threshold) {
    int total = 0, pivot, l, r, ran, fixed, dir, var, swap, swaporder, theSpan, newSpan, span, temp, temp2, last = 999999999;
    double sc, other, improvement = 1000;
    // First we want to find a constraint to improve
    
    do {
        sc = computeOrderScore1();
        for(int i = 0; i < pendingConstraints.size() ;i++) {
            total = 0;
            probTable.clear();
            for(synExp* s : pendingConstraints) {
                s->computeMaxMin(var2pos);
                span = s->getMax() - s->getMin();
                total += span+1;
                probTable.push_back(total);
            }
            ran = random() % total;
            l   = 0;
            r   = probTable.size();
            bool keep = true;
            pivot = -1;
            while (r-l > 1 && keep) {
                pivot = (r+l) / 2;
                if (probTable[pivot] > ran)
                    r = pivot;
                else
                    if (probTable[pivot] < ran)
                        l = pivot;
                if (probTable[pivot] == ran)
                    keep = false;
            }
            if (keep) {
                if (probTable[r] <= ran) pivot = r;
                else
                    if (probTable[l] <= ran) pivot = l;
            }
            //
            //        if (!(probTable[pivot] <= ran && probTable[pivot+1] > ran)) {
            //            std::cerr << "Wrong search, ran = " << ran << " pivot " << pivot << std::endl;
            //            std::cerr   << "probTable["  << pivot   << "]=" <<  probTable[pivot]
            //                        << " probTable[" << pivot+1 << "]=" <<  probTable[pivot+1] << std::endl;
            //            std::cerr << "l " << l << " r " << r << std::endl;
            //            exit(-1);
            //        }
            synExp *myconst = pendingConstraints[pivot];
            theSpan =  myconst->getMax() - myconst->getMin();
            
            //std::cerr   << "expression is " << myconst << " span is "
            //<< theSpan <<  std::endl;
            if (theSpan > 2) {
                int heads = random() % 2;
                if (heads == 0) {
                    dir = -1;
                    var   = myconst->giveMaxInd(var2pos);
                    fixed = myconst->giveMinInd(var2pos);
                }
                else {
                    dir = +1;
                    var   = myconst->giveMinInd(var2pos);
                    fixed = myconst->giveMaxInd(var2pos);
                }
                
                swaporder = var2pos[var] + dir*(random() % theSpan);
                swap      = pos2var[swaporder];
                //            std::cerr   << "heads "  << heads << " dir " << dir
                //            << " var "   << var   << "(" << order[var]   << ")"
                //            << " fixed " << fixed << "(" << order[fixed] << ")"
                //            << " swap "  << swap  << "(" << swaporder    << ")"
                //            << std::endl;
                
                //std::cerr << "temp = " << order[var] << std::endl;
                temp = var2pos[var];
                //std::cerr << "order[" << swap << "]=" << order[var] << std::endl;
                var2pos[swap] = var2pos[var];
                //std::cerr << "order[" << var << "]=" << swaporder << std::endl;
                var2pos[var]  = swaporder;
                
                //std::cerr << "invOrder[" << swaporder << "]=" << var << std::endl;
                pos2var[swaporder]  = var;
                //std::cerr << "invOrder[" << temp << "]=" << swap << std::endl;
                pos2var[temp]       = swap;
                
                //            for(int x = 0; x < varMap.size();x++) {
                //                if (invOrder[order[x]] != x) {
                //                    std::cerr << "x " << x << " order[" << x << "]=" << order[x] << "invOrder[" << order[x] << "]="
                //                              << invOrder[order[x]] << std::endl;
                //                    exit(-1);
                //                }
                //                if (order[invOrder[x]] != x) {
                //                    std::cerr << "x " << x << " invOrder[" << x << "]=" << invOrder[x] << "order[" << invOrder[x] << "]="
                //                              << order[invOrder[x]] << std::endl;
                //                    exit(-1);
                //                }
                //            }
                myconst->computeMaxMin(var2pos);
                newSpan =  myconst->getMax() - myconst->getMin();
                //std::cerr   << "expression is " << myconst << " span is "
                //<< newSpan <<  std::endl;
                
                if (newSpan > theSpan) {
                    std::cerr << "span is growing" << std::endl;
                    exit(-1);
                }
                other  = computeOrderScore1();
                
                if (other > last) {
                    //std::cerr << "Reverting" << std::endl;
                    //std::cerr << "temp = " << order[var] << std::endl;
                    temp  = var2pos[var];
                    //std::cerr << "temp2 = " << order[swap] << std::endl;
                    
                    temp2 = var2pos[swap];
                    //std::cerr << "order[" << swap << "]=" << order[var] << std::endl;
                    var2pos[swap] = var2pos[var];
                    //std::cerr << "order[" << var << "]=" << temp2 << std::endl;
                    var2pos[var]  = temp2;
                    
                    //std::cerr << "invOrder[" << swaporder << "]=" << var << std::endl;
                    pos2var[temp2]  = var;
                    //std::cerr << "invOrder[" << temp << "]=" << swap << std::endl;
                    pos2var[temp]       = swap;
                    //                for(int x = 0; x < varMap.size();x++) {
                    //                    if (invOrder[order[x]] != x) {
                    //                        std::cerr << "x " << x << " order[" << x << "]=" << order[x] << "invOrder[" << order[x] << "]="
                    //                        << invOrder[order[x]] << std::endl;
                    //                        exit(-1);
                    //                    }
                    //                    if (order[invOrder[x]] != x) {
                    //                        std::cerr << "x " << x << " invOrder[" << x << "]=" << invOrder[x] << "order[" << invOrder[x] << "]="
                    //                        << order[invOrder[x]] << std::endl;
                    //                        exit(-1);
                    //                    }
                    //                }
                }
                else {
                    last =  computeOrderScore1();
                    //std::cerr << "score " << std::setw(8) << (int) last << " " << std::setw(150) << myconst << " " << std::setw(7) << theSpan << std::endl;
               }
            }
        }
        other       = computeOrderScore1();
        improvement = 100.0*(sc - other) / sc ;
        std::cerr << "score " << (int) other << " improvement " << improvement << "%" <<  std::endl;
    } while (improvement > threshold);
}
