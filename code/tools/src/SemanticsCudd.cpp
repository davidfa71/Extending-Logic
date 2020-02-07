//
//  SemanticsCudd.cpp
//  myKconf
//
//  Created by david on 14/11/13.
//  Copyright (c) 2013 david. All rights reserved.
//

#include "SemanticsCudd.hpp"

using namespace cudd;


Cudd* SemanticsCudd::pmgr = NULL;

SemanticsCudd::SemanticsCudd(std::string s) :
logicValue(std::pair<const BDD&, const BDD&>(pmgr->bddZero(), pmgr->bddZero())) {
    isStr = true;
    strValue = s;
};


SemanticsCudd::SemanticsCudd(const BDD& a, const BDD& b) : logicValue(std::pair<const BDD&, const BDD&>(a, b)) {
    isStr = false;
}

std::ostream& operator<<(std::ostream& os, const SemanticsCudd& t) {
    if (SemanticsCudd::pmgr == NULL) {
        std::cerr << "Manager is NULL" << std::endl;
        exit(-1);
    }
    //if (t.isStr) os << t.strValue;
    //else os << "{" << (t.logicValue.first) << ", " << (t.logicValue.second) << "}";
    return os;
}

const SemanticsCudd operator&&(const SemanticsCudd& e1, const SemanticsCudd& e2) {
    return SemanticsCudd(e1.logicValue.first & e2.logicValue.first,
                         e1.logicValue.first & e2.logicValue.first.Ite(e1.logicValue.second | e2.logicValue.second,
                                                   SemanticsCudd::pmgr->bddZero())
                          );
    

}

const SemanticsCudd operator||(const SemanticsCudd& e1, const SemanticsCudd& e2) {
    return SemanticsCudd(   e1.logicValue.first   |  e2.logicValue.first,
                            (e1.logicValue.second & !e2.logicValue.first) |
                            (e2.logicValue.second & !e1.logicValue.first)  |
                            (e1.logicValue.second &  e2.logicValue.second));
}

const SemanticsCudd operator==(const SemanticsCudd& e1, const SemanticsCudd& e2) {
    return SemanticsCudd(
                        ((e1.logicValue.first   & e2.logicValue.first)    | (!e1.logicValue.first   & !e2.logicValue.first)) &
                         ((e1.logicValue.second & e2.logicValue.second)  | (!e1.logicValue.second & !e2.logicValue.second)),
                          SemanticsCudd::pmgr->bddZero());
                          }

const SemanticsCudd operator!=(const SemanticsCudd& e1, const SemanticsCudd& e2) {
    return SemanticsCudd(!(e1 == e2));
}

const SemanticsCudd operator!(const SemanticsCudd& e) {
    return SemanticsCudd(! e.logicValue.first | e.logicValue.second,
                                         e.logicValue.second);
}

const SemanticsCudd operator<(const SemanticsCudd& e1, const SemanticsCudd& e2) {
    return SemanticsCudd(e2.logicValue.first.Ite(
                                 e2.logicValue.second.Ite(! e1.logicValue.first | e1.logicValue.second, SemanticsCudd::pmgr->bddOne()),
                                 !e1.logicValue.first & !e1.logicValue.second), SemanticsCudd::pmgr->bddZero());
}

const SemanticsCudd operator>(const SemanticsCudd& e1, const SemanticsCudd& e2) {
    return SemanticsCudd(e1.logicValue.first.Ite(
                                       e1.logicValue.second.Ite(!e2.logicValue.first | e2.logicValue.second, SemanticsCudd::pmgr->bddOne()),
                          !e2.logicValue.first & !e2.logicValue.second), SemanticsCudd::pmgr->bddZero());
}

bool equal(const SemanticsCudd& e1, const SemanticsCudd& e2) {
    return  (e1.logicValue.first == e2.logicValue.first) &
            (e1.logicValue.second == e2.logicValue.second);
}

bool different(const SemanticsCudd& e1, const SemanticsCudd& e2) {
    return (e1.logicValue.first != e2.logicValue.first) ||
    (e1.logicValue.second != e2.logicValue.second);
}

