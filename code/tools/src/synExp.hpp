//
//  synExp.h
//  myKconf
//
//  Created by david on 04/12/13.
//  Copyright (c) 2013 david. All rights reserved.
//

#ifndef __myKconf__synExp__
#define __myKconf__synExp__

#include <iostream>
#include <algorithm>
#include <string>
#include <set>
#include <list>
#include <vector>
#include <sstream>
#include <iomanip>
#include <math.h>
#include "symbolTable.hpp"

enum synExpT    { synExp_Const,   synExp_Symbol,   synExp_Compound, synExp_XOR, synExp_String, synExp_Unknown };
enum synConstsT { synConst_false, synConst_module, synConst_true                                         };
enum synOp      { synNot, synAnd, synOr, synImplies, synIfThenElse, synEqual, synXor                             };

class synExp {
    
    public:
    
    synExp() { mySymbols = NULL; }
    
        //virtual ~synExp() { std::cerr << "In destructor " << this << std::endl;};
        virtual synExpT                         get_type()          const                   = 0;
        virtual std::string                     get_string()        const { return "";           }
        virtual std::string                     getSymbol()         const {
            throw std::logic_error("Error accessing getSymbol in wrong object.\n");
            int *p = NULL;
            std::cerr << *p << std::endl;
            exit(-1);
            return "";
        }
        virtual synOp                           get_op()            const {
            std::ostringstream ost;
            ost << "Error accessing getOp in wrong object:" << this << std::endl;
            throw std::logic_error(ost.str());
        }
        virtual synExp*                         first()             const {
            return NULL;
        }
        virtual synExp*                         second()            const {
            return NULL;
        }
        virtual synExp*                         third()             const {
            return NULL;
        }
        virtual std::vector<synExp*>           get_parms()          const  {
            throw (std::logic_error("Error accesing get_parms in wrong object"));
        }

        virtual const std::set<synOp>&          giveOps()           const   {
            std::cerr << "Error accesing giveOps in wrong object: " << this << std::endl;
            int *p = NULL;
            std::cerr << *p << std::endl;
            exit(-1);
        }
        virtual bool    isOr()       { return false; }
        virtual bool    isAnd()      { return false; }
        virtual bool    isNot()      { return false; }
        virtual bool    isImplies()  { return false; }
        virtual bool    isIf()       { return false; }
        virtual bool    isEqual()    { return false; }
        virtual bool    isSymbol()   { return false; }
        virtual bool    isLiteral()  { return false; }
        virtual bool    isXOR()      { return false; }
        virtual bool    isConst()    { return false; }
        virtual bool    isString()   { return false; }
        virtual bool    isUnknown()  { return false; }

    
        virtual synExp* copy_exp() {
            throw std::logic_error("Calling copy_exp in base class synExp\n");
        }
    
        virtual void    destroy() {
            throw std::logic_error("Calling destroy in base class synExp\n");
        }
        void deleteSymbols() {
            if (mySymbols != NULL) {
                mySymbols->clear();
                delete mySymbols;
                mySymbols = NULL;
            }
        }
        //void addModules() { mySymbols.insert("MODULES"); }
        void setSymbols(std::set<std::string>*ss) { mySymbols = ss; };
        const std::set<std::string>&            giveSymbols()       {
            if (mySymbols == NULL) {
                mySymbols = new std::set<std::string>();
                computeSymbols(mySymbols);
            }
            return *mySymbols;
        }
        virtual void computeSymbols(std::set<std::string> *ss) {};
        const std::set<int>&                    giveSymbolIndices() const { return myIndices;    }
        virtual int                             opPriority()        const  { return 0;  }
        void                                    computeIndices(std::map<std::string, int>& m) {
                                                    myIndices.clear();
                                                    for(const std::string & s : giveSymbols()) {
                                                        try {
                                                        myIndices.insert(m.at(s));
                                                        }
                                                        catch(std::exception e) {
                                                            std::ostringstream ost;
                                                            ost << "Error looking for key *" << s << "* in varMap. Expression " << this << std::endl;
                                                            throw std::logic_error(ost.str());
                                                        }
                                                    }
                                                }

        friend  std::ostream&          operator<<(std::ostream& os, synExp* ps);
        friend  bool                   equal(synExp *e1, synExp *e2);
    
    template <class Q>
        int giveMax(const Q& var2pos) const {
        int pos;
        int max = -1;
        for(int x : giveSymbolIndices()) {
            if ((pos = var2pos.at(x)) > max) {
                    max = pos;
                }
        }
        return max;
    }
    template <class Q>
    int giveMaxInd(const Q& q) const {
        int pos, ind = -1;
        int max = -1;
        for(int x : giveSymbolIndices()) {
            if ((pos = q.at(x)) > max) {
                max = pos;
                ind = x;
            }
        }
        return ind;
    }

    template <class Q>
    int giveMinInd(const Q& q) const {
        int pos, ind = -1;
        int min = std::numeric_limits<int>::max();
        for(int x : giveSymbolIndices()) {
            if ((pos = q.at(x)) < min) {
                min = pos;
                ind = x;
            }
        }
        return ind;
    }

