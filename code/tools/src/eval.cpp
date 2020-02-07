//
//  eval.cpp
//  myKconf
//
//  Created by david on 15/10/14.
//  Copyright (c) 2014 david. All rights reserved.
//

#include "eval.hpp"

void addTable(synExp* exp, std::map<synExp*, synExp*>& tab) {
    //std::cerr << "addTable " << exp << " pointer " << (void*) exp << std::endl;
    tab[exp] = synTrue;
    //std::cerr << " map " << exp << " = true " << std::endl;
    if (exp->isNot()) {
        //std::cerr << " map " << exp->first() << " = false " << std::endl;

        tab[exp->first()] = synFalse;
    }
    if (exp->isAnd()) {
        addTable(exp->first() , tab);
        addTable(exp->second(), tab);
    }
}



synExp* eval_recur(synExp* exp, std::map<synExp*, synExp*> tab) {
    //std::cerr << "map size " << tab.size() << std::endl;
    for(std::pair<synExp*, synExp*> p : tab) {
        //std::cerr << "exp " << (void*) exp << " p first " << (void*) p.first << " p second " << (void*) p.second << std::endl;
        //std::cerr << "first " << p.first << " second " << p.second << std::endl;
        //std::cerr << "Checking " << exp << " against " << p.first << std::endl;
        if (equal(exp, p.first))
            return p.second;
    }
    
    synExp *res = synTrue;
    if (exp == NULL) return NULL;
    switch (exp->get_type()) {
        case synExp_Const :  { return exp; }
        case synExp_Symbol : {
            std::string theSymbol = exp->getSymbol();
            configInfo*    var       = symbolTable::getSymbol(theSymbol);
            if (var == NULL || var->getValue() == NULL) {
                // This variable is not declared and is therefore false
                // The name of the symbol
                return exp;
            }
            else {
                // The value of the symbol
                res = var->getValue()->copy_exp();
                exp->destroy();
                return res;
            }
        }
        case synExp_Compound :
        {
            switch (exp->get_op()) {
                case synNot        : {
                    res =  makeNot(eval_recur(exp->first(), tab));
                    delete exp;
                    return res;
                }
                case synAnd        : {
                    synExp *one   = eval_recur(exp->first(), tab);
                    synExp *two   = eval_recur(exp->second(), tab);
                    synExp *three = makeAnd(one, two);
                    delete exp;

                    return three;
                }
                case synOr         : {
                    synExp *one   = eval_recur(exp->first(), tab);
                    synExp *two   = eval_recur(exp->second(), tab);
                    synExp *three = makeOr(one, two);
                    delete exp;

                    return three; }
                case synImplies    : {

                    res =  makeImplies(eval_recur(exp->first(), tab),
                                        eval_recur(exp->second(), tab));
                    delete exp;

                    return res;
                }
                case synIfThenElse : {
                    std::map<synExp*, synExp*> tab2, tab3;
                    tab2 = tab;
                    tab3 = tab;
                    
                    addTable(exp->first()->copy_exp(), tab2);
                    addTable(makeNot(exp->first()->copy_exp()), tab3);
                    
                    res =  makeIfThenElse(eval_recur(exp->first() , tab),
                                          eval_recur(exp->second(), tab2),
                                          eval_recur(exp->third(), tab3));
                    delete exp;

                    return res;
                }
                case synEqual      : {
                    res = makeEqual(eval_recur(exp->first(), tab),
                                    eval_recur(exp->second(), tab));
                    delete exp;
                    return res;
                }
                case synXor: {
                    throw std::logic_error("An XOR cannot happpen here");
                    break;
                }
            }
        }
        case synExp_String : { return exp; }
            
        case synExp_XOR : {
            //std::cerr << "Evaluating expression " << exp << std::endl;
            std::vector<synExp*> evaluated, newParms;
            for(synExp* s :exp->get_parms())
                evaluated.push_back(eval_recur(s->copy_exp(),tab));
            int counter = 0;
            for(synExp* ev : evaluated) {
                //std::cerr << "Evaluated " << ev << std::endl;
                if (ev == synTrue) counter++;
                if (ev != synFalse)
                    newParms.push_back(ev);
            }
            if (counter > 1) {
                exp->destroy();
                for(synExp* d : evaluated) d->destroy();
                return synFalse;
            }
            if (counter == 1) {
                res = synTrue;
                for(synExp* ev : evaluated) {
                    if (ev != synTrue)
                        res = makeAnd(res, makeNot(ev));
                }
                return res;
            }
            if (newParms.size() == 1) {
                //std::cerr << "newParms[0] = " << newParms[0] << std::endl;
                res = newParms[0]->copy_exp();
                exp->destroy();
                for(synExp* d : evaluated) d->destroy();
                return res;
            }
            if (evaluated.size() == 0) {
                exp->destroy();
                return synTrue;
            }
            delete exp;
            return(new synXOR(newParms));
        }
        case synExp_Unknown : break;
    }
    return NULL;
}

synExp *eval(synExp* exp) {
    std::map<synExp*, synExp*> tab;
    synExp *res = eval_recur(exp->copy_exp(), tab);
    // Repeat while anything changes
    while (!equal(res, exp)) {
        delete exp;
        exp = res;
        res = eval_recur(exp->copy_exp(), tab);
    }
    return res;
}
