//
//  synExp.cpp
//  myKconf
//
//  Created by david on 04/12/13.
//  Copyright (c) 2013 david. All rights reserved.
//

#include "synExp.hpp"

std::vector<int> synExp::ocSoFar;
std::vector<int> synExp::weights;
//std::map<std::string&, std::set<synExp*> > synExp::inverse;

std::ostream& operator<<(std::ostream& os, synExp* ps) {
    if (ps == NULL) {
        os << "NULL";
        return os;
    }
    ps->print(os);
    return os;
}


void synExp::print(std::ostream& os) const {
    return;
}

void synConst::print(std::ostream& os) const {
    switch (get_const()) {
            case synConst_true   : os << "synTrue";   break;
            case synConst_false  : os << "synFalse";  break;
            case synConst_module : os << "synModule"; break;
        default: os << "Error, const is " << get_const() << std::endl;
    }
}

void synSymbol::print(std::ostream& os) const {
    //os  << std::setw(len) << getSymbol();
    os  << getSymbol();
}

void synCompound::print(std::ostream& os) const {
    std::ostringstream ost1, ost2, ost3;
    std::string stFirst, stSecond, stThird;
    if (Ce1 == NULL || Ce2 == NULL || Ce3 == NULL) {
        std::cerr << "Null pointer in SynCompound::print" << std::endl;
        exit(-1);
    }
    if (Ce1 &&  (Ce1->opPriority() < opPriority() ||
        (Ce1->get_type() == synExp_Compound && Ce1->get_op() == Cop)))  {
        ost1 << Ce1;
        stFirst = ost1.str();
    }
    else {
        ost1 << "(" << Ce1 << ")";
        stFirst = ost1.str();
    }
    if (Ce2 && !Ce2->isIf() &&  (Ce2->opPriority() < opPriority() ||
        (Ce2->get_type() == synExp_Compound && Ce2->get_op() == Cop)))  {
        ost2 << Ce2;
        stSecond = ost2.str();
    }
    else {
        ost2 << "(" << Ce2 << ")";
        stSecond = ost2.str();
    }
    if (Ce3 && (Ce3->opPriority() < opPriority() ||
        (Ce3->get_type() == synExp_Compound && Ce3->get_op() == Cop)))  {
        ost3 << Ce3;
        stThird = ost3.str();
    }
    else {
        ost3 << "(" << Ce3 << ")";
        stThird = ost3.str();
    }
    switch (Cop) {
        case synAnd :
            os << stFirst << " and " << stSecond; break;
        case synOr  : {
            //if (stFirst.length() <= len/2 - 2) {
            //    os << std::right << std::setw(len/2 -2) << stFirst  << " or "
            //                    << std::left  << std::setw(len/2 -2) << stSecond; break;
            //}
            //else {
            //    os  << std::setw(0) << stFirst << " or " << std::setw(len - stFirst.length() - 4)
            //                << stSecond; break;
            //}
            os  << stFirst << " or " << stSecond; break;
        }
        case synNot : {
            //os << std::setw(0) << "not " << std::setw(len-4) << stFirst; break;
            os << "not " << stFirst; break;
        }
        case synImplies: {
            //os << "len =" << len << " ";
            //os << stFirst  << " -> " <<  stSecond; break;
            //if (stFirst.length() <= len/2 - 2) {
            //    os << std::right << std::setw(len/2 -2) << stFirst  << " -> "
            //        << std::left  << std::setw(len/2 -2) << stSecond; break;
            //}
            //else {
            //    os  << std::setw(0) << stFirst << " -> " << std::setw(len - stFirst.length() - 4)
            //        << stSecond; break;
            //}
            os << stFirst << " -> " << stSecond; break;
        }
        case synIfThenElse : {
            int len  = int(os.width());
            int len1 = int(stFirst.length());
            int len2 = int(stSecond.length());
            //os << "len =" << len << " ";
            if (Ce3 == synTrue || Ce3 == NULL)
                os << std::setw(3) << "if " << stFirst << " then "
                   <<  std::setw(len - len1 - 9) << stSecond;
            else
                os << std::setw(3) << "if " << stFirst << " then " << stSecond
                   << " else " << std::setw(len - len1 - len2 - 16) << stThird;
            break;
            //os << "if " << stFirst << " then " << stSecond << " else " << stThird << " endif ";
        }
        case synEqual : {
            //if (stFirst.length() <= len/2 - 1) {
            //    os << std::right << std::setw(len/2 -1) << stFirst  << " = "
            //      << std::left  << std::setw(len/2 -2) << stSecond; break;
            //}
            //os << stFirst << " = "
            //   << std::setw(len - stFirst.length() - 3) << stSecond; break;
            os << stFirst << " = " << stSecond; break;
        }
        default : os << "Error, operator is " << get_op() << std::endl;
    }
}

