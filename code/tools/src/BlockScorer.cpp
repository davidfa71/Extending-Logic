//
//  BlockScorer.cpp
//  myKconf
//
//  Created by david on 03/11/15.
//  Copyright © 2015 david. All rights reserved.
//

#include "BlockScorer.hpp"


void BlockScorer::buildTreeAll() {
    tree = NULL;
    for(MenuBlock* m : menuBlocks) {
        m->setNext(NULL);
        m->setSubTree(NULL);
        m->setPrev(NULL);
        m->computeIndices(varMap);
        m->updateOrder(var2pos);
        m->buildExpAll(pendingConstraints);
        tree = dropTree(tree, NULL, m);
    }
    for(std::pair<std::string, int> p : varMap) {
        std::set<std::string> x;
        x.insert(p.first);
        MenuBlock *y = new MenuBlock(x.begin(), x.end(), "variable "+p.first, "");
        y->computeIndices(varMap);
        y->updateOrder(var2pos);
        y->setNext(NULL);
        y->setPrev(NULL);
        y->setSubTree(NULL);
        y->buildExpAll(pendingConstraints);
        tree = dropTree(tree, NULL, y);
    }
};


void BlockScorer::buildTreeOut() {
    tree = NULL;
    for(MenuBlock* m : menuBlocks) {
        m->setNext(NULL);
        m->setSubTree(NULL);
        m->setPrev(NULL);
        m->computeIndices(varMap);

        m->updateOrder(var2pos);
        m->buildExpOut(pendingConstraints);
            tree = dropTree(tree, NULL, m);
    }
    for(std::pair<std::string, int> p : varMap) {
        std::set<std::string> x;
        x.insert(p.first);
        MenuBlock *y = new MenuBlock(x.begin(), x.end(), "variable "+p.first, "");
        y->setNext(NULL);
        y->setPrev(NULL);
        y->setSubTree(NULL);
        y->computeIndices(varMap);
        y->updateOrder(var2pos);
        y->buildExpOut(pendingConstraints);
            tree = dropTree(tree, NULL, y);
    }
};

MenuBlock* BlockScorer::dropTree(MenuBlock *tree, MenuBlock *prev, MenuBlock* m) {
    // Void
    if (tree == NULL) {
        m->setPrev(prev);
        if (prev != NULL) {
            prev->setLength(prev->getLength()+1);
            prev->setNext(m);
        }
        return m;
    }
    
    // Identity check
    if (m->getMin() == tree->getMin() && m->getMax() == tree->getMax()
        && tree->getDescription() == m->getDescription()) {
        return m;
    }
    
    // Before this one
    if (m->getMax() < tree->getMin()) {
        m->setNext(tree);
        m->setPrev(prev);
        tree->setPrev(m);
        m->setLength(tree->getLength()+1);
        return m;
    }
    // After this one
    if (tree->getMax() < m->getMin()) {
        tree->setNext(dropTree(tree->getNext(), tree, m));
        tree->setLength(tree->getNext()->getLength()+1);
        return tree;
    }
    // Inside, goes to subtree
    if (m->getMin() >= tree->getMin() && m->getMax() <= tree->getMax()) {
        tree->setSubTree(dropTree(tree->getSubTree(), NULL, m));
        return tree;
    }
    
    // Above
    if (tree->getMin() >= m->getMin() && tree->getMax() <= m->getMax()) {
        m->setSubTree(dropTree(m->getSubTree(), NULL, tree));
        return m;
    }
    
    std::ostringstream ost;
    // Error. No more possibilities.
    ost   << "Error inserting " << m << " in " << tree << std::endl;
    // We don't stop. There may be an incompatibility between the components
    // and the blocks, in that case, we don't insert the block.
    std::cerr << ost.str() << std::endl;
    std::cerr << "Ordering " << std::endl;
    for(int x : pos2var)
        std::cerr << " (" << x << ") ";
    std::cerr << std::endl;
    throw(std::logic_error(ost.str()));
    return tree;
}

void BlockScorer::deleteTree(MenuBlock *tree) {
    if (tree->getNext() != NULL)
        deleteTree(tree->getNext());
    if (tree->getSubTree() != NULL)
        deleteTree(tree->getSubTree());
    delete tree;
}


void BlockScorer::blockdown(MenuBlock* left) {
    int n, l;
    MenuBlock *right = left->getNext();
 
    left->addIncrement(var2pos, pos2var, right->getSize());
    right->addIncrement(var2pos, pos2var, -left->getSize());

    /* Swap left and right data in the order */
    left->setNext(right->getNext());
    right->setPrev(left->getPrev());
    left->setPrev(right);
    right->setNext(left);
    
    if (right->getPrev() != NULL)
        right->getPrev()->setNext(right);
    
    if (left->getNext() != NULL)
        left->getNext()->setPrev(left);
    
    n = left->getPos();
    l = left->getLength();
    left->setPos(right->getPos());
    right->setPos(n);
    left->setLength(right->getLength());
    right->setLength(l);
}
