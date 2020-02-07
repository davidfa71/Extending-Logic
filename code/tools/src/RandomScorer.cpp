//
//  RandomScorer.cpp
//  myKconf
//
//  Created by david on 03/11/15.
//  Copyright © 2015 david. All rights reserved.
//

#include "RandomScorer.hpp"

void RandomScorer::randomHeuristic(double threshold) {
    double sc, other, improvement;
    sc = computeOrderScore1();
    std::cerr << "Random Initial score : " << showHuman(sc) << std::endl;
    // Step one: We build a tree of menublocks from the menublock structure
    
    
    buildTreeOut();
    tree->updateOrder(var2pos);
    //std::cerr << "The tree is" << std::endl << tree;
    randomHelper(tree, threshold);
    other       = computeOrderScore1();
    improvement = 100.0*(sc - other)/sc;
    std::cerr << "Final score : " << showHuman(other) << " improvement " << improvement <<  std::endl;
    deleteTree(tree);
}
void RandomScorer::randomHelper(MenuBlock *ptree, double threshold) {
    std::cerr << "Helping with " << ptree->getDescription() << std::endl;
    double sc, other, improvement;
    other = computeOrderScore1();

    do {
        sc          = other;
        ptree        = permutate(ptree);
        other       = computeOrderScore1();
        improvement = 100.0*(sc - other)/sc;
        std::cerr.precision(2);
        std::cerr << "Intermediate score: " << showHuman(other)
                  << " improvement "        << improvement << "%" << std::endl;
    } while (improvement > threshold);
    //std::cerr << "The tree is" << std::endl << tree << std::endl;
    for(MenuBlock *Treeterator = ptree; Treeterator != NULL; Treeterator = Treeterator->getNext()) {
        if (Treeterator->getSubTree() != NULL)
            randomHelper(Treeterator->getSubTree(), threshold);
    }
    
}
MenuBlock* RandomScorer::permutate(MenuBlock* top) {
    double sc, other;
    int len, cmax = 0;
    int first;
    int second;
    int counter = 0;
    int ntimes = 0;
    MenuBlock *oneBlock, *otherBlock;
    other = computeOrderScore1();
    //std::cerr << "Random Initial score : " << showHuman(other) << std::endl;
    len    = top->getLength();
    
    if (len > 2) {
        do {
            sc     = other;
            second = 1 + random() % (len - 1);
            first  = random() % second;
            //                std::cerr << std::endl << "Len is " << len << " first " << first << " second " << second;
            //                std::cerr << "Top is " << top << " len " << top->getLength() << std::endl;
            //                std::cerr << "order is " << std::endl << "*******" << std::endl;
            //                for(int x = 0; x < order.size(); x++)
            //                    std::cerr << "x : " << x << " " << order[x] << std::endl;
            //                std::cerr << std::endl << "********" << std::endl;
            
            oneBlock = otherBlock = top;
            for(int x = 0; x < first; x++)  oneBlock   = oneBlock->getNext();
            for(int x = 0; x < second; x++) otherBlock = otherBlock->getNext();
            //std::cerr << "left " << oneBlock->getDescription()  << "right " << otherBlock->getDescription() << std::endl;
            
            top = swapBlocks(oneBlock, otherBlock);
            other = computeOrderScore1();
            if (other >= sc) {
                top = swapBlocks(otherBlock, oneBlock);
                other = sc;
            }
            else {
                counter = 0;
            }
            if (counter > cmax) cmax = counter;
            if (ntimes % 10000 == 0)
                std::cerr   << "xIntermediate score: " << showHuman(other)
                            << " cmax:" << std::setw(6) << cmax << " len " << std::setw(6) << len << std::endl;
            
            counter++;// if (counter % 10000 == 0) std::cerr << "+";
            ntimes++;
            
        } while (counter < len);
    }
    MenuBlock* it = top;
    while (it->getPrev() != NULL)
        it = it->getPrev();
    
    while (it != NULL) {
        if (it->getSubTree() != NULL) {
            //std::cerr << "Permutate " << it->getDescription() << ":" << std::endl;
            //for(MenuBlock *t = it->getSubTree(); t != NULL; t = t->getNext()) std::cerr << t->getDescription() << " ";
            //std::cerr << std::endl;
            it->setSubTree(permutate(it->getSubTree()));
        }
        it = it->getNext();
    }
    it = top;
    while (it->getPrev() != NULL)
        it = it->getPrev();
    return it;
}

MenuBlock *RandomScorer::swapBlocks(MenuBlock* left, MenuBlock *right) {
    int leftsize = 0;
    int len = left->getLength();
    int cont = 0;
    for(MenuBlock *it = left->getNext(); it != right; it = it->getNext()) {
        std::cerr << "4 addIncrement menuBlock " << it->getDescription() << std::endl;
        if (it == NULL) {
            std::cerr << "it is NULL" << std::endl;
            exit(-1);
        }
        leftsize += it->getSize();
        std::cerr << "1 Incrementing " << it->getDescription() << " by " << right->getSize() - left->getSize() << " right->getSize() " << right->getSize()  << " left->getSize() "
            << left->getSize() << std::endl;
        it->addIncrement(var2pos, pos2var, right->getSize() - left->getSize());
        //std::cerr << "done" << std::endl;
        cont++;
        if (len < cont) {
            std::cerr << "Too many blocks, len " << len << " cont " << cont << std::endl;
            exit(-1);
        }
    }
    
    std::cerr << "2 Incrementing " << left->getDescription() << " by " << leftsize+right->getSize() << std::endl;
    
    left->addIncrement(var2pos, pos2var, leftsize+right->getSize());
    
    std::cerr << "3 Incrementing " << right->getDescription() << " by " << -leftsize - left->getSize() << std::endl;
    
    right->addIncrement(var2pos, pos2var, -leftsize - left->getSize());
    
    /* Swap left and right data in the order */
    if (left->getNext() != right) {
        //std::cerr << "P1" << std::endl;
        MenuBlock *x, *y;
        x = left->getNext();
        y = right->getPrev();
        left->setNext(right->getNext());
        right->setPrev(left->getPrev());
        left->setPrev(y);
        right->setNext(x);
    }
    else {
        //std::cerr << "P2" << std::endl;
        left->setNext(right->getNext());
        right->setPrev(left->getPrev());
        left->setPrev(right);
        right->setNext(left);
        
    }
    
    if (right->getPrev() != NULL)
        right->getPrev()->setNext(right);
    
    if (right->getNext() != NULL)
        right->getNext()->setPrev(right);
    
    
    if (left->getNext() != NULL)
        left->getNext()->setPrev(left);
    
    if (left->getPrev() != NULL)
        left->getPrev()->setNext(left);
    
    
    int n = left->getPos();
    left->setPos(right->getPos());
    right->setPos(n);
    
    n = left->getLength();
    left->setLength(right->getLength());
    right->setLength(n);
    
    MenuBlock *p = left;
    while (p->getPrev() != NULL)
        p = p->getPrev();
    
    return p;
}