    template <class Q>
    int giveMin(const Q& var2pos) const {
        int pos;
        int min = std::numeric_limits<int>::max();
        for(int x : giveSymbolIndices()) {
            if ((pos = var2pos.at(x)) < min) {
                    min = pos;
            }
        }
        return min;
    }
    
    int   getMax()   const { return max;   }
    int   getMin()   const { return min;   }
    int   getLCA()   const { return lca;   }
    float getScore() const { return score; }
    
    template <class Q>   void computeMaxMin(const Q& var2pos) {
        max = giveMax(var2pos);
        min = giveMin(var2pos);
        if (min > max) {
            std::ostringstream ost;
            ost << "Min is bigger than max in " << this << std::endl;
            throw std::logic_error(ost.str());
        }
    }
    
    template <class O>   void computeScore(const O& o) {
        score = 0;
        for(int x = min; x <= max; x++) 
            score += weights[x];
               return;
        //if (max != min)
        //    score = score / (max - min + 1);
        
        //score = log(score)/2.0 + (max - min)/2.0;
        //score += max - min;
    }

    void setScore(double x) { score = x;}
    
    template <class O>   int computeSpan(const O& var2pos) const {
        int smin = std::numeric_limits<int>::max();
        int smax = -1;
        for(int i : myIndices) {
            if (var2pos[i] < smin) smin = var2pos[i];
            if (var2pos[i] > smax) smax = var2pos[i];
        }
        return smax - smin;
    }
   
    

    void setLCA(int x) { lca = x; }
    
    void thisOneIsProcessed() {
        for(int index : myIndices)
            weights[index]++;
      
        return;
    }

    static void numVars(int s) {
        ocSoFar.resize(s);
        weights.resize(s);
    }
    virtual std::vector<int>    toDimacs(std::map<std::string, int>& theMap);
    virtual void print(std::ostream& os) const;

    virtual ~synExp() {
        deleteSymbols();
        myIndices.clear();
    };

protected:
    static std::vector<int> ocSoFar;
    static std::vector<int> weights;
    std::set<std::string> *mySymbols;
    std::set<int>         myIndices;
    int max, min, lca;
    float score;

};


extern synExp *synTrue;
extern synExp *synModule;
extern synExp *synFalse;
synExp *makeAnd(synExp* e1, synExp* e2);
synExp *makeOr(synExp* e1, synExp* e2);
synExp *makeNot(synExp* e1);
synExp *makeImplies(synExp* e1, synExp* e2);
synExp *makeIfThenElse(synExp* e1, synExp* e2, synExp* e3);
synExp *makeEqual(synExp* e1, synExp* e2);
synExp *makeXOR(std::vector<synExp*> v);
std::list<synExp *> expandImplication(synExp* e);
std::list<synExp *> toCNF(synExp* e);


class synConst : public synExp {
public:
    synConst() : synExp() {};
    static synConst*       getSynFalse()  {
        if (!syncFalse)  syncFalse  = new synConst(synConst_false);  return syncFalse;
        }
    static synConst*       getSynModule() {
            if (!syncModule) syncModule = new synConst(synConst_module); return syncModule;
        }
    static synConst*       getSynTrue()   {
        if (!syncTrue)   syncTrue   = new synConst(synConst_true);   return syncTrue;
        }
    
    bool    isConst()    { return true; }
    synOp                           get_op()            const {
        std::ostringstream ost;
        ost << "Error accessing getOp in synConst:" << this << std::endl;
        throw std::logic_error(ost.str());
    }
    
    static void delConst() {
        delete synTrue;
        delete synFalse;
        delete synModule;
    }
    synExpT                         get_type()              const { return synExp_Const; }
    synConstsT                      get_const()             const { return theConst;     }
  
    const std::set<synOp>&          giveOps()               const { return myOps;        }
    int                             opPriority()            const { return 1;            }
    synExp* copy_exp();

    void destroy();
 


private:
    

    std::set<synOp>       myOps;
    static synConst *syncTrue, *syncFalse, *syncModule;
    synConst(synConstsT oneConst) : theConst(oneConst) {};
    synConstsT theConst;
    void print(std::ostream& os) const;

};



class synSymbol : public synExp {
public:
    
                                    synSymbol(std::string s) : synExp() {
                                        name = s;
                                    }
    synExpT                         get_type()          const   { return synExp_Symbol;           }
    std::string                     getSymbol()         const   { return name;                    }
    const std::set<synOp>&          giveOps()           const   { return myOps;                   }
    int                             opPriority()        const   { return 1;                       }
    bool                            isLiteral()                 { return true;                    }
    bool                            isSymbol()                  { return true;                    }
    virtual std::vector<int>        toDimacs(std::map<std::string, int>& theMap);
    synExp* copy_exp();
    void computeSymbols(std::set<std::string>* ss) {
        ss->insert(name);
    };
    virtual synOp                           get_op()            const {
        std::ostringstream ost;
        ost << "Error accessing getOp in synSymbol:" << this << ", "
                    << (synExp*) this << std::endl;
        throw std::logic_error(ost.str());
    }
    void    destroy();
    //void    clear() { mySymbols.clear(); myIndices.clear(); myOps.clear(); }

