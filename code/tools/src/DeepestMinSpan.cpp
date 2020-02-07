//
//  DeepestMinSpan.cpp
//  myKconf
//
//  Created by david on 20/12/2016.
//  Copyright © 2016 david. All rights reserved.
//

#include "DeepestMinSpan.hpp"


DeepestMinSpan::DeepestMinSpan(const std::string& varFile, const std::string& expFile) : SiftMinScore(varFile, expFile){};

void DeepestMinSpan::go() {
    minScore = 99999999999999999;
    computeScore();
    std::cerr << "Score is " << std::setw(10) << std::fixed << score << " vars " << variables.size() << std::endl;
    MultiComponents comp(var2pos, pos2var);
    for(synExp* s : expList)
        comp.join(s, false);
    comp.reorder();
    computeScore();
    utScore startScore = score;
    utScore lastScore;
    std::cerr.precision(0);
    bool change = true;
    std::cerr << "Score is (after components) " << std::setw(10) << std::fixed << score << " vars " << variables.size() << std::endl;

    while (change) {
        lastScore = score;
        change = false;
        for(int i = 0; i < variables.size(); i++)
            change |= sift(i);
        
        //std::cerr << "MinScore is " << minScore << " " << showHuman(minScore) << std::endl;// << " minVar " << minVar << " minPos " << minPos  << " change " << change << std::endl;
        int pos = var2pos[minVar];
        
        if (change) {
            if (minPos < pos) {
                while (minPos != pos) {
                    exchangeUpScore(var2pos, pos2var, expMin, expMax, expScore, mbExpPosSet, pos, score);
                    pos--;
                }
                if (score != minScore) {
                    std::cerr << "score " << score << " minScore " << minScore << std::endl;
                    exit(-1);
                }
            }
            else {
                while (minPos != pos) {
                    exchangeDownScore(var2pos, pos2var, expMin, expMax, expScore, mbExpPosSet, pos, score);
                    pos++;
                }
                if (score != minScore) {
                    std::cerr << "score " << score << " minScore " << minScore << std::endl;
                    exit(-1);
                }
            }
        }
        double reduction, lastreduction;
        if (lastScore != 0 && startScore != 0) {
            reduction     = 100.0*(startScore - score)/startScore;
            lastreduction = 100.0*(lastScore - score)/lastScore;
        }
        else {
            reduction     = 0;
            lastreduction = 0;
        }
        std::cerr   << "Score is " << std::setw(10) << std::fixed << score << " " << showHuman(score)
        << " reduction " << std::setw(4) << std::fixed << reduction << "%"
        << " last reduction " << std::setw(4) << std::fixed << lastreduction << "%"<< std::endl;
        wrapUp();
    }

}

bool DeepestMinSpan::sift(int var) {
    siftRes resUp, resDown;
    bool change = false;
    getBounds(var);
    //std::cerr << "Sift " << std::setw(6) << var << " score " << score << " minScore " << minScore << std::endl;
    resUp   = siftUp  (score, var2pos, pos2var, expMin, expMax, expScore, mbExpPosSet, var);
    resDown = siftDown(score, var2pos, pos2var, expMin, expMax, expScore, mbExpPosSet, var);
    if (resUp.score < minScore) {
        minVar   = var;
        minPos   = resUp.pos;
        minScore = resUp.score;
        change   = true;
    }
    if (resDown.score < minScore) {
        minVar   = var;
        minPos   = resDown.pos;
        minScore = resDown.score;
        change   = true;
    }
    return change;
}