void synXOR::print(std::ostream& os) const {
    os << "XOR(";
    bool first = true;
    for(synExp* parm : v)
        if (first) {
            os << parm;
            first = false;
        }
        else
            os << " " << parm;
    os << ")";
}
    
void synString::print(std::ostream& os) const {
    os << '"' << st << '"';
}

bool equal(synExp *e1, synExp *e2) {
    if ((e1 == NULL && e2 == NULL) || (e1 == synFalse && e2 == NULL) || (e1 == NULL && e2 == synFalse))
        return true;
    if (e1->get_type() != e2->get_type())
        return false;
    switch (e1->get_type()) {
        case synExp_Const : { return e1 == e2; }
        case synExp_Symbol :
        { return e1->getSymbol() == e2->getSymbol();}
        case synExp_Compound : {
            if (e1->get_op() != e2->get_op()) return false;
            //std::set<std::string>::iterator it1;
            //std::set<std::string>::iterator it2;
            //for(it1 = e1->giveSymbols().begin(),
            //   it2 = e2->giveSymbols().begin();
            //    it1 != e1->giveSymbols().end() && it2 != e2->giveSymbols().end();
            //    it1++, it2++)
            //    if (*it1 != *it2) return false;
            //if (it1 != e1->giveSymbols().end() || it2 != e2->giveSymbols().end())
            //    return false;
            if (e1->get_op() == synOr || e1->get_op() == synAnd)
                return (equal(e1->first(), e2->first())  && equal(e1->second(), e2->second()))
                    || (equal(e1->first(), e2->second()) && equal(e1->second(), e2->first()));
            
            return    (equal(e1->first(), e2->first())
                       && equal(e1->second(), e2->second())
                       && equal(e1->third(), e2->third()));
        }
        case synExp_String : { return e1->get_string() == e2->get_string(); }
        case synExp_XOR : {
            // We need this to work at lest when everything is the same
            // for eval to do its work
            std::vector<synExp*> one, two;
            one = e1->get_parms();
            two = e2->get_parms();
            if (one.size() != two.size())
                return false;
            
            for(int i = 0; i < one.size(); i++)
                if (!equal(one[i], two[i]))
                    return false;
            
            return true;
        }
        case synExp_Unknown : { throw std::logic_error("Calling equal on synUnknown"); }
    }
}


synConst *synConst::syncTrue, *synConst::syncFalse, *synConst::syncModule;

synExp *synTrue   = synConst::getSynTrue();
synExp *synModule = synConst::getSynModule();
synExp *synFalse  = synConst::getSynFalse();


