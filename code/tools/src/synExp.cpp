//
//  synExp.cpp
//  myKconf
//
//  Created by david on 04/12/13.
//  Copyright (c) 2013 david. All rights reserved.
//

#include "synExp.hpp"

#ifdef _DEBUG_
//std::unordered_map<synExp*, std::string> synExp::store;
#endif
vint synExp::ocSoFar;
vint synExp::weights;
String2Int synExp::string2int;
std::unordered_map<int, std::string> synExp::int2string;

synExp::~synExp() {
    //std::cerr << "In synExp destructor " << std::endl;
    deleteSymbols();
    myIndices.clear();
#ifdef _DEBUG_
    //store.erase(this);
#endif
};


void synExp::flattenAnd(synExp* exp, std::unordered_set<synExp*>& ss) {
    // Destroys exp
    if (exp->isAnd()) {
        // We do it second then first to improve the unrolling
        // of nested if-then-elses with exclusive guards
        flattenAnd(exp->second()->copy_exp(), ss);
        flattenAnd(exp->first()->copy_exp(), ss);
        exp->destroy();
    }
    else {
        bool found = false;
        for(auto x : ss)
            if (equal(x, exp)) {
                found = true;
                exp->destroy();
                break;
            }
        if (!found) {
            // Revise later
            //ss.insert(exp->copy_exp());
            //exp->destroy();
            ss.insert(exp);
        }
    }
}

void synExp::flattenOr(synExp* exp, std::unordered_set<synExp*>& ss) {
    if (exp->isOr()) {
        flattenOr(exp->first()->copy_exp(), ss);
        flattenOr(exp->second()->copy_exp(), ss);
        exp->destroy();
    }
    else {
        bool found = false;
        for(auto x : ss)
            if (equal(x, exp)) {
                found = true;
                exp->destroy();
                break;
            }
        if (!found) {
            ss.insert(exp);
        }
    }
}


synExpT                         synExp::get_type()          const {
    std::ostringstream ost;
    ost << "Error accessing get_type in wrong expression: " << this << std::endl;
    throw std::logic_error(ost.str());
    return synExp_Const;
}
const std::string                     synExp::get_string()        const {
    std::ostringstream ost;
    ost << "Error accessing get_string in wrong expression: " << (synExp*) this << std::endl;
    throw std::logic_error(ost.str());
    return "";
    
}
std::string                     synExp::getSymbol()         const {
    std::ostringstream ost;
    std::cerr << "Error accessing getSymbol in wrong object: ";
    this->print(std::cerr); std::cerr << std::endl;
    throw std::logic_error("Error accessing getSymbol in wrong object.\n");
    int *p = NULL;
    std::cerr << *p << std::endl;
    exit(-1);
    return "";
}
std::string                 synExp::getComment()       const {
         throw std::logic_error("Error accessing getComment in wrong object.\n");
          return "";
     }

std::string                 synExp::getMacro()       const {
         throw std::logic_error("Error accessing getMacro in wrong object.\n");
          return "";
     }

synOp                           synExp::get_op()            const {
         std::ostringstream ost;
         ost << "Error accessing getOp in wrong object:" << this << std::endl;
         throw std::logic_error(ost.str());
     }
synExp*                         synExp::first()             const {
         return NULL;
     }
synExp*                         synExp::second()            const {
         return NULL;
     }
synExp*                         synExp::third()             const {
         return NULL;
     }
vSynExp           synExp::get_parms()          const  {
         throw (std::logic_error("Error accesing get_parms in wrong object"));
     }

const SetSynOp&          synExp::giveOps()           const   {
         std::cerr << "Error accesing giveOps in wrong object: " << this << std::endl;
         int *p = NULL;
         std::cerr << *p << std::endl;
         exit(-1);
     }
bool    synExp::isCompound() const        { return false; }
bool    synExp::isOr()       const        { return false; }
bool    synExp::isAnd()      const        { return false; }
bool    synExp::isNot()      const        { return false; }
bool    synExp::isImplies()  const        { return false; }
bool    synExp::isIf()       const        { return false; }
bool    synExp::isEqual()    const        { return false; }
bool    synExp::isSymbol()   const        { return false; }
bool    synExp::isLiteral()  const        { return false; }
bool    synExp::isXOR()      const        { return false; }
bool    synExp::isConst()    const        { return false; }
bool    synExp::isString()   const        { return false; }
bool    synExp::isUnknown()  const        { return false; }
bool    synExp::isComment()  const        { return false; }
bool    synExp::isMacro()    const        { return false; }

bool    synExp::isDisjunctionOfLiterals() const {
    if (isLiteral())
        return true;
    if (!isOr())
        return false;
    bool FDoL = first()->isDisjunctionOfLiterals();
    bool SDol = second()->isDisjunctionOfLiterals();
    if ((first()->isLiteral()   && SDol) ||
        (second()->isLiteral()  && FDoL) ||
        (FDoL && SDol))
        return true;
    
    return false;
}

// Does the formula contain the mod constant?
bool    synExp::hasConstantModule() const { return false; }

synExp* synExp::copy_exp() {
         throw std::logic_error("Calling copy_exp in base class synExp\n");
}
 
void    synExp::destroy() {
         throw std::logic_error("Calling destroy in base class synExp\n");
}

void synExp::deleteSymbols() {
        if (mySymbols != NULL) {
            mySymbols->clear();
            delete mySymbols;
            mySymbols = NULL;
        }
    }

void synExp::setSymbols(sstring*ss) { mySymbols = ss; };
const sstring&   synExp::giveSymbols()       {
    if (mySymbols == NULL) {
        mySymbols = new sstring();
        computeSymbols(mySymbols);
    }
    return *mySymbols;
}

void synExp::computeSymbols(sstring *ss) {};
const std::unordered_set<int>&    synExp::giveSymbolIndices() const { return myIndices;    }
int                synExp::opPriority()        const  { return 0;  }
void  synExp::computeIndices(String2Int& m) {
    myIndices.clear();
    for(const std::string & s : giveSymbols()) {
        try {
            myIndices.insert(m.at(s));
        }
        catch(std::exception e) {
            std::ostringstream ost;
            ost << "Error looking for key *" << s << "* in varMap. Expression " << this << std::endl;
            ost << "varMap size " << m.size();
            throw std::logic_error(ost.str());
        }
    }
}
int synExp::giveMax(const vint& var2pos) const {
     int pos;
     int max = -1;
     for(int x : giveSymbolIndices()) {
         if ((pos = var2pos[x]) > max) {
             max = pos;
         }
     }
     return max;
 }
 int synExp::giveMaxInd(const vint& q) const {
     int pos, ind = -1;
     int max = -1;
     for(int x : giveSymbolIndices()) {
         if ((pos = q[x]) > max) {
             max = pos;
             ind = x;
         }
     }
     return ind;
 }

 int synExp::giveMinInd(const vint& q) const {
     int pos, ind = -1;
     int min = std::numeric_limits<int>::max();
     for(int x : giveSymbolIndices()) {
         if ((pos = q[x]) < min) {
             min = pos;
             ind = x;
         }
     }
     return ind;
 }

 int synExp::giveMin(const vint& var2pos) const {
     int pos;
     int min = std::numeric_limits<int>::max();
     for(int x : giveSymbolIndices()) {
         if ((pos = var2pos[x]) < min) {
                 min = pos;
         }
     }
     return min;
 }
