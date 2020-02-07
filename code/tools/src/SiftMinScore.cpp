//
//  SiftMinScore.cpp
//  myKconf
//
//  Created by david on 21/06/16.
//  Copyright © 2016 david. All rights reserved.
//

#include "SiftMinScore.hpp"

class auxOrder {
public:
    auxOrder(int exp, int span) : exp(exp), span(span) {};
    int exp, span;
    bool operator()(auxOrder a, auxOrder b) {
        if (a.span != b.span)
            return a.span > b.span;
        return a.exp > b.exp;
    }
};


SiftMinScore::SiftMinScore(const std::string& varFile, const std::string& expFile) {
    verbose   = false;
    threshold = 1.0;
    maxRounds = std::numeric_limits<int>::max();
    readInfo(varFile, expFile);
    max = std::numeric_limits<int>::max();
    mbExpPosSet.resize(expressions.size());
    expMin.resize(expressions.size());
    expMax.resize(expressions.size());
    expScore.resize(expressions.size());

}

SiftMinScore::SiftMinScore(bool verbose, double threshold, int maxRounds, const std::string& varFile, const std::string& expFile) : verbose(verbose), threshold(threshold), maxRounds(maxRounds) {
    readInfo(varFile, expFile);
    max = std::numeric_limits<int>::max();
    mbExpPosSet.resize(expressions.size());
    expMin.resize(expressions.size());
    expMax.resize(expressions.size());
    expScore.resize(expressions.size());

}


void SiftMinScore::readInfo(const std::string& varFile, const std::string& expFile) {
    // We read the variables in order
    std::ifstream vf(varFile.c_str());
    std::ifstream ef(expFile.c_str());
    std::string temp;
    while (vf >> temp)
        variables.push_back(temp);
    
    // We parse the expression file
    kconf::synExpDriver driver;
    driver.parse_file(expFile);
    expList  = driver.getConstraints();
    // We initialize the order and the inverse mapping
    for(int x = 0; x < variables.size(); x++) {
        var2pos.push_back(x);
        pos2var.push_back(x);
        varMap[variables.at(x)] = x;
    }
    //std::cerr << "Read " << variables.size() << " variables " << std::endl;
    //exit(-1);
    // Now we go with the expressions
    for(synExp *s : expList) {
        s->computeIndices(varMap);
           std::set<int> vtemp;
           for(int i : s->giveSymbolIndices())
           vtemp.insert(i);
           expressions.push_back(vtemp);
    }
   fillExpressionsVar();
}