synExp *makeAnd(synExp* e1, synExp* e2) {
    if (e1 == synTrue)                        return e2;
    if (e2 == synTrue)                        return e1;
    if (e1 == synFalse) {
        e2->destroy();
        return synFalse;
    }
    if (e2 == synFalse) {
        e1->destroy();
        return synFalse;
    }
    if (equal(e1, e2))  {
        e2->destroy();
        return e1;
    }
    synExp* temp = makeNot(e2->copy_exp());
    if (equal(e1, temp)) {
        e1->destroy();
        e2->destroy();
        temp->destroy();
        return synFalse;
    }
    else
        temp->destroy();
    
    
    if (e1->giveOps().size() == 1 &&
        e1->giveOps().find(synAnd) != e1->giveOps().end() &&
        e2->giveOps().size() == 1 &&
        e2->giveOps().find(synAnd) != e2->giveOps().end()) { 
        synExp* res = synTrue;
        std::set<std::string> soFar;
        for(std::set<std::string>::iterator it  = e1->giveSymbols().begin();
                                            it != e1->giveSymbols().end();it++)
            if (soFar.find(*it) == soFar.end()) {
                res = new synCompound(synAnd, res, new synSymbol(*it));
                soFar.insert(*it);
            }
        for(std::set<std::string>::iterator it  = e2->giveSymbols().begin();
            it != e2->giveSymbols().end();it++)
            if (soFar.find(*it) == soFar.end()) {
                res = new synCompound(synAnd, res, new synSymbol(*it));
                soFar.insert(*it);
            }
  
        e1->destroy();
        e2->destroy();
        return res;
    }
    return new synCompound(synAnd, e1, e2);
}

synExp *makeOr(synExp* e1, synExp* e2) {
    if (e1 == synFalse) return e2;
    if (e2 == synFalse) return e1;
    if ((e1 == synTrue) || (e2 == synTrue)) return synTrue;
    if (equal(e1, e2)) {
        e2->destroy();
        return e1;
    }
    synExp *temp =  makeNot(e2->copy_exp());
    if (equal(e1, temp)) {
        e1->destroy();
        e2->destroy();
        temp->destroy();
        return synTrue;
    }
    else
        temp->destroy();
    
    if (e1->giveOps().size() == 1 &&
        e1->giveOps().find(synOr) != e1->giveOps().end() &&
        e2->giveOps().size() == 1 &&
        e2->giveOps().find(synOr) != e2->giveOps().end()) {
        //std::cout <<  "e1 "  << std::setw(50) << e1
        //          <<  " e2 " << std::setw(50) << e2 << std::endl;
        synExp* res = synFalse;
        std::set<std::string> soFar;
        for(std::set<std::string>::iterator it  = e1->giveSymbols().begin();
            it != e1->giveSymbols().end();it++)
            if (soFar.find(*it) == soFar.end()) {
                //std::cout << "1 *it " << *it << std::endl;
                res = new synCompound(synOr, res, new synSymbol(*it));
                soFar.insert(*it);
            }
        for(std::set<std::string>::iterator it  = e2->giveSymbols().begin();
            it != e2->giveSymbols().end();it++)
            if (soFar.find(*it) == soFar.end()) {
                //std::cout << "2 *it " << *it << std::endl;
                res = new synCompound(synOr, res, new synSymbol(*it));
                soFar.insert(*it);
            }
        e1->destroy();
        e2->destroy();
        //std::cout << " res " << std::setw(30) << res << std::endl;
        return res;
        
    }

    return new synCompound(synOr, e1, e2);
}

synExp *makeImplies(synExp* e1, synExp* e2) {
    synExp *res = NULL;
    if (e1 == synFalse)  return synTrue;
    if (e1 == synTrue)   return e2;
    if (e1 == synModule) return e2;
    if (e2 == synFalse)  return makeNot(e1);
    if (e2 == synTrue)   return synTrue;
    if (e1->isNot() && e2->isNot()) {
        res =  makeImplies(makeNot(e2->copy_exp()), makeNot(e1->copy_exp()));
        e1->destroy();
        e2->destroy();
        return res;
    }
    return new synCompound(synImplies, e1, e2);
}


synExp *makeNot(synExp* e1) {
    synExp *res = NULL;
    if (e1 == synFalse)  return  synTrue;
    if (e1 == synTrue)   return  synFalse;
    if (e1 == synModule) return  synModule;
    
    if (e1->isNot()) {
        res =  e1->first()->copy_exp();
        e1->destroy();
        return res;
    }
    return new synCompound(synNot, e1);
}

