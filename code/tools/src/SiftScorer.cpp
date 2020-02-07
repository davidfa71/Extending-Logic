//
//  SiftScorer.cpp
//  myKconf
//
//  Created by david on 03/11/15.
//  Copyright © 2015 david. All rights reserved.
//

#include "SiftScorer.hpp"



void SiftScorer::siftGo(double threshold) {
    double sc, other, improvement;
    std::cerr.precision(2);
    double origScore = computeOrderScore1();
//    std::cerr << "Order:";
//    for(auto var : pos2var)
//        std::cerr << var << " ";
    std::cerr << std::endl;
    //exit(-1);
    std::cerr << "Order Score:          " << std::fixed << std::showbase << std::right
    << origScore << std::endl;
    // Step one: We build a tree of menublocks from the menublock structure
    buildTreeOut();
    other = computeOrderScore1();
    do {
        sc = other;
        tree = sift_sequence(tree, other);
        improvement = 100.0*(sc - other)/sc;
        if (improvement < 0.0000000001) improvement = 0;
        std::cerr   << "Intermediate score: "
        << std::setw(10) << std::right << std::showbase << std::fixed << other
        << " improvement "        << std::right << std::setw(10)
        << std::showbase << std::fixed
        << improvement << "%" << std::endl;
        //std::cerr << "ZZZ score " << computeOrderScore1() << std::endl;


    } while (improvement > threshold);
    if (abs(other - computeOrderScore1()) > 0.1) {
        std::ostringstream ost;
        ost << "Error. relative score " << other << " absolute score " << computeOrderScore1() << " dif "
        << abs(computeOrderScore1() - other) << std::endl;
    }
    std::cerr << "Final score:        " << std::setw(10) << std::showbase << other
    << " reduction   "        << std::right << std::setw(10)
    << 100.0*(origScore - other)/origScore << "%" << std::endl;
    //std::cerr << "The tree after is : " << tree << std::endl;
    deleteTree(tree);
}

MenuBlock* SiftScorer::sift_sequence(MenuBlock* top, double& sc) {
    //std::cerr << "Enter sift_sequence " << top->getDescription() << " min " << top->getMin()
    // << " max " << top->getMax() << std::endl;
    int pos = 0;
    MenuBlock *t = top;
    std::list<MenuBlock*> seq;
    for(; t != NULL; t = t->getNext()) {
        t->setPos(pos++);
        seq.push_back(t);
        //std::cerr << t->getDescription() << std::endl;
    }
    for(MenuBlock* p : seq) {
        double oldsc = sc;
        sc = update_best_pos(p, seq.size()/2, sc);
        if (oldsc < sc) {
            std::cerr << "old score was better than this score" << std::endl;
            exit(-1);
        }
        
    }
    /* Find first block */
    for (;top->getPrev() != NULL ; top=top->getPrev());
    
    
    for(MenuBlock *trav = top; trav != NULL; trav = trav->getNext())
        if (trav->getSubTree() != NULL)
            trav->setSubTree(sift_sequence(trav->getSubTree(), sc));
        
    return top;
}



double SiftScorer::update_best_pos(MenuBlock* blk, int middlePos, double score) {
    double best    = score;
    //std::cerr << "P1 best is " << best << std::endl;
    int bestpos = 0;
    int dirIsUp = 1;
    int n;
    double rn;
    rn = score;
    /* Determine initial direction */
    if (blk->getPos() > middlePos)
        dirIsUp = 0;
    int min, max;
    // You get better results this way, sorry
    blk->getExpOutMaxMin(var2pos, min, max);
    //min = -1;
    //max = std::numeric_limits<int>::max();
    /* Move block back and forth */
    for (n=0 ; n<2 ; n++) {
        int first = 1;
        
        if (dirIsUp) {
            while ((blk->getPrev() != NULL) && blk->getPos() > min) {
                first = 0;
                MenuBlock *pre = blk->getPrev();
                rn -= computeOrderScore2(blk->getSynExpOut(), pre->getSynExpOut());
                blockdown(blk->getPrev());
                rn += computeOrderScore2(blk->getSynExpOut(), pre->getSynExpOut());
                bestpos--;
                if (rn < best) {
                    best        = rn;
                    bestpos = 0;
                    //std::cerr << "P2 best is " << best << std::endl;

                }
            }
        }
        else {
            while ((blk->getNext() != NULL) && blk->getPos() < max) {//  && (rn <= maxAllowed || first)) {
                first = 0;
                MenuBlock *nex = blk->getNext();
                rn -= computeOrderScore2(blk->getSynExpOut(), nex->getSynExpOut());
                blockdown(blk);
                rn += computeOrderScore2(blk->getSynExpOut(), nex->getSynExpOut());
                bestpos++;
                if (rn < best) {
                    bestpos = 0;
                    best        = rn;
                    //std::cerr << "P3 best is " << best << std::endl;
                }
            }
        }
        dirIsUp = !dirIsUp;
    }
    /* Move to best pos */
    //std::cerr << "---- bestpos " << bestpos << std::endl;
    while (bestpos < 0) {
        blockdown(blk);
        bestpos++;
        //std::cerr << "1 dif "
        //    << abs(computeOrderScore1()-best) << " bestpos " << bestpos << std::endl;
     
    }
    while (bestpos > 0) {
        blockdown(blk->getPrev());
        bestpos--;
        //std::cerr << "2 dif " << abs(computeOrderScore1()-best)
        //        << " bestpos " << bestpos << std::endl;
    }
    double temp = computeOrderScore1();

    if (abs(temp-best) > 0.1) {
        std::cerr << " best " << best << " score " << temp << " dif " << abs(temp-best) <<  std::endl;
        while (blk->getPrev() != NULL) {
            blockdown(blk->getPrev());
            bestpos--;
            std::cerr << "3 dif " << abs(computeOrderScore1()-best)
            << " bestpos " << bestpos << std::endl;
        }
        exit(-1);
    }
    //std::cerr << "Exit update_best_pos " << blk->getDescription() << std::endl;
    return best;
}