    ~synSymbol() {
        myOps.clear();
    };

    
private:
    
    std::string           name;
    std::set<synOp>       myOps;

    
    void print(std::ostream& os) const;

};


class synCompound : public synExp {
public:
    synCompound(synOp op, synExp *e1, synExp *e2 = synFalse, synExp *e3 = synFalse) : synExp(), Cop(op), Ce1(e1), Ce2(e2), Ce3(e3)   {
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
    }
    int     opPriority() const  {
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

    synExpT                         get_type()          const   { return synExp_Compound;   }
    synOp                           get_op()            const   { return Cop;               }
    synExp                          *first()            const   { return Ce1;               }
    synExp                          *second()           const   { return Ce2;               }
    synExp                          *third()            const   { return Ce3;               }
    const std::set<synOp>&          giveOps()           const   { return myOps;             }

    

    friend std::ostream& operator<<(std::ostream& os, synCompound* ps);
    bool                            isOr()                        { return Cop == synOr         ;}
    bool                            isAnd()                       { return Cop == synAnd        ;}
    bool                            isNot()                       { return Cop == synNot        ;}
    bool                            isImplies()                   { return Cop == synImplies    ;}
    bool                            isIf()                        { return Cop == synIfThenElse ;}
    bool                            isEqual()                     { return Cop == synEqual      ;}

    bool                            isLiteral()                   { return Cop == synNot &&
                                                                Ce1->get_type() == synExp_Symbol; }
    synExp* copy_exp();
    void computeSymbols(std::set<std::string>* ss) {
        Ce1->computeSymbols(ss);
        Ce2->computeSymbols(ss);
        Ce3->computeSymbols(ss);
    };
    
    void    destroy();
    //void    clear() { mySymbols.clear(); myIndices.clear(); myOps.clear(); }

    ~synCompound() {
        myOps.clear();
    };

private:
    
    std::set<synOp>       myOps;

    synOp Cop;
    synExp *Ce1, *Ce2, *Ce3;
    void print(std::ostream& os) const;
    friend  bool equal(synExp *e1, synExp *e2);

};

class synXOR : public synExp {
public:
    
    synXOR(std::vector<synExp*> v) : v(v) { myOps.insert(synXor); };
    synExpT                         get_type()          const   { return synExp_XOR;   }
    std::vector<synExp*>            get_parms()         const   { return v;            }
    int                             opPriority()        const   { return 4;            }
    bool    isXOR()                                             { return true; }
    const std::set<synOp>&          giveOps()           const   { return myOps;                   }

    void print(std::ostream& os) const;
    friend std::ostream& operator<<(std::ostream& os, synXOR* ps);
    void    destroy();
    void computeSymbols(std::set<std::string>* ss) {
        for(synExp* s : v)
            s->computeSymbols(ss);
    }
    virtual synOp                           get_op()            const {
        std::ostringstream ost;
        ost << "Error accessing getOp in XOR:" << this << std::endl;
        throw std::logic_error(ost.str());
    }
    synExp* copy_exp();

    
private:
    std::vector<synExp*> v;
    std::set<synOp>       myOps;

};


class synString : public synExp {
public:
    
                                    synString(std::string s) : synExp(), st(s)     {
                                    };
    bool                            isString()                { return true; }
    synExpT                         get_type()          const { return synExp_String;   }
    std::string                     get_string()        const { return st;              }
    synExp*                         eval()                    { return this;            }
    int                             opPriority()        const { return 1;               }
    const std::set<synOp>&          giveOps()           const { return myOps;           }

    synOp                           get_op()            const {
        std::ostringstream ost;
        ost << "Error accessing getOp in synString:" << st << std::endl;
        throw std::logic_error(ost.str());
    }
    friend std::ostream& operator<<(std::ostream& os, synString* ps);
    synExp* copy_exp();
    //void    clear() { mySymbols.clear(); myIndices.clear(); myOps.clear(); }

    ~synString() {
        myOps.clear();
    };

    
private:
    std::set<synOp>       myOps;
    std::string st;
    void print(std::ostream& os) const;
    void    destroy();

};

class synUnknown : public synExp {
public:
    
    synUnknown(std::string s) : synExp(), st(s)     {};
    synExpT                         get_type()          const { return synExp_Unknown;   }
    std::string                     get_string()        const { return st;              }
    synExp*                         eval()                    { return this;            }
    int                             opPriority()        const { return 1;               }
    const std::set<synOp>&          giveOps()           const { return myOps;           }
    bool                            isUnknown()               { return true; }
    synOp                           get_op()            const {
        std::ostringstream ost;
        ost << "Error accessing getOp in synUnknown:" << st << std::endl;
        throw std::logic_error(ost.str());
    }
    synExp* copy_exp();

    ~synUnknown() {
        myOps.clear();
    };

    
private:
    std::set<synOp>       myOps;
    std::string st;
    void    destroy();

};




#endif /* defined(__myKconf__synExp__) */