synExp *makeIfThenElse(synExp* e1, synExp* e2, synExp* e3 = synTrue) {
    synExp *res = NULL;
    if (e1 == synTrue)   {
        e3->destroy();
        return e2;
    }
    if (e1 == synModule) {
        e3->destroy();
        return e2;
    }
    if (e1 == synFalse)  {
        e2->destroy();
        return e3;
    }
    if (e2 == synTrue  && e3 == synFalse) {
        return e1;
    }
    if (e2 == synFalse && e3 == synTrue)  {
        return makeNot(e1);
    }
    if (e2 == synFalse && e3 == synFalse) {
        e1->destroy();
        return synFalse;
    }
    if (e2 == synTrue  && e3 == synTrue)  {
        e1->destroy();
        return synTrue;
    }
    if (e2 == synTrue) {
        res =  makeIfThenElse(makeNot(e1->copy_exp()),e3);
        e1->destroy();
        e2->destroy();
        return res;
    }
    if (equal(e2, e3)) {
        res = e2;
        e1->destroy();
        e3->destroy();
        return res;
    }
   
    if ((e3 == synTrue) && (e1->isNot()) && (e2->isNot())) {
        res = makeIfThenElse(e2->first()->copy_exp(), e1->first()->copy_exp(), synTrue);
        e1->destroy();
        e2->destroy();
        return res;
    }
    synExp* temp = makeNot(e3->copy_exp());
    if (equal(e1, e2) && (equal(e2, temp) || e3 == synTrue)) {
        temp->destroy();
        e1->destroy();
        e2->destroy();
        e3->destroy();
        return synTrue;
    }
    temp->destroy();
    temp = makeNot(e2->copy_exp());

    if (equal(e1, temp) && e3 == synTrue) {
        e1->destroy();
        e2->destroy();
        e3->destroy();
        return temp;
    }
    temp->destroy();
    
    return new synCompound(synIfThenElse, e1, e2, e3);
}

synExp *makeEqual(synExp *e1, synExp *e2) {
    synExp *res = NULL;
    if (e2 == synTrue) {
        return e1;
    }
    if (e1 == synTrue)  {
        return e2;
    }
    if (e2 == synFalse) {
        res =  makeNot(e1);
        return res;
    }
    if (e1 == synFalse) {
        res =  makeNot(e2);
        return res;
    }
    return new synCompound(synEqual, e1, e2);
}

synExp *makeXOR(std::vector<synExp*> v) {
    int count = 0;
    std::vector<synExp*>::iterator itv = v.begin();
    while (itv != v.end()) {
        if (*itv == synTrue)
            count++;
        if (*itv == synFalse)
            itv = v.erase(itv);
        else
            itv++;
    }
    if (v.size() == 0)
        return synTrue;
    
    if (v.size() == 1)
        return v[0];
    
    if (count > 1)
        return synFalse;
    
    return new synXOR(v);
}
std::list<synExp *> expandImplication(synExp* e) {
    std::list<synExp*> res;
    std::vector<synExp *> stack;
    synExp* left;
    synExp* right;
    stack.push_back(e);
    while (! stack.empty()) {
        synExp *s = stack.back();
        stack.pop_back();
        left  = s->first();
        right = s->second();
        // s is of the form a -> b and c we write a -> b, a -> c
        if (s->isImplies() &&  right->isAnd()) {
            stack.push_back(makeImplies(s->first(), right->first()));
            stack.push_back(makeImplies(s->first(), right->second()));
        }
        else
            if (s->isAnd()) {
                stack.push_back(s->first());
                stack.push_back(s->second());
            }
        
        else
            res.push_back(s);
    }
    return res;
}