void  synExp::setSmartSpan(int n){ smartspan = n;}
int   synExp::getSmartSpan() const    { return smartspan; }
int   synExp::getMax()   const   { return max;   }
int   synExp::getMin()   const   { return min;   }
int   synExp::getLCA()   const   { return lca;   }
float synExp::getScore() const   { return score; }
void  synExp::setScore(double x) { score = x;    }

void synExp::computeMaxMin(const vint& var2pos) {
    if (isComment()) {
        min = 0;
        max = 0;
        return;
    }
    max = giveMax(var2pos);
    min = giveMin(var2pos);
    if (min > max) {
        std::ostringstream ost;
        ost << "Min is bigger than max in " << this << std::endl;
        throw std::logic_error(ost.str());
    }
}
    void setScore(double x);
    
int synExp::computeSpan(const vint& var2pos) const {
    int smin = std::numeric_limits<int>::max();
    int smax = -1;
    for(int i : myIndices) {
        if (var2pos[i] < smin) smin = var2pos[i];
        if (var2pos[i] > smax) smax = var2pos[i];
    }
    return smax - smin;
}
   

void synExp::setLCA(int x) { lca = x; }

void synExp::thisOneIsProcessed() {
    for(int index : myIndices)
        weights[index]++;
  
    return;
}

void synExp::numVars(int s) {
    ocSoFar.resize(s);
    weights.resize(s);
}

#ifdef _DEBUG_
//void synExp::showRemaining() {
    //store.erase(synTrue);
    //store.erase(synModule);
    //store.erase(synFalse);
    //std::cerr << "Remaining expressions " << store.size() << std::endl;
    //for(auto& p : store) {
    //    std::cerr << (void*) p.first;
    //    std::cerr << " " << p.first << std::endl;
    //    std::cerr << p.second << std::endl;
    //}
    //store.clear();
//}
#endif
std::ostream& operator<<(std::ostream& os, synExp* ps) {
    if (ps == NULL) {
        os << "NULL";
        return os;
    }
    ps->print(os);
    return os;
}


void synExp::print(std::ostream& os) const {
    std::ostringstream ost;
    ost << "Error accessing print in base class synExp: "
            << this << std::endl;
    throw std::logic_error(ost.str());
    return;
}

synExp* synExp::substitute(SubMap& map) {
    std::ostringstream ost;
    ost << "Error accessing substitute in base class synExp: "
            << this << std::endl;
    throw std::logic_error(ost.str());
    return synFalse;
}
synConst::synConst() : synExp() {};
synConst*       synConst::getSynFalse()  {
      if (!syncFalse)  syncFalse  = new synConst(synConst_false);  return syncFalse;
      }
synConst*       synConst::getSynModule() {
          if (!syncModule) syncModule = new synConst(synConst_module); return syncModule;
      }
 synConst*       synConst::getSynTrue()   {
      if (!syncTrue)   syncTrue   = new synConst(synConst_true);   return syncTrue;
      }
  
  bool    synConst::isConst()  const  { return true; }
  synOp                           synConst::get_op()            const {
      std::ostringstream ost;
      ost << "Error accessing getOp in synConst:" << this << std::endl;
      throw std::logic_error(ost.str());
  }
  bool    synConst::hasConstantModule() const { return this == synModule; }

void synConst::delConst() {
      delete synTrue;
      delete synFalse;
      delete synModule;
  }
  synExpT                         synConst::get_type()              const { return synExp_Const; }
  synConstsT                      synConst::get_const()             const { return theConst;     }

  const SetSynOp&          synConst::giveOps()               const { return myOps;        }
  int                             synConst::opPriority()            const { return 1;            }

void synConst::print(std::ostream& os) const {
    switch (get_const()) {
            case synConst_true   : os << "synTrue";   break;
            case synConst_false  : os << "synFalse";  break;
            case synConst_module : os << "synModule"; break;
        default: os << "Error, const is " << get_const() << std::endl;
    }
}

synExp* synConst::substitute(SubMap& map) {
    return this;
}
synSymbol::synSymbol(int num) : synExp(), mapNum(num) {};
synSymbol::synSymbol(const std::string& s) : synExp() {
          if (string2int.find(s) == string2int.end()) {
              mapNum = string2int.size();
              string2int[s] = mapNum;
              int2string[mapNum] = s;
          }
          else {
              mapNum = string2int[s];
          }
       }

synExpT         synSymbol::get_type()          const   { return synExp_Symbol;           }
std::string     synSymbol::getSymbol()         const   { return int2string[mapNum];      }
const SetSynOp& synSymbol::giveOps()           const   { return myOps;                   }
int             synSymbol::opPriority()        const   { return 1;                       }
bool            synSymbol::isLiteral()         const   { return true;                    }
bool            synSymbol::isSymbol()          const   { return true;                    }
void            synSymbol::computeSymbols(sstring* ss) {
        ss->insert(int2string[mapNum]);
    };
synOp     synSymbol::get_op()            const {
        std::ostringstream ost;
        ost << "Error accessing getOp in synSymbol:" << this << ", "
                    << (synExp*) this << std::endl;
        throw std::logic_error(ost.str());
}

synExp* synSymbol::substitute(SubMap& map) {
    SubMap::iterator itMap;
    itMap = map.find(int2string[mapNum]);
    if (itMap != map.end()) {
        this->destroy();
        return itMap->second->copy_exp();
    }
    return this;
}
synSymbol::~synSymbol() {
        myOps.clear();
};

void synSymbol::print(std::ostream& os) const {
    //os  << std::setw(len) << getSymbol();
    os  << getSymbol();
}


synCompound::synCompound(synOp op, synExp *e1, synExp *e2, synExp *e3) : synExp(), Cop(op), Ce1(e1), Ce2(e2), Ce3(e3)   {
    myOps.insert(op);
    if (e1->get_type() == synExp_Compound) {
        myOps.insert(e1->giveOps().begin(), e1->giveOps().end());
    }
    if (e2->get_type() == synExp_Compound) {
        myOps.insert(e2->giveOps().begin(), e2->giveOps().end());
    }
    if (e3->get_type() == synExp_Compound) {
        myOps.insert(e3->giveOps().begin(), e3->giveOps().end());
    }
    if (e1->isXOR() || e2->isXOR() || e3->isXOR()) {
        myOps.insert(synXor);
    }
#ifdef _DEBUG_
    //store.erase(e1);
    //store.erase(e2);
    //store.erase(e3);
#endif
}


