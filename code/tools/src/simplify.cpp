#include "simplify.hpp"


std::map<std::string, std::set<int>>      reverseIndex;
int numChanges;

void    addSymbols2Table(std::vector<std::string>& vars) {
    for(std::string s : vars) {
        VariableTable::addVariable(s, s);
    }
}

int simplifySymbol(const std::string& sn, synExp* value) {
    int numchanges = 0;
     synExp* wValue = VariableTable::getValue(sn);
     std::cerr << "Value of " << sn << " in table is " << wValue << " but simplifySymbol is " << value << std::endl; 
    if (wValue != value) {
        // If we have A and later not A or vice versa we may
        // flip A from true to false forever. So we check and
        // later we'll get an unsatisfiable set
        if (wValue != synTrue && wValue != synFalse) {
            //std::cerr << "We set var " << std::setw(70) << sn << " to " << value << std::endl;
            numchanges++;
            VariableTable::setValue(sn, value);
        }
        else if (wValue == makeNot(value)) {
            std::cerr << "Var " << std::setw(70) << sn << " is true and false " << std::endl;
            exit(-1);
        }
    }
    return numchanges;
}

void makeReverseIndex(std::vector<synExp*>& pC) {
    // For each var we make a list of the related expressions
    for(int vec = 0; vec < pC.size(); vec++)
        for(auto& var : pC[vec]->giveSymbols()) {
            reverseIndex[var].insert(vec);
        }
}

void simplifyExpression(int numExp, synExp* e, std::unordered_set<int>& nextSimplify) {
    std::string symbol;
    synExp* value = NULL;
    if (e->isLiteral()) {
        if (e->isSymbol()) {
            // Constraint is a variable
            symbol = e->getSymbol();
            value  = synTrue;
        }
        else {
            // Constraint is "not Variable"
            if (e->isNot() && e->first()->isSymbol()) {
                symbol = e->first()->getSymbol();
                value  = synFalse;
            }
        }
        numChanges += simplifySymbol(symbol, value);
        reverseIndex[symbol].erase(numExp);
        for(auto exp : reverseIndex[symbol])
            nextSimplify.insert(exp);
    }
    // If the constraint corresponds to an undeclared config we delete it
    return;
}
std::vector<synExp*> simplifyConstraints(std::vector<synExp*>& pendingConstraints) {
    std::unordered_set<int> toSimplify, nextSimplify;
    numChanges = 0;
    makeReverseIndex(pendingConstraints);
    std::cerr << "Simplify Constraints" << std::endl;
    for(int i = 0; i < pendingConstraints.size(); i++) {
        //std::cerr << "Going with i " << i << " " << pendingConstraints[i] << std::endl;
        // This only puts values to synTrue or synFalse in symbol table
        simplifyExpression(i, pendingConstraints[i], toSimplify);
    }
    do {
        numChanges = 0;
        for(int numExp : toSimplify) {
            synExp* e = pendingConstraints[numExp];
            synExp* before = e->copy_exp();
            if (!e->isLiteral()) {
                //std::cerr << "Before simplifying (" << numExp <<  ") " << e << std::endl;
                e = eval(e);
                pendingConstraints[numExp] = e;
                //std::cerr << "After  simplifying (" << numExp << " ) " << e << std::endl;
                if (e == synFalse) {
                    std::ostringstream ost;
                    ost << "Expression simplified to false: " << before << std::endl;
                    ost << "First  simplifies to " << eval(before->first())  << std::endl;
                    ost << "Second simplifies to " << eval(before->second()) << std::endl;
                    ost << "Third  simplifies to " << eval(before->third())  << std::endl;

                     throw std::logic_error(ost.str());
                }
                before->destroy();
                if (e->isLiteral()) {
                    simplifyExpression(numExp, e, nextSimplify);
                }
            }
      }
        std::cerr << "Numchanges " << std::setw(7) << numChanges << std::endl;
        toSimplify = nextSimplify;
        nextSimplify.clear();
   }
    while (numChanges > 0);

    // We don't delete constraint here, only in Kconfig2Logic
    std::vector<synExp*> remainingConstraints;
    std::string symbol;
    for(auto exp : pendingConstraints) {
        //std::cerr << "In pendingContraints " << exp << std::endl;
        if (exp->isComment()) {
            symbol = exp->getComment();
            remainingConstraints.push_back(exp);
            continue;
        }

        if (exp == synFalse) {
            std::ostringstream ost;
            ost << "synFalse found in simplifyConstraints" << std::endl;
            throw std::logic_error(ost.str());
        }
        if (exp == synTrue)
            continue;

        remainingConstraints.push_back(exp);

    }

    return remainingConstraints;
}