void printVector(std::list<synExp *>& l) {
    for(auto e : l)
        std::cerr << e << std::endl;
}
std::list<synExp *> toCNF_rec(synExp* e) {
    std::list<synExp*> res;
    // If expression is a variable, we leave as it is
    // Also for XOR. It doesn't make sense to use XOR with CNF but
    // at least some conversion is made
    if (e->get_type() == synExp_Symbol || e->get_type() == synExp_XOR) {
        res.push_back(e);
        return res;
    }
    // If expression is of the form P and Q, we turn to CNF P and Q
    // toCNF(P) = P1 ^ P2 ^ ... ^ Pm
    // toCNF(Q) = Q1 ^ Q2 ^ ... ^ Qn
    // Then, toCNF(P and Q) = P1 ^ P2 ^ ... ^ Pm ^ Q1 ^ Q2 ^ ... ^ Qn
    if (e->isAnd()) {
        std::list<synExp*> resP, resQ;
        resP = toCNF_rec(e->first());
        resQ = toCNF_rec(e->second());

        res.insert(res.end(), resP.begin(), resP.end());
        res.insert(res.end(), resQ.begin(), resQ.end());
        delete e;
        return res;
    }
    // If expression is of the form P v Q,
    // toCNF(P) = P1 ^ P2 ^ ... ^ Pm
    // toCNF(Q) = Q1 ^ Q2 ^ ... ^ Qn
    // Then, toCNF(P v Q) is (P1 v Q1) ^ (P1 v Q2) ^ ... ^ (P1 v Qn)
    //                     ^ (P2 v Q1) ^ (P2 v Q2) ^ ... ^ (P2 v Qn)
    //                      ...
    //                     ^ (Pm v Q1) ^ (P2 v Q2) ^ ... ^ (Pm v Qn)
    if (e->isOr()) {
        std::list<synExp*> resP, resQ;
        resP = toCNF_rec(e->first());
        resQ = toCNF_rec(e->second());

        for(synExp* a : resP) {
            //std::cerr << "a " << a;
            for(synExp* b : resQ) {
                //std::cerr << " b " << b << std::endl;
                synExp *temp = makeOr(a->copy_exp(),b->copy_exp());
                if (temp != synTrue)
                    res.push_back(temp);
            }
        }
        for(synExp* a : resP)
            a->destroy();
        for(synExp* b : resQ)
            b->destroy();

        delete e;
        return res;
    }
    // If expression is of the form not p,
    if (e->isNot()) {
        synExp *negated = e->first();
        // if p is a variable, return e
        if (negated->isSymbol()) {
            res.push_back(e);
            return res;
        }
        // if p is not Q, then return Q
        if (negated->isNot()) {
            res.push_back(negated->first());
            delete e;
            delete negated;
            return res;
        }
        // if p is (P ^ Q) then return toCNF(not P v not Q)
        if (negated->isAnd()) {
            std::list<synExp*> t = toCNF_rec(makeOr(makeNot(negated->first()),
                                                    makeNot(negated->second())));
            res.insert(res.end(), t.begin(), t.end());
            delete negated;
            delete e;
            return res;
        }
        // if p is (P v Q) then return toCNF(not P ^ not Q)
        if (negated->isOr()) {
            std::list<synExp*> t = toCNF_rec(makeNot(negated->first()));
            res.insert(res.end(), t.begin(), t.end());
            t.clear();
            t = toCNF_rec(makeNot(negated->second()));
            res.insert(res.end(), t.begin(), t.end());
            delete negated;
            delete e;
            return res;
        }
        // if p is If A then B else C,
         // not (A -> B ^ not A -> C) ... not (A -> B) or not (not A -> C) ...
         // not (not A v B) or not (A v C) ... (A ^ not B) or (not A ^ not C) ...
         // toCNF(not B or not A) ^ (A or not C) ^(not B or not C)
        if (negated->isIf()) {
            std::list<synExp*> t = toCNF_rec(makeOr(makeNot(negated->first()->copy_exp()),
                                                     makeNot(negated->second()->copy_exp())));
            res.insert(res.end(), t.begin(), t.end());
            t = toCNF_rec(makeOr(negated->first()->copy_exp(), makeNot(negated->third()->copy_exp())));
            res.insert(res.end(), t.begin(), t.end());
            t = toCNF_rec(makeOr(makeNot(negated->second()->copy_exp()),
                                 makeNot(negated->third()->copy_exp())));
            res.insert(res.end(), t.begin(), t.end());
            
            negated->destroy();
            delete e;
            return res;
        }
        if (negated->isXOR()) {
            res.push_back(e);
            return res;
        }
        std::ostringstream ost;
        ost << "Unhandled case for CNF in " << e << std::endl;
        throw std::logic_error(ost.str());
    }
    // If expression is of the from P -> Q, then return toCNF(not P v Q)
    if (e->isImplies()) {
        std::list<synExp*> t = toCNF_rec(makeOr(makeNot(e->first()), e->second()));
        res.insert(res.end(), t.begin(), t.end());
        delete e;
        return res;
    }
    // There is still the "IF THEN ELSE" construction. If A then B else C,
    // return toCNF(not A v B) concat toCNF(A v C)
    if (e->isIf()) {
        std::list<synExp*> t = toCNF_rec(makeOr(makeNot(e->first()->copy_exp()),
                                                e->second()));
        res.insert(res.end(), t.begin(), t.end());
        if (e->third() != NULL) {
            std::list<synExp*> w = toCNF_rec(makeOr(e->first()->copy_exp(), e->third()));
            res.insert(res.end(), w.begin(), w.end());
        }
        e->first()->destroy();
        delete e;
        return res;
    }
    // Now A equals B,
    // return (not A v B ) concat (A v not B)
    if (e->isEqual()) {
        std::list<synExp*> t = toCNF_rec(makeOr(makeNot(e->first()->copy_exp()),
                                                e->second()->copy_exp()));
        res.insert(res.end(), t.begin(), t.end());
        t = toCNF_rec(makeOr(e->first()->copy_exp(), makeNot(e->second()->copy_exp())));
        res.insert(res.end(), t.begin(), t.end());
        
        e->destroy();
        return res;
    }
    if (e == synTrue)
        return res;
    
    if (e == synFalse || e == synModule) {
        res.push_back(e);
        return res;
    }
    // And that's it, we don't use more conectives
    std::ostringstream ost;
    ost << "Unhandled case in toCNF : " << e << std::endl;
    throw std::logic_error(ost.str());
    //std::cerr << "Unhandled case in toCNF : " << e << std::endl;
    //return res;
    //exit(-1);
}