int     synCompound::opPriority() const  {
       switch(Cop) {
           case synNot        : return  3;
           case synXor        : return  3;
           case synAnd        : return  4;
           case synOr         : return  4;
           case synImplies    : return  7;
           case synIfThenElse : return  8;
           case synEqual      : return  2;
           }
       return 0;
   }

synExpT          synCompound::get_type()         const   { return synExp_Compound;   }
synOp            synCompound::get_op()           const   { return Cop;               }
synExp*          synCompound::first()            const   { return Ce1;               }
synExp*          synCompound::second()           const   { return Ce2;               }
synExp*          synCompound::third()            const   { return Ce3;               }
const SetSynOp& synCompound::giveOps()           const   { return myOps;             }
void             synCompound::setFirst(synExp* f)        { Ce1 = f;                  }
void             synCompound::setSecond(synExp* s)       { Ce2 = s;                  }
void             synCompound::setThird(synExp* t)        { Ce3 = t;                  }
   
bool        synCompound::isCompound() const            { return true;                 }
bool        synCompound::isOr()       const            { return Cop == synOr         ;}
bool        synCompound::isAnd()      const            { return Cop == synAnd        ;}
bool        synCompound::isNot()      const            { return Cop == synNot        ;}
bool        synCompound::isImplies()  const            { return Cop == synImplies    ;}
bool        synCompound::isIf()       const            { return Cop == synIfThenElse ;}
bool        synCompound::isEqual()    const            { return Cop == synEqual      ;}
bool        synCompound::isLiteral()  const            { return Cop == synNot &&
                                                            Ce1->get_type() == synExp_Symbol; }
void synCompound::computeSymbols(sstring* ss) {
    Ce1->computeSymbols(ss);
    Ce2->computeSymbols(ss);
    Ce3->computeSymbols(ss);
};
bool  synCompound::hasConstantModule() const {
    return  Ce1->hasConstantModule() ||
            Ce2->hasConstantModule() ||
            Ce3->hasConstantModule();
}
synCompound::~synCompound() {
    myOps.clear();
};
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
        (Ce2->get_type() == synExp_Compound && Ce2->get_op() == Cop && Cop != synEqual)))  {

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
            os  << stFirst << " or " << stSecond; break;
        }
        case synNot : {
            os << "not " << stFirst; break;
        }
        case synImplies: {
            os << stFirst << " -> " << stSecond; break;
        }
        case synIfThenElse : {
            int len  = int(os.width());
            int len1 = int(stFirst.length());
            int len2 = int(stSecond.length());
            if (Ce3 == synTrue || Ce3 == NULL)
                os << std::setw(3) << "if " << stFirst << " then "
                   <<  std::setw(len - len1 - 9) << stSecond;
            else
                os << std::setw(3) << "if " << stFirst << " then " << stSecond
                   << " else " << std::setw(len - len1 - len2 - 16) << stThird;
            break;
        }
        case synEqual : {
            os << stFirst << " = " << stSecond; break;
        }
        default : os << "Error, operator is " << get_op() << std::endl;
    }
}

synExp* synCompound::substitute(SubMap& map) {
    synExp* result  = NULL;
    synExp* cFirst  = NULL;
    synExp* cSecond = NULL;
    synExp* cThird  = NULL;
    if (Ce1 != NULL)
        cFirst = Ce1->copy_exp()->substitute(map);
    if (Ce2 != NULL)
        cSecond = Ce2->copy_exp()->substitute(map);
    if (Ce3 != NULL)
        cThird = Ce3->copy_exp()->substitute(map);
    switch (Cop) {
        case synNot : { result = makeNot(cFirst);
                        break; }
        case synAnd : { result = makeAnd(cFirst,
                                         cSecond);
                        break; }
        case synOr  : { result = makeOr(cFirst,
                                        cSecond);
                        break;}
        case synImplies : { result = makeImplies(cFirst,
                                                 cSecond);
                        break; }
        case synIfThenElse : {
            result = makeIfThenElse(cFirst,
                                    cSecond,
                                    cThird);
            break;
        }
        case synEqual : { result = makeEqual(cFirst,
                                             cSecond);
                            break;}
        case synXor   : {
            throw std::logic_error("synXor used in a synExp instead of class synXor");
        }
    }
    this->destroy();
    return result;
}

synString::synString(const std::string& s) : synExp(), st(s)     {};
bool                synString::isString()          const { return true; }
synExpT             synString::get_type()          const { return synExp_String;   }
const std::string   synString::get_string()        const { return st;              }
synExp*             synString::eval()                    { return this;            }
int                 synString::opPriority()        const { return 1;               }
const SetSynOp&     synString::giveOps()           const { return myOps;           }

synOp               synString::get_op()            const {
    std::ostringstream ost;
    ost << "Error accessing getOp in synString:" << st << std::endl;
    throw std::logic_error(ost.str());
}

synExp* synString::substitute(SubMap& map) {
    return this;
}
synString::~synString() { myOps.clear(); };

synExp* synUnknown::substitute(SubMap& map) {
    return this;
}
void synUnknown::print(std::ostream& os) const {
    os  << "<UNKNOWN> " << get_string();
}

           synXOR::synXOR(vSynExp v) : v(v)    { myOps.insert(synXor); }
synExpT    synXOR::get_type()          const   { return synExp_XOR;    }
vSynExp    synXOR::get_parms()         const   { return v;             }
void       synXOR::set_parms(vSynExp& w)       { v = w;                }
void       synXOR::clear_parms()               { v.clear();            }
int        synXOR::opPriority()        const   { return 4;             }
bool       synXOR::isXOR()             const   { return true;          }
synExp*    synXOR::substitute(SubMap& map) {
    vSynExp newVector;
    for(auto exp : v)
        newVector.push_back(exp->substitute(map));
    this->destroy();
    return new synXOR(newVector);
}

const SetSynOp&      synXOR::giveOps()           const   { return myOps;            }
void  synXOR::computeSymbols(sstring* ss) {
       for(synExp* s : v)
           s->computeSymbols(ss);
   }
   
synOp                 synXOR::get_op()            const {
       std::ostringstream ost;
       ost << "Error accessing getOp in XOR:" << this << std::endl;
       throw std::logic_error(ost.str());
}
   
