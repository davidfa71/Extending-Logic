//
//  OneComponent.cpp
//  myKconf
//
//  Created by david on 08/07/16.
//  Copyright © 2016 david. All rights reserved.
//

#include "OneComponent.hpp"

OneComponent::OneComponent(int num) : num(num)                {
  for(int i = 0; i < num; i++)
    pos2var.push_back(i);
  };
OneComponent::OneComponent(std::vector<int>& var2pos, std::vector<int>& pos2var) : pos2var(pos2var) {}
 void    OneComponent::changeOrder(std::vector<int>  x)                { pos2var = x;   }
std::vector<int> OneComponent::getOrder()                              { return pos2var;}

 void    OneComponent::newVariable()                                   { pos2var.push_back(num++);}
 void    OneComponent::replayConstraints()                             {}
 int     OneComponent::getComponent(int x)                             { return 0; }
 int     OneComponent::getStart(int x)                                 { return 0; }
 int     OneComponent::getCompSize(int x)                              { return 0; }
 int     OneComponent::getNumComponents()                              { return 0; }
 int     OneComponent::getMaxLength()                                  { return 0; }
bool     OneComponent::join(synExp* s, bool changeOrder)               { return false; }

 void    OneComponent::reorder()                                       { 
			//std::cerr << "In OneComponent.reorder. Doing nothing." << std::endl;
}
 bool    OneComponent::isOutOfSync()                                   { return false;}
 void    OneComponent::sync()                                          {}
 int     OneComponent::find(int x)                                     { return 0;}
 void    OneComponent::showInfo(int x)                                 {}
 void    OneComponent::setUnchanged(int x)                             {}
void     OneComponent::printComponents(){};


std::set<std::pair<int, int> >              OneComponent::listComponents() {
    std::set<std::pair<int, int> > s;
    s.insert(std::pair<int, int>(0,pos2var.size()));
    return s;
}
void                                        OneComponent::checkLengths() {};

std::set<std::pair<int, int> > OneComponent::listChangedComponents() {
    std::set<std::pair<int, int> > s;
    s.insert(std::pair<int, int>(0,pos2var.size()));
    return s;
}


 float OneComponent::computeEntropy()				      { return 0; }
bool OneComponent::makeUnion(int x, int y, bool changeOrder) { return false; }