std::list<synExp *> toCNF(synExp* e) {
    std::list<synExp *> result, theList = toCNF_rec(e);
    // Post processing of the "clauses"
    // There can be literals, but also XOR, synFalse and synTrue
    for(synExp* s : theList) {
        std::set<std::string> positive, negative;
        std::list<synExp*> xorList;
        std::string var;
        std::vector<synExp *> stack, litlist;
        // If the clause is false, the result of everything is false
        if (s == synFalse) {
            result.clear();
            result.push_back(synFalse);
            return result;
        }
        if (s == synModule)
            result.push_back(synModule);
        else
            if (s != synTrue)
                stack.push_back(s);
        
        // Lets flatten the "clause"
        while (!stack.empty()) {
            synExp* trav = stack.back();
            stack.pop_back();
            
            if (trav->isOr()) {
                stack.push_back(trav->first());
                stack.push_back(trav->second());
            }
            else
                if (trav->isLiteral())
                    litlist.push_back(trav);
                else if (trav->isXOR()) {
                    litlist.push_back(trav);
                }
                else
                    if (trav == synTrue) {
                        stack.clear();
                        litlist.clear();
                        break;
                    }
            // Can be synModule
                else if (trav != synFalse) {
                    std::ostringstream ost;
                    ost << "Neither or nor literal nor XOR nor synTrue nor synFalse: " << trav << std::endl;
                    throw std::logic_error(ost.str());
                }
        }
        bool positiveAndNegative  = false;
        
        for(synExp * literal : litlist) {
            bool isPositive           = false;
            bool isNegative           = false;
            if (literal->isXOR())
                xorList.push_back(literal);
            else {
                if (literal->isSymbol()) {
                    var        = literal->getSymbol();
                    isPositive = true;
                }
                else {
                    if (literal->isNot())
                        var        = literal->first()->getSymbol();
                    isNegative = true;
                }
            }
            
            if (isPositive) {
                if (positive.find(var) == positive.end())
                    positive.insert(var);
                
                if (negative.find(var) != negative.end()) {
                    positiveAndNegative = true;
                }
            }
            else {
                if (isNegative) {
                    if (negative.find(var) == negative.end())
                        negative.insert(var);
                    
                    if (positive.find(var) != positive.end())
                        positiveAndNegative = true;
                }
            }
        }
        // If it is positive and negative in a clause, the clause is true
        // and there is no need to include it
        if (!positiveAndNegative) {
            synExp* myRes = synFalse;
            for(auto x : xorList)
                myRes = makeOr(myRes, x->copy_exp());
            for(std::string var  : positive) {
                myRes = makeOr(myRes, new synSymbol(var));
            }
            for(std::string var  : negative) {
                myRes = makeOr(myRes, makeNot(new synSymbol(var)));
                
            }
            result.push_back(myRes);
        }
        
    }
    return result;
}