bool    synXOR::hasConstantModule() const {
       bool result = false;
        for(auto s : v)
            result |= s->hasConstantModule();
       return result;
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

void synComment::print(std::ostream& os) const {
    os << "# " << comment;
}

void synMacro::print(std::ostream& os) const {
    os << "<macro> " << macro;
}

synUnknown::synUnknown(const std::string& s) : synExp(), st(s)     {};
synExpT           synUnknown::get_type()          const { return synExp_Unknown;  }
const std::string       synUnknown::get_string()        const { return st;              }
synExp*           synUnknown::eval()                    { return this;            }
int               synUnknown::opPriority()        const { return 1;               }
const SetSynOp&   synUnknown::giveOps()           const { return myOps;           }
bool              synUnknown::isUnknown()         const { return true;            }
synOp             synUnknown::get_op()            const {
    std::ostringstream ost;
    ost << "Error accessing getOp in synUnknown:" << st << std::endl;
    throw std::logic_error(ost.str());
}
void synUnknown::computeSymbols(sstring *ss) { ss->insert(st); };
synExp*          synUnknown::first()             const {
    std::ostringstream ost;
    ost << "Error accessing first in synUnknown:" << st << std::endl;
    throw std::logic_error(ost.str());
}

synUnknown::~synUnknown() {
    myOps.clear();
};

synComment::synComment(const std::string& s) : synExp(), comment(s)     {};
synExpT            synComment::get_type()      const { return synExp_Comment; }
std::string        synComment::getComment()    const { return comment;        }
bool               synComment::isComment()     const { return true;           }
int                synComment::opPriority()    const { return 1;              }
const SetSynOp&    synComment::giveOps()       const { return myOps;          }
synOp              synComment::get_op()        const {
    std::ostringstream ost;
    ost << "Error accessing getOp in synComment:" << comment << std::endl;
    throw std::logic_error(ost.str());
}

synExp* synComment::substitute(SubMap& map) {
    SubMap::iterator itm = map.find(getComment());
    if (itm != map.end() && itm->second->isSymbol())
        return new synComment(itm->second->getSymbol());
    return this;
}
synComment::~synComment() {
         myOps.clear();
};

synMacro::synMacro(const std::string& s) : synExp(), macro(s)     {};
synExpT            synMacro::get_type()      const { return synExp_Macro; }
std::string        synMacro::getMacro()    const { return macro;        }
bool               synMacro::isMacro()     const { return true;           }
int                synMacro::opPriority()    const { return 1;              }
const SetSynOp&    synMacro::giveOps()       const { return myOps;          }
synOp              synMacro::get_op()        const {
    std::ostringstream ost;
    ost << "Error accessing getOp in synMacro:" <<  macro << std::endl;
    throw std::logic_error(ost.str());
}

synExp* synMacro::substitute(SubMap& map) {
    return this;
}
synMacro::~synMacro() {
         myOps.clear();
};

bool equal(synExp *e1, synExp *e2) {
    if ((e1 == NULL     && e2 == NULL) ||
        (e1 == synFalse && e2 == NULL) ||
        (e1 == NULL     && e2 == synFalse))
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
            // We need this to work at least when everything is the same
            // for eval to do its work
            vSynExp one, two;
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
        case synExp_Comment : { throw std::logic_error("Calling equal on synComment"); }
        case synExp_Macro   : {
            return (e1->getMacro() == e2->getMacro());
        }
    }
    return false;
}


synConst *synConst::syncTrue, *synConst::syncFalse, *synConst::syncModule;

synExp *synTrue   = synConst::getSynTrue();
synExp *synModule = synConst::getSynModule();
synExp *synFalse  = synConst::getSynFalse();

synExp *makeAnd(std::vector<synExp*>& s) {
    synExp* res = synTrue;
    for(auto x : s) {
        res = makeAnd(res, x);
    }
    return res;
}

synExp *makeAnd(synExp* e1, synExp* e2) {
    if (e1 == synTrue)  return e2;
    if (e2 == synTrue)  return e1;
    if (e1 == synFalse) {
        e2->destroy();
        return synFalse;
    }
    if (e2 == synFalse) {
        e1->destroy();
        return synFalse;
    }
    
    if (e1->isMacro() || e2->isMacro())
        return new synCompound(synAnd, e1, e2);
        
    if (equal(e1, e2))  {
        e2->destroy();
        return e1;
    }
;
    synExp* temp = makeNot(e2->copy_exp());
    if (equal(e1, temp)) {
        e1->destroy();
        e2->destroy();
        temp->destroy();
        return synFalse;
    }
    else
        temp->destroy();
//    bool conjLit1 = (e1->giveOps().size() == 1 &&
//                     e1->giveOps().find(synAnd) != e1->giveOps().end()) ||
//                    (e1->giveOps().size() == 2 &&
//                     e1->giveOps().find(synAnd) != e1->giveOps().end() &&
//                     e1->giveOps().find(synNot) != e1->giveOps().end());
//    bool conjLit2 = (e2->giveOps().size() == 1 &&
//                     e2->giveOps().find(synAnd) != e1->giveOps().end()) ||
//                    (e2->giveOps().size() == 2 &&
//                     e2->giveOps().find(synAnd) != e2->giveOps().end() &&
//                     e2->giveOps().find(synNot) != e2->giveOps().end());

    if (e1->giveOps().size() == 1 &&
        e1->giveOps().find(synAnd) != e1->giveOps().end() &&
        e2->giveOps().size() == 1 &&
        e2->giveOps().find(synAnd) != e2->giveOps().end()) { 
        synExp* res = NULL;
        std::unordered_set<std::string> soFar;
        // We are losing "m" because it is not a symbol
        for(auto& ss : e1->giveSymbols())
            if (soFar.find(ss) == soFar.end()) {
                if (res == NULL)
                    res = new synSymbol(ss);
                else
                    res = new synCompound(synAnd, res, new synSymbol(ss));
                
                soFar.insert(ss);
            }
        for(auto& ss : e2->giveSymbols())
            if (soFar.find(ss) == soFar.end()) {
                if (res == NULL)
                    res = new synSymbol(ss);
                else
                    res = new synCompound(synAnd, res, new synSymbol(ss));
                soFar.insert(ss);
            }
        if (e1->hasConstantModule() || e2->hasConstantModule()) {
            if (res == NULL)
                res = synModule;
            else
                res = new synCompound(synAnd, res, synModule);
        }
        e1->destroy();
        e2->destroy();
        return res;
    }
    return new synCompound(synAnd, e1, e2);
}
synExp *makeOr(std::vector<synExp*>& s) {
    synExp* res = synFalse;
    for(auto x : s)
        res = makeOr(res, x);
    return res;
}
synExp *makeOr(synExp* e1, synExp* e2, bool assumeBool) {
    if (e1 == synFalse) return e2;
    if (e2 == synFalse) return e1;
    if (e1 == synTrue) {
        e2->destroy();
        return synTrue;
    }
    if (e2 == synTrue) {
        e1->destroy();
        return synTrue;
    }
    if (equal(e1, e2)) {
        e2->destroy();
        return e1;
    }
    if (e1->isMacro() || e2->isMacro())
        return new synCompound(synOr, e1, e2);
    synExp *temp =  makeNot(e2->copy_exp());
    if (equal(e1, temp) && assumeBool) {
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
        synExp* res = NULL;
        std::unordered_set<std::string> soFar;
        for(auto& ss : e1->giveSymbols())
            if (soFar.find(ss) == soFar.end()) {
                if (res == NULL)
                    res = new synSymbol(ss);
                else
                    res = new synCompound(synOr, res, new synSymbol(ss));
                soFar.insert(ss);
            }
        for(auto& ss : e2->giveSymbols()) {
            if (soFar.find(ss) == soFar.end()) {
                if (res == NULL)
                     res = new synSymbol(ss);
                else
                    res = new synCompound(synOr, res, new synSymbol(ss));
                soFar.insert(ss);
            }
        }
        if (e1->hasConstantModule() || e2->hasConstantModule()) {
            if (res == NULL)
                res = synModule;
            else
                res = new synCompound(synOr, res, synModule);
        }
        e1->destroy();
        e2->destroy();
        return res;
    }
    // A or (A and B) is simplifiable to A
    if (e2->isAnd() && (equal(e1, e2->first()) || equal(e1, e2->second()))) {
        e2->destroy();
        return e1;
    }
    if (e1->isAnd() && (equal(e2, e1->first()) || equal(e2, e1->second()))) {
        e1->destroy();
        return e2;
    }
    return new synCompound(synOr, e1, e2);
}