void SiftMinScore::go() {
    computeScore();
    std::cerr << "Score is " << std::setw(10) << std::fixed << score << " vars " << variables.size() << std::endl;
    MultiComponents comp;
    for(int p = 0; p < var2pos.size(); p++)
        comp.newVariable();
    for(synExp* s : expList)
        comp.join(s, false);
    comp.reorder();
    computeScore();
    utScore startScore = score;
    utScore lastScore;
    std::cerr.precision(10);
    bool change;
    std::cerr << "Score is (after MultiComponents) " << std::setw(10) << std::fixed << score << " vars " << variables.size() << std::endl;
    int rounds = 0;
    for(;;) {
        change    = false;
        lastScore = score;
        computeScore();
        if (lastScore != score) {
            std::cerr << "Last score " << lastScore << " score " << score << std::endl;
            exit(-1);
        }
        for(int i = 0; i < variables.size(); i++) {
            change |= sift(i);
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
        std::cerr   << "Score is " << std::setw(10) << std::fixed << score
                    << " reduction " << std::setw(4) << std::fixed << reduction << "%"
                    << " last reduction " << std::setw(4) << std::fixed << std::right << std::setw(15) <<  lastreduction << "%"<< std::endl;
        wrapUp();

        //std::cerr << "!Change " << !change << " other " << ((startScore - score)/startScore) << std::endl;
        if (!change) {
            std::cerr << "No changes in this last round"<< std::endl;
            break;
        }
        rounds++;
        if (rounds > maxRounds) {
            std::cerr << "Max rounds " << maxRounds << " achieved (" << rounds << ")" << std::endl;
            break;
        }
        if (lastreduction < threshold) {
            std::cerr << "Last reduction " << lastreduction << " is lower than threshold " << threshold << std::endl;
            break;
        }
    }
   }
    
void  SiftMinScore::wrapUp() {
    MultiComponents comp;
    for(int p = 0; p < var2pos.size(); p++)
        comp.newVariable();
    for(synExp* s : expList)
        comp.join(s, false);
    comp.reorder();
    computeScore();
//    if (fabs(score - temp) > 0.1) {
//        std::cerr << "There was an error. temp " << temp << " score " << score << std::endl;
//        exit(-1);
//    }
    //std::cerr << "Going to write result, score is " << std::setw(10) << std::fixed << score << std::endl;
    // Check that pos2var and var2pos are inverse of each other
    if (var2pos.size() != pos2var.size()) {
        std::cerr << "Error type 1" << std::endl;
        exit(-1);
    }
    for(int x = 0; x < pos2var.size(); x++)
        if (var2pos.at(pos2var.at(x)) != x || pos2var.at(var2pos.at(x)) != x) {
            std::cerr << "Error, var2pos and pos2var not inverses of each other" << std::endl;
            exit(-1);
        }
    
    std::set<int> check1, check2;
    for(int i : var2pos) check1.insert(i);
    for(int j : pos2var) check2.insert(j);
    if (check1.size() != check2.size()) {
        std::cerr << "Error type 2" << std::endl;
    }
    if (check1.size() != var2pos.size()) {
        std::cerr << "Error type 3" << std::endl;
    }
    int c = 0;
    for(int i : check1)
        if (i != c++) {
            std::cerr << "Error type 4 in " << i << " " << c-1 << std::endl;
            exit(-1);
        }
    c = 0;
    for(int i : check2)
        if (i != c++) {
            std::cerr << "Error type 5 in " << i << " " << c-1 << std::endl;
            exit(-1);
        }
    
    for(int x : pos2var)
        std::cout << variables.at(x) << " ";
    std::cout << std::endl;
    //std::cerr << "Done!" << std::endl;
}

int SiftMinScore::computeSpan(int exp) {
    int smax = -1;
    int smin = std::numeric_limits<int>::max();
    for(auto v : expressions[exp]) {
        if (var2pos[v] < smin) smin = var2pos[v];
        if (var2pos[v] > smax) smax = var2pos[v];
    }
    return smax - smin;
}

int SiftMinScore::computeOrderSpan(const std::vector<int>& v2p, int exp) {
    int smax = -1;
    int smin = std::numeric_limits<int>::max();
    for(auto v : expressions[exp]) {
        if (v2p[v] < smin) smin = v2p[v];
        if (v2p[v] > smax) smax = v2p[v];
    }
    return smax - smin;
}

bool SiftMinScore::stopCondition(const std::vector<int>& v2p, int var1, int var2) {
    for(auto exp : expressionsVar[var1])
        if (computeOrderSpan(v2p, exp) > max)
            return true;
    
    for(auto exp : expressions[var2] )
        if (std::find(expressions[exp].begin(), expressions[exp].end(), var1) == expressions[exp].end())
            if (computeOrderSpan(v2p, exp) > max)
                return true;
    return false;
}

bool SiftMinScore::sift(int var) {
    siftRes resUp, resDown;
    bool change = false;
    getBounds(var);
    if (verbose)
        std::cerr << "Sift " << std::setw(6) << var << std::setw(50) << variables.at(var) << " score " << score << std::endl;
    
    resUp   = siftUp  (score, var2pos, pos2var, expMin, expMax, expScore, mbExpPosSet, var);
    resDown = siftDown(score, var2pos, pos2var, expMin, expMax, expScore, mbExpPosSet, var);
    
    int pos = var2pos[var];
    //std::cerr << "Before pos " << pos << std::endl;
    if ((resUp.score < resDown.score) && resUp.score < score ) {
        //std::cerr << "minpos is " << resUp.pos << " pos " << pos << std::endl;
        while (pos != resUp.pos) {
            exchangeUpScore(var2pos, pos2var, expMin, expMax, expScore, mbExpPosSet, pos, score);
            pos--;
        }
        if (score != resUp.score) {
            std::cerr << "Down score " << score << " resUp.score " << resDown.score << std::endl;
            exit(-1);
        }
        change  = true;
    }
    else if (resDown.score < score) {
        //std::cerr << "minpos is " << resDown.pos << " pos " << pos << std::endl;

        while (pos != resDown.pos) {
            exchangeDownScore(var2pos, pos2var, expMin, expMax, expScore, mbExpPosSet, pos, score);
            pos++;
//            tScore temp = score;
//            computeScore();
//            if (abs(temp - score) > 0.1) {
//                std::cerr << "Down score " << temp << " real score " << score << std::endl;
//                exit(-1);
//            }
//            score = temp;
        }
        if (score != resDown.score) {
            std::cerr << "score " << score << " resDown.score " << resDown.score << std::endl;
            exit(-1);
        }
        change  = true;
    }
    //std::cerr << " pos " << var2pos[var] << std::endl;
    return change;
   }


void SiftMinScore::exchange(std::vector<int>& v2p, std::vector<int>& p2v, int pos1, int pos2) {
    int var1 = p2v.at(pos1);
    int var2 = p2v.at(pos2);
   
    v2p.at(var1)            = pos2;
    v2p.at(var2)            = pos1;
    p2v.at(pos1)            = var2;
    p2v.at(pos2)            = var1;
}

siftRes SiftMinScore::siftUp(utScore startScore, std::vector<int> v2p, std::vector<int> p2v,
                              std::vector<int> eMin,
                              std::vector<int> eMax,
                              std::vector<utScore> eScore,
                              std::vector<std::vector<bool> > eps,
                              int var) {

    //computeExpInfo(v2p, var);
    //std::cerr << "in siftUp" << std::endl;
    int pos                     =   v2p[var];
    utScore localScore           =   startScore;
    //std::cerr << "siftUp   " << std::setw(5) << var << " localScore " << localScore << std::endl;
    siftRes res(localScore, pos);
    //std::cerr << "siftup   " << std::setw(4) << var << " " << std::setw(4) << pos
    //        << " lowerBound " << std::setw(4) << lowerBound << std::endl;
    //std::cerr   << "1 SiftUp   "  << std::setw(4) << var << " "
    //    << std::setw(4) << pos << " localScore " << localScore <<  " real Score " << computeOrderScore(v2p) <<  std::endl;
    //    if (fabs(localScore - computeOrderScore(v2p)) >0.01) {
    //        exit(-1);
    //    }
    while (pos > lowerBound) {
        exchangeUpScore(v2p, p2v, eMin, eMax, eScore, eps, pos, localScore);
        pos--;
        //std::cerr   << "2 SiftUp   "  << std::setw(4) << var << " "
        //<< std::setw(4) << pos << " localScore " << localScore <<  " real Score " << computeOrderScore(v2p) <<  std::endl;
        //if (fabs(localScore - computeOrderScore(v2p)) >0.01) {
        //    exit(-1);
        //}

        //if (startScore*1.20 < localScore) {
            //std::cerr << "! startScore*1.20 " << startScore*1.20 << " localScore " << localScore;
            //exchangeDownScore(v2p, p2v, pos, localScore);
            //break;
        //}
        if (localScore < res.score) {
            res = siftRes(localScore, pos);
            //std::cerr  << "min var " << std::setw(6) << var << " localScore " << localScore << std::endl;
        }
    }
    return res;
}

siftRes SiftMinScore::siftDown(utScore startScore, std::vector<int> v2p, std::vector<int> p2v,
                               std::vector<int> eMin,
                               std::vector<int> eMax,
                               std::vector<utScore> eScore,
                               std::vector<std::vector<bool> > eps,
                               int var) {
    //computeExpInfo(v2p, var);
    //std::cerr << "in siftDown" << std::endl;

    int pos                      =   v2p[var];
    utScore localScore           =   startScore;
    //std::cerr << "siftDown " << std::setw(5) << var << " localScore " << localScore << std::endl;
    //std::cerr << "siftdown " << std::setw(4) << var << " " << std::setw(4) << pos
    //<< " upperBound " << std::setw(4) << upperBound << std::endl;

    siftRes res(localScore, pos);

    while (pos < upperBound - 1) {
        exchangeDownScore(v2p, p2v, eMin, eMax, eScore, eps, pos, localScore);
        pos++;
        //std::cerr   << "SiftDown "  << std::setw(4) << var  << " "
        //<< std::setw(4) << pos << " localScore " << localScore << std::endl; //<< " real Score " << computeOrderScore(v2p) <<  std::endl;
        //if (abs(localScore - computeOrderScore(v2p)) >0.01) {
        //    exit(-1);
        //}
        //if (startScore*1.20 < localScore) {
            //std::cerr << "! startScore*1.20 " << startScore*1.20 << " localScore " << localScore;

            //std::cerr << "!";
            //exchangeUpScore(v2p, p2v, pos, localScore);
        //    break;
        //}
        if (localScore < res.score) {
            //std::cerr << "min var " << std::setw(6) << var << " localScore " << localScore << std::endl;
            res = siftRes(localScore, pos);
        }
    }
    return res;
}

void SiftMinScore::exchangeUpScore(std::vector<int>& v2p,
                                   std::vector<int>& p2v,
                                   std::vector<int>& eMin,
                                   std::vector<int>& eMax,
                                   std::vector<utScore>& eScore,
                                   std::vector<std::vector<bool> >& eps,
                                   int pos, utScore& localScore) {
    localScore          -= computeScore2(v2p, p2v.at(pos),  p2v.at(pos-1));
    exchange(v2p, p2v, pos, pos-1);
    localScore          += computeScore2(v2p, p2v.at(pos), p2v.at(pos-1));
}

void SiftMinScore::exchangeDownScore(std::vector<int>& v2p,
                                     std::vector<int>& p2v,
                                     std::vector<int>& eMin,
                                     std::vector<int>& eMax,
                                     std::vector<utScore>& eScore,
                                     std::vector<std::vector<bool> >& eps,
                                     int pos, utScore& localScore) {
    localScore          -= computeScore2(v2p, p2v.at(pos), p2v.at(pos+1));
    exchange(v2p, p2v, pos, pos+1);
    localScore          += computeScore2(v2p, p2v.at(pos), p2v.at(pos+1));

}

void SiftMinScore::computeScore() {
    //throw std::runtime_error( "1 SiftMinScore");
    
    utScore tot = 0;
    for(int exp = 0; exp < expressions.size(); exp++)
        tot += computeScore(exp);
    
    score = tot;
}

utScore SiftMinScore::computeOrderScore(const std::vector<int>& v2p) {
     //throw std::runtime_error( "1 SiftMinScore");
    //std::cerr << "In SiftMinScore" << std::endl;
    //exit(-1);
    utScore tot = 0;
    for(int exp = 0; exp < expressions.size(); exp++) {
        tot += computeOrderScore(v2p, exp, true);
    }
    
    return tot;
}

utScore SiftMinScore::computeSetScore(const std::vector<int>& v2p, std::set<int> s) {
    utScore tot = 0;
    for(int exp : s) {
        tot += computeOrderScore(v2p, exp, false);
    }
    return tot;
}

utScore SiftMinScore::computeScore(int exp) {
     //throw std::runtime_error( "1 SiftMinScore");
    int factor =  1;
    int smax   = -1;
    int smin   = std::numeric_limits<int>::max();
    for(auto v : expressions[exp]) {
        factor += weight(v);
        //std::cerr << "var " << v << " " << variables[v] << " var2pos " << var2pos[v] << std::endl;
        if (var2pos.at(v) < smin) smin = var2pos.at(v);
        if (var2pos.at(v) > smax) smax = var2pos.at(v);
    }
    //std::cerr << "exp " << std::setw(80) << expList[exp] << "(" << smax << ", " << smin << ") " << giveScore(smax, smin)*factor << std::endl;

return giveScore(smax, smin);
//    tScore tot = 0;
//
//    for(auto v : expressions[exp])
//        for(auto w : expressions[exp])
//            if (var2pos.at(v) > var2pos.at(w))
//                tot += giveScore(var2pos.at(v), var2pos.at(w));
//    return tot;
}

utScore SiftMinScore::computeOrderScore(const std::vector<int>& v2p, int exp, bool verbose) {
//    tScore tot = 0;
//    for(auto v : expressions[exp])
//        for(auto w : expressions[exp])
//            if (v2p.at(v) > v2p.at(w))
//                tot += giveScore(v2p.at(v), v2p.at(w));
//    return tot;
     //throw std::runtime_error( "1 SiftMinScore");
    int factor =  1;
    int smax   = -1;
    int smin   = std::numeric_limits<int>::max();
    for(auto v : expressions.at(exp)) {
        factor += weight(v);
        if (v2p.at(v) < smin) smin = v2p.at(v);
        if (v2p.at(v) > smax) smax = v2p.at(v);
    }
    return giveScore(smax, smin);
}

int SiftMinScore::weight(int v) {
    return expressionsVar.at(v).size();
}

utScore SiftMinScore::computeScore2(const std::vector<int>& v2p, int var1, int var2) {
     //throw std::runtime_error( "1 SiftMinScore");
    utScore tot  = 0;
    for(auto exp : expressionsVar.at(var1)) {
        tot += computeOrderScore(v2p, exp, false);
    }
    
    for(auto exp : expressionsVar.at(var2) )
        if (std::find(expressions.at(exp).begin(), expressions.at(exp).end(), var1) == expressions.at(exp).end()) {
            tot += computeOrderScore(v2p, exp, false);
        }
    
    return tot;
}

utScore SiftMinScore::giveScore(int a, int b) {
    //return abs(a-b);
//    if (a != b)
//        return log(a - b);
//    else return 0;
 //   return (a-b)*(a-b);
//    if (a == b) return 0;
 //   return log(a-b);
//    return (a-b)*(a-b-1);
    if (a == b) return 0;
    return (a-b)*log(a-b);
    
//    if (a == b) return 0;
    if (a > b)
        return (a-b)*log(a-b);
}

void SiftMinScore::fillExpressionsVar() {
// For each var we make a list of the related expressions
expressionsVar.resize(variables.size());
for(int exp = 0; exp < expressions.size(); exp++)
    for(auto var : expressions.at(exp))
        expressionsVar.at(var).insert(exp);
    
}

void    SiftMinScore::getBounds(int var) {
    lowerBound = 0;
    upperBound = variables.size();
    //int min = std::numeric_limits<int>::max();
    //int max = -1;
    //for(auto exp : expressionsVar[var])
    //    for(int v : expressions[exp]) {
    //        if (var2pos[v] > max)
    //            max = var2pos[v];
    //        if (var2pos[v] < min)
    //            min = var2pos[v];
    //    }
    //lowerBound = min;
    //upperBound = max;
}