std::vector<int> synExp::toDimacs(std::map<std::string, int>& theMap) {
    std::vector<int> res, res1, res2;
    // If the expression is a conjunction, we process each part separately
    if (isOr()) {
        res1 = first()->toDimacs(theMap);
        res2 = second()->toDimacs(theMap);
        res.insert(res.end(), res1.begin(), res1.end());
        res.insert(res.end(), res2.begin(), res2.end());
        return res;
    }
    // If the expression is a negated variable, it is negative
    // If expression is of the form not p,
    if (isNot()) {
        synExp *negated = first();
        // if p is a variable, return e
        if (negated->isSymbol()) {
            res1 = negated->toDimacs(theMap);
            res.push_back(-res1.front());
            return res;
        }
    }
    if (this == synFalse)
        return res;
    
    std::cerr << "Unhandled case converting clause " << this << " to DIMACS format" << std::endl;
    exit(-1);

}

std::vector<int> synSymbol::toDimacs(std::map<std::string, int>& theMap) {
    std::vector<int> res;
    // If the expression is a variable, it is positive
    // Variables in DIMACS have to start at 1, because
    // 0 is used to end the clause
    // Also -0 wouldn't make a lot sense, now would it?
        res.push_back(1+theMap.at(getSymbol()));
        return res;
}


synExp* synConst::copy_exp() {
    return this;
}

synExp* synSymbol::copy_exp() {
    return new synSymbol(name);
}

synExp* synUnknown::copy_exp() {
    return new synUnknown(st);
}

synExp* synCompound::copy_exp() {
    return new synCompound(Cop, Ce1->copy_exp(), Ce2->copy_exp(), Ce3->copy_exp());
}

synExp* synString::copy_exp() {
    return new synString(st);
}

synExp* synXOR::copy_exp() {
    std::vector<synExp*> copies;
    for(synExp* s : get_parms())
        copies.push_back(s->copy_exp());
    return new synXOR(copies);
}


void synConst::destroy() {
    return;
}

void synSymbol::destroy() {
        delete this;
}

void synUnknown::destroy() {
        delete this;
}

void synCompound::destroy() {
    Ce1->destroy();
    Ce2->destroy();
    Ce3->destroy();
    delete this;
}

void synString::destroy() {
    delete this;
}

void synXOR::destroy() {
    for(synExp* parm : v) {
        parm->destroy();
    }
    
    delete this;
}