synExp *makeImplies(synExp* e1, synExp* e2) {
    synExp *res = NULL;
    if (e1 == synFalse)  {
        e2->destroy();
        return synTrue;
    }
    if (e1 == synTrue)   return e2;
    // Not true in trivalued logic
    //if (e1 == synModule) return e2;
    if (e2 == synFalse)  return makeNot(e1);
    if (e2 == synTrue)  {
        e1->destroy();
        return synTrue;
    }
    if (e1->isNot() && e2->isNot()) {
        res =  makeImplies(e2->first()->copy_exp(), e1->first()->copy_exp());
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
    // De Morgan laws... too slow, sorry
//    if (e1->isOr()) {
//        synExp* temp = makeAnd(makeNot(e1->first()->copy_exp()),
//                               makeNot(e1->second()->copy_exp())
//                               );
//        e1->destroy();
//        return temp;
//    }
//
//    if (e1->isAnd()) {
//        synExp* temp = makeOr(makeNot(e1->first()->copy_exp()),
//                              makeNot(e1->second()->copy_exp()));
//        e1->destroy();
//        return temp;
//    }
    
    
    return new synCompound(synNot, e1);
}

synExp *makeIfThenElse(synExp* e1, synExp* e2, synExp* e3 = synTrue, bool assumeBool) {
    //std::cerr << "synExp::makeIfThenElse, a " << e1 << ", b " << e2 << ", " << e3 << std::endl;
    synExp *res = NULL;
    if (e1 == synTrue)   {
        e3->destroy();
        return e2;
    }
    if (e1 == synModule) {
        e3->destroy();
        return makeImplies(synModule, e2);
    }
    if (e1 == synFalse)  {
        e2->destroy();
        return e3;
    }
    if (e2 == synTrue  && e3 == synFalse && assumeBool) {
        return e1;
    }
    if (e2 == synFalse && e3 == synTrue && assumeBool)  {
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
// This breaks unrollIf because it puts the defined
// in the definition (the conditions of the ifs)
// We will do this in eval
  
// Not anymore, so back again
    
    if (e2 == synTrue && assumeBool ) {
        res =  makeIfThenElse(makeNot(e1),e3);
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
        res = makeIfThenElse(e2->first()->copy_exp(),
                             e1->first()->copy_exp(),
                             synTrue);
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
    
//temp = makeNot(e2->copy_exp());
//    if (equal(e1, temp) && e3 == synTrue) {
//        e1->destroy();
//        e2->destroy();
//        e3->destroy();
//        return temp;
//    }
//    temp->destroy();
    
    // Vital to deal with tristate variables
    if (e2->isIf() && equal(e2->first(), e1)) {
        synExp* res = new synCompound(synIfThenElse,
                                      e1,
                                      e2->second()->copy_exp(),
                                      e3);
        e2->destroy();
        return res;
    }
    return new synCompound(synIfThenElse, e1, e2, e3);
}

synExp *makeEqual(synExp *e1, synExp *e2, bool assumeBool) {
    synExp *res = NULL;
    if (e1->isConst() && e2->isConst()) {
        if (e1 == e2)
            return synTrue;
        else
            return synFalse;
    }
    
    if (e2 == synTrue && assumeBool)
           return e1;
            
    if (e1 == synTrue && assumeBool)
           return e2;
    
    if (e2 == synFalse && assumeBool) {
        res =  makeNot(e1);
        return res;
    }
    if (e1 == synFalse && assumeBool) {
        res =  makeNot(e2);
        return res;
    }
    
    // If comparing equal(x and y, synFalse) = equal(x, synFalse) or equal(y, synFalse)
    if (e1->isAnd() && e2 == synFalse) {
        synExp* temp = makeOr(makeEqual(e1->first()->copy_exp(), synFalse),
                              makeEqual(e1->second()->copy_exp(), synFalse), assumeBool);
  
        e1->destroy();
        e2->destroy();
        return temp;
    }
    if (e2->isAnd() && e1 == synFalse) {
        synExp* temp = makeOr(makeEqual(e2->first()->copy_exp(), synFalse),
                              makeEqual(e2->second()->copy_exp(), synFalse,
                                        assumeBool));
        e1->destroy();
        e2->destroy();
        return temp;
    }
    // If comparing equal(x or y, synFalse) = equal(x, synFalse) and equal(y, synFalse)
    if (e1->isOr() && e2 == synFalse) {
        synExp* temp = makeAnd(makeEqual(e1->first()->copy_exp(), synFalse),
                               makeEqual(e1->second()->copy_exp(), synFalse));
        e1->destroy();
        e2->destroy();
        return temp;
    }
    
    if (e2->isOr() && e1 == synFalse) {
        synExp* temp = makeAnd(makeEqual(e2->first()->copy_exp(), synFalse),
                               makeEqual(e2->second()->copy_exp(), synFalse));
        e1->destroy();
        e2->destroy();
        return temp;
    }
    
    if (e1->isNot() && e2->isConst()) {
        synExp* temp = makeEqual(e1->first()->copy_exp(), makeNot(e2));
        e1->destroy();
        return temp;
    }
    if (e2->isNot() && e1->isConst()) {
        synExp* temp = makeEqual(e2->first()->copy_exp(), makeNot(e1));
        e2->destroy();
        return temp;
    }
    return new synCompound(synEqual, e1, e2);
}

synExp *makeXOR(vSynExp& v) {
    int count = 0;
    vSynExp::iterator itv = v.begin();
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
    
    if (v.size() == 1) {
        synExp *res = v[0];
        v.clear();
        return res;
    }
    
    if (count > 1) {
        for(auto& w : v) w->destroy();
        v.clear();
        return synFalse;
    }
    
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
            stack.push_back(makeImplies(left->copy_exp(), right->first()->copy_exp()));
            stack.push_back(makeImplies(left->copy_exp(), right->second()->copy_exp()));
        }
        else
        // s is of the from a v b -> c we write a -> c and b -> c
            if (s->isImplies() && left->isOr()) {
                stack.push_back(makeImplies(left->first()->copy_exp(), right->copy_exp()));
                stack.push_back(makeImplies(left->first()->copy_exp(), right->copy_exp()));
            }
        else
            if (s->isAnd()) {
                stack.push_back(s->first()->copy_exp());
                stack.push_back(s->second()->copy_exp());
            }
        else
            res.push_back(s->copy_exp());
    }
    return res;
}

void printVector(std::list<synExp *>& l) {
    for(auto e : l)
        std::cerr << e << std::endl;
}

void synExp::printExpresion() {
    std::cerr << this << std::endl;
}

std::list<synExp *> toCNF_rec(synExp* e) {
    //std::cerr << "CNF_rec of " << e << std::endl;
    std::list<synExp*> res;
    // If expression is a variable, we leave as it is
    // Also for XOR. It doesn't make sense to use XOR with CNF but
    // at least some conversion is made
    if (e->isSymbol() || e->isXOR() || e->isComment()) {
        res.push_back(e);
        return res;
    }
    // If expression is of the form P and Q, we turn to CNF P and Q
    // toCNF(P) = P1 ^ P2 ^ ... ^ Pm
    // toCNF(Q) = Q1 ^ Q2 ^ ... ^ Qn
    // Then, toCNF(P and Q) = P1 ^ P2 ^ ... ^ Pm ^ Q1 ^ Q2 ^ ... ^ Qn
    if (e->isAnd()) {
        std::list<synExp*> resP, resQ;
        resP = toCNF_rec(e->first()->copy_exp());
        resQ = toCNF_rec(e->second()->copy_exp());

        res.insert(res.end(), resP.begin(), resP.end());
        res.insert(res.end(), resQ.begin(), resQ.end());
        e->destroy();
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
        resP = toCNF_rec(e->first()->copy_exp());
        resQ = toCNF_rec(e->second()->copy_exp());

        for(synExp* a : resP) {
            //std::cerr << "a " << a;
            for(synExp* b : resQ) {
                //std::cerr << " b " << b << std::endl;
                synExp *temp = makeOr(a->copy_exp(),b->copy_exp(), true);
                if (temp != synTrue)
                    res.push_back(temp);
            }
        }
        for(synExp* a : resP)
            a->destroy();
        for(synExp* b : resQ)
            b->destroy();
        
        e->destroy();
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
            res.push_back(negated->first()->copy_exp());
            e->destroy();
            return res;
        }
        // if p is (P ^ Q) then return toCNF(not P v not Q)
        if (negated->isAnd()) {
            std::list<synExp*> t = toCNF_rec(makeOr(makeNot(negated->first()->copy_exp()),
                                                    makeNot(negated->second()->copy_exp()), true));
            res.insert(res.end(), t.begin(), t.end());

            e->destroy();;
            return res;
        }
        // if p is (P v Q) then return toCNF(not P ^ not Q)
        if (negated->isOr()) {
            std::list<synExp*> t = toCNF_rec(makeNot(negated->first()->copy_exp()));
            res.insert(res.end(), t.begin(), t.end());
            t.clear();
            t = toCNF_rec(makeNot(negated->second()->copy_exp()));
            res.insert(res.end(), t.begin(), t.end());
            e->destroy();
            return res;
        }
        // if p is If A then B else C,
         // not (A -> B ^ not A -> C) ... not (A -> B) or not (not A -> C) ...
         // not (not A v B) or not (A v C) ... (A ^ not B) or (not A ^ not C) ...
         // toCNF(not B or not A) ^ (A or not C) ^(not B or not C)
        if (negated->isIf()) {
            std::list<synExp*> t = toCNF_rec(makeOr(makeNot(negated->first()->copy_exp()),
                                                     makeNot(negated->second()->copy_exp()), true));
            res.insert(res.end(), t.begin(), t.end());
            t = toCNF_rec(makeOr(negated->first()->copy_exp(), makeNot(negated->third()->copy_exp()), true));
            res.insert(res.end(), t.begin(), t.end());
            t = toCNF_rec(makeOr(makeNot(negated->second()->copy_exp()),
                                 makeNot(negated->third()->copy_exp()), true));
            res.insert(res.end(), t.begin(), t.end());
            
            e->destroy();
            return res;
        }
        if (negated->isXOR()) {
            res.push_back(e);
            return res;
        }
        // not (A -> B) = not (not A v B) = A ^ not B, so CNF(A), CNF(not B)
        if (negated->isImplies()) {
            std::list<synExp*> t = toCNF_rec(negated->first()->copy_exp());
            res.insert(res.end(), t.begin(), t.end());
            t = toCNF_rec(makeNot(negated->second()->copy_exp()));
            res.insert(res.end(), t.begin(), t.end());
            return res;
        }
        // not (A = B) = (A ^ !B) v (!A ^ B) = (A v B) ^ (!B v !A)
        if (negated->isEqual()) {
            std::list<synExp*> t = toCNF_rec(makeOr(negated->first()->copy_exp(),
                                                    negated->second()->copy_exp(), true));
            res.insert(res.end(), t.begin(), t.end());
            t = toCNF_rec(makeOr(makeNot(negated->first()->copy_exp()),
                                 makeNot(negated->second()->copy_exp()), true));
            res.insert(res.end(), t.begin(), t.end());
            return res;
        }
        std::ostringstream ost;
        ost << "Unhandled case for CNF in " << e << std::endl;
        throw std::logic_error(ost.str());
    }
    // If expression is of the from P -> Q, then return toCNF(not P v Q)
    if (e->isImplies()) {
        std::list<synExp*> t = toCNF_rec(makeOr(makeNot(e->first()->copy_exp()), e->second()->copy_exp(), true));
        res.insert(res.end(), t.begin(), t.end());
        e->destroy();
        return res;
    }
    // There is still the "IF THEN ELSE" construction. If A then B else C,
    // return toCNF(not A v B) concat toCNF(A v C)
    if (e->isIf()) {
        std::list<synExp*> t = toCNF_rec(makeOr(makeNot(e->first()->copy_exp()),
                                                e->second()->copy_exp(), true));
        res.insert(res.end(), t.begin(), t.end());
        if (e->third() != NULL) {
            std::list<synExp*> w = toCNF_rec(makeOr(e->first()->copy_exp(), e->third()->copy_exp(), true));
            res.insert(res.end(), w.begin(), w.end());
        }
        e->destroy();
        return res;
    }
    // Now A equals B,
    // return (not A v B ) concat (A v not B)
    if (e->isEqual()) {
        std::list<synExp*> t = toCNF_rec(makeOr(makeNot(e->first()->copy_exp()),
                                                e->second()->copy_exp(), true));
        res.insert(res.end(), t.begin(), t.end());
        t = toCNF_rec(makeOr(e->first()->copy_exp(), makeNot(e->second()->copy_exp()), true));
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
        //std::cerr << "post processing " << s << std::endl;
        std::unordered_set<std::string> positive, negative;
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
            if (s->isComment()) {
                result.push_back(s);
                return result;
            }
            else
                if (s != synTrue)
                stack.push_back(s);
        
        // Lets flatten the "clause"
        while (!stack.empty()) {
            synExp* trav = stack.back();
            stack.pop_back();
            
            if (trav->isOr()) {
                stack.push_back(trav->first()->copy_exp());
                stack.push_back(trav->second()->copy_exp());
                trav->destroy();
            }
            else
                if (trav->isLiteral() || trav->isXOR())
                    litlist.push_back(trav);
                else
                    if (trav == synTrue) {
                        stack.clear();
                        litlist.clear();
                        break;
                    }
            // Can be synModule
                else if (trav != synFalse) {
                    std::ostringstream ost;
                    ost << "Neither or nor literal nor XOR nor Comment nor synTrue nor synFalse: " << trav << std::endl;
                    throw std::logic_error(ost.str());
                }
        }
        bool positiveAndNegative  = false;
        
        for(auto literal : litlist) {
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
                    if (literal->isNot()) {
                        var        = literal->first()->getSymbol();
                        isNegative = true;
                    }
                    else {
                        std::ostringstream ost;
                        ost << literal << " is not positive and not negative" << std::endl;
                        throw std::logic_error(ost.str());
                    }
                }
                literal->destroy();
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
                myRes = makeOr(myRes, x, true);
            for(std::string var  : positive) {
                myRes = makeOr(myRes, new synSymbol(var), true);
            }
            for(std::string var  : negative) {
                myRes = makeOr(myRes, makeNot(new synSymbol(var)), true);
                
            }
            result.push_back(myRes);
        }
        
    }
    return result;
}

