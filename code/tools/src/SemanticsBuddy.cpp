//
//  SemanticsBuddy.cpp
//  myKconf
//
//  Created by david on 14/11/13.
//  Copyright (c) 2013 david. All rights reserved.
//

#include "SemanticsBuddy.hpp"

using namespace buddy;

SemanticsBuddy::SemanticsBuddy(std::string s) :
logicValue(std::pair<const bdd&, const bdd&>(bddfalse, bddfalse)) {
    isStr = true;
    strValue = s;
};

SemanticsBuddy::SemanticsBuddy(const bdd& a, const bdd& b) : logicValue(std::pair<const bdd&, const bdd&>(a, b)) {
    isStr = false;
}

std::ostream& operator<<(std::ostream& os, const SemanticsBuddy& t) {
   // if (t.isStr) os << t.strValue;
   // else {
   //     os << "{";
   //     os << t.logicValue.first;
   //     os << ", ";
   //     os << t.logicValue.second;
   //     os << "}";
   // }
    return os;
}

const SemanticsBuddy operator&&(const SemanticsBuddy& e1, const SemanticsBuddy& e2) {
   
    return SemanticsBuddy(  e1.logicValue.first & e2.logicValue.first,
                            e1.logicValue.first & bdd_ite(e2.logicValue.first, e1.logicValue.second | e2.logicValue.second, buddy::bddfalse)
                          );
}

const SemanticsBuddy operator||(const SemanticsBuddy& e1, const SemanticsBuddy& e2) {
    return SemanticsBuddy(   e1.logicValue.first   |  e2.logicValue.first,
                            (e1.logicValue.second & !e2.logicValue.first) |
                            (e2.logicValue.second & !e1.logicValue.first)  |
                            (e1.logicValue.second &  e2.logicValue.second));
}

const SemanticsBuddy operator==(const SemanticsBuddy& e1, const SemanticsBuddy& e2) {
    return SemanticsBuddy(
                        ((e1.logicValue.first   & e2.logicValue.first)    | (!e1.logicValue.first   & !e2.logicValue.first)) &
                         ((e1.logicValue.second & e2.logicValue.second)  | (!e1.logicValue.second & !e2.logicValue.second)),
                          buddy::bddfalse);
                          }

const SemanticsBuddy operator!=(const SemanticsBuddy& e1, const SemanticsBuddy& e2) {
    return SemanticsBuddy(!(e1 == e2));
}

const SemanticsBuddy operator!(const SemanticsBuddy& e) {
  
    return SemanticsBuddy(! e.logicValue.first | e.logicValue.second,
                e.logicValue.second);
}

const SemanticsBuddy operator<(const SemanticsBuddy& e1, const SemanticsBuddy& e2) {
    return SemanticsBuddy(bdd_ite(e2.logicValue.first,
                                 bdd_ite(e2.logicValue.second, ! e1.logicValue.first | e1.logicValue.second, buddy::bddtrue),
                                 !e1.logicValue.first & !e1.logicValue.second), buddy::bddfalse);
}

const SemanticsBuddy operator>(const SemanticsBuddy& e1, const SemanticsBuddy& e2) {
    return SemanticsBuddy(bdd_ite(e1.logicValue.first,
                            bdd_ite(e1.logicValue.second, ! e2.logicValue.first | e2.logicValue.second, buddy::bddtrue),
                          !e2.logicValue.first & !e2.logicValue.second), buddy::bddfalse);
}

bool equal(const SemanticsBuddy& e1, const SemanticsBuddy& e2) {
    return  (e1.logicValue.first == e2.logicValue.first) &
            (e1.logicValue.second == e2.logicValue.second);
}

bool different(const SemanticsBuddy& e1, const SemanticsBuddy& e2) {
    return (e1.logicValue.first != e2.logicValue.first) ||
    (e1.logicValue.second != e2.logicValue.second);
}