vint synExp::toDimacs(String2Int& theMap) {
    vint res, res1, res2;
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
    
    std::ostringstream ost;
    ost << "Unhandled case converting clause " << this
            << " to DIMACS format" << std::endl;
    throw std::logic_error(ost.str());

}
vint synComment::toDimacs(String2Int& theMap) {
    vint res;
    return res;
}
vint synMacro::toDimacs(String2Int& theMap) {
    vint res;
    return res;
}
vint synSymbol::toDimacs(String2Int& theMap) {
    vint res;
    // If the expression is a variable, it is positive
    // Variables in DIMACS have to start at 1, because
    // 0 is used to end the clause
    // Also -0 wouldn't make a lot sense, now would it?
    try {
        res.push_back(1+theMap.at(getSymbol()));
    }
    catch (std::out_of_range e) {
        std::cerr << "Key " << getSymbol() << " not found in map" << std::endl;
        exit(-1);
    }
    return res;
}


synExp* synConst::copy_exp() {
    return this;
}

synExp* synSymbol::copy_exp() {
    return new synSymbol(mapNum);
}

synExp* synUnknown::copy_exp() {
    return new synUnknown(st);
}

synExp* synComment::copy_exp() {
    return new synComment(comment);
}

synExp* synMacro::copy_exp() {
    return new synMacro(macro);
}

synExp* synCompound::copy_exp() {
    return new synCompound(Cop,
                           Ce1->copy_exp(),
                           Ce2->copy_exp(),
                           Ce3->copy_exp());
}

synExp* synString::copy_exp() {
    return new synString(st);
}

synExp* synXOR::copy_exp() {
    vSynExp copies;
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

void synComment::destroy() {
        delete this;
}
void synMacro::destroy() {
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

bool operator<(const synExp& a,  const synExp& b) {
    if (a.get_type() != b.get_type())
        return a.get_type() < b.get_type();
    
    switch (a.get_type()) {
        case synExp_Const : {
            synConst& c = (synConst&)a;
            synConst& d = (synConst&)b;
            return c.get_const() < d.get_const();
        }
        case synExp_Symbol : return a.getSymbol() < b.getSymbol();
        case synExp_Compound : {
            synCompound& c = (synCompound&) a;
            synCompound& d = (synCompound&) b;
            if (a.get_op() != b.get_op())
                return c.get_op() < d.get_op();
            if (!equal(c.first(), d.first()))
                return operator<(*c.first(), *d.first());
            if (!equal(c.second(), d.second()))
                return operator<(*c.second(), *d.second());
            if (!equal(c.third(), d.third()))
                return operator<(*c.third(), *d.third());
            return false;
        }
        case synExp_String  : return a.get_string() < b.get_string();
        case synExp_Unknown : return a.get_string() < b.get_string();
        case synExp_Comment : return a.getComment() < b.getComment();
        case synExp_Macro   : return a.getMacro()   < b.getMacro();
        case synExp_XOR     : {
            if (a.get_parms().size() != b.get_parms().size())
                return a.get_parms().size() < b.get_parms().size();
            for(int i = 0; i < a.get_parms().size(); i++)
                if (!equal(a.get_parms()[i], b.get_parms()[i]))
                    return operator<(*a.get_parms()[i], *b.get_parms()[i]);
            return false;
        }
    }
    return false;
};

std::string getArt(int i) {
    std::ostringstream ost;
    ost << "ART_" << i;
    return ost.str();
}
void addArtificialVariables(synExp* e, int& varnum, std::unordered_map<synExp*, std::string>& map) {
    if (e == NULL || e == synFalse)
        return;
    if (e->isSymbol()) {
        map[e] = e->getSymbol();
        return;
    }
    else {
        std::cerr << getArt(varnum) << " stands for " << e << std::endl;
        map[e] = getArt(varnum++);
    }
    
    addArtificialVariables(e->first(),  varnum, map);
    addArtificialVariables(e->second(), varnum, map);
    addArtificialVariables(e->third(),  varnum, map);
}

void TseitinExp_rec(synExp* e,
                    std::unordered_map<synExp*, std::string>& map,
                    std::list<synExp*>& res) {
    if (e->isComment())
        return;
    if (e->isConst())
        return;
    if (e->isSymbol())
        return;
    if (!e->isCompound()) {
        std::ostringstream ost;
        ost << "Don't know what to do with " << e << " in Tseitin_rec" << std::endl;
        throw std::logic_error(ost.str());
    }
    // Let's do this
    if (e->isNot()) {
        TseitinExp_rec(e->first(), map, res);
        std::string& a = map.at(e);
        std::string& b = map.at(e->first());
        // ¬a v ¬b
        res.push_back(makeOr(makeNot(new synSymbol(a)),
                             makeNot(new synSymbol(b)),
                             true));
        // a v b
        res.push_back(makeOr(new synSymbol(a),
                             new synSymbol(b),
                             true));
        return;
    }
    if (e->isAnd()) {
        TseitinExp_rec(e->first(),  map, res);
        TseitinExp_rec(e->second(), map, res);

        std::string& a = map.at(e);
        std::string& b = map.at(e->first());
        std::string& c = map.at(e->second());
        // ¬a v b
        res.push_back(makeOr(makeNot(new synSymbol(a)),
                             new synSymbol(b),
                             true));
        // ¬a v c
        res.push_back(makeOr(makeNot(new synSymbol(a)),
                             new synSymbol(c),
                             true));
        // a v ¬b v ¬c
        res.push_back(makeOr(new synSymbol(a),
                             makeOr(makeNot(new synSymbol(b)),
                                    makeNot(new synSymbol(c)), true),
                             true));
        return;
    }
    if (e->isOr()) {
        TseitinExp_rec(e->first(),  map, res);
        TseitinExp_rec(e->second(), map, res);
        
        std::string& a = map.at(e);
        std::string& b = map.at(e->first());
        std::string& c = map.at(e->second());
        // a v ¬b
        res.push_back(makeOr(new synSymbol(a),
                             makeNot(new synSymbol(b)),
                             true));
        // a v ¬c
        res.push_back(makeOr(new synSymbol(a),
                             makeNot(new synSymbol(c)),
                             true));
        // ¬a v b v c
        res.push_back(makeOr(makeNot(new synSymbol(a)),
                             makeOr(new synSymbol(b),
                                    new synSymbol(c), true),
                             true));
        return;
    }
    if (e->isImplies()) {
        TseitinExp_rec(e->first(),  map, res);
        TseitinExp_rec(e->second(), map, res);
        
        std::string& a = map.at(e);
        std::string& b = map.at(e->first());
        std::string& c = map.at(e->second());

        // ¬a v ¬b v c
        res.push_back(makeOr(makeNot(new synSymbol(a)),
                             makeOr(makeNot(new synSymbol(b)),
                                    new synSymbol(c), true),
                             true));
        // (a v ¬c)
        res.push_back(makeOr(new synSymbol(a),
                             makeNot(new synSymbol(c)),
                             true));
        // (a v b)
        res.push_back(makeOr(new synSymbol(a),
                             new synSymbol(b),
                             true));
        return;
    }
    if (e->isIf()) {
        TseitinExp_rec(e->first(),  map, res);
        TseitinExp_rec(e->second(), map, res);
        TseitinExp_rec(e->third(),  map, res);

        std::string& a = map.at(e);
        std::string& b = map.at(e->first());
        std::string& c = map.at(e->second());
        std::string& d = map.at(e->third());
        
        // A v ¬B ∨ ¬C
        res.push_back(makeOr(new synSymbol(a),
                             makeOr(makeNot(new synSymbol(b)),
                                    makeNot(new synSymbol(c)), true),
                             true));
        //  A v B v ¬D
        res.push_back(makeOr(new synSymbol(a),
                             makeOr(new synSymbol(b),
                                    makeNot(new synSymbol(d)), true),
                             true));
        
        // (A v ¬C v ¬D)
        res.push_back(makeOr(new synSymbol(a),
                             makeOr(makeNot(new synSymbol(c)),
                                    makeNot(new synSymbol(d)), true),
                             true));
        // (¬A v ¬B v C)
        res.push_back(makeOr(makeNot(new synSymbol(a)),
                             makeOr(makeNot(new synSymbol(b)),
                                    new synSymbol(c), true),
                             true));
        // (¬A v B v D)
        res.push_back(makeOr(makeNot(new synSymbol(a)),
                             makeOr(new synSymbol(b),
                                    new synSymbol(d), true),
                             true));


        return;
    }

    if (e->isEqual()) {
        TseitinExp_rec(e->first(),  map, res);
        TseitinExp_rec(e->second(), map, res);

        std::string& a = map.at(e);
        std::string& b = map.at(e->first());
        std::string& c = map.at(e->second());
        // A v B v C
        res.push_back(makeOr(new synSymbol(a),
                             makeOr(new synSymbol(b),
                                    new synSymbol(c), true),
                             true));
        
        // A v ¬B v ¬C
        res.push_back(makeOr(new synSymbol(a),
                             makeOr(makeNot(new synSymbol(b)),
                                    makeNot(new synSymbol(c)),
                                    true),
                             true));
        // ¬A v ¬B v C
        res.push_back(makeOr(makeNot(new synSymbol(a)),
                             makeOr(makeNot(new synSymbol(b)),
                                   new synSymbol(c), true),
                             true));
        // ¬A v B v ¬C
        res.push_back(makeOr(makeNot(new synSymbol(a)),
                             makeOr(new synSymbol(b),
                                   makeNot(new synSymbol(c)), true),
                             true));

        return;
    }
    std::ostringstream ost;
    ost << "Not implemented to Tseitin of " << e << std::endl;
    throw std::logic_error(ost.str());
}

std::list<synExp*> synExp::Tseitin(int& varnum) {
    std::list<synExp*> res;
    if (isComment())
        return res;

    if (isConst()   ||
        isLiteral() ||
        isDisjunctionOfLiterals()) {
        res.push_back(this);
        return res;
    }
    std::unordered_map<synExp*, std::string> map;
    res.push_back(new synSymbol(getArt(varnum)));
    
    addArtificialVariables(this, varnum, map);
//    std::cerr << "Artificial variables:" << std::endl;
//    for(auto p : map)
//        std::cerr << p.first << " " << p.second << std::endl;
//    std::cerr << std::endl;
    
    TseitinExp_rec(this, map, res);
    return res;
}

 

