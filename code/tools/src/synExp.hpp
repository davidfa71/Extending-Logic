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
#include <unordered_set>
#include <list>
#include <vector>
#include <sstream>
#include <iomanip>
#include <limits>
#include <math.h>
#include <execinfo.h>
#include <signal.h>
#include <unordered_map>
#include <cxxabi.h>
#include <sys/wait.h>
#include <unistd.h>
//#include <sys/prctl.h>

class synExp;

typedef std::vector<int>                     vint;
typedef std::unordered_set<std::string>      sstring;
typedef std::unordered_map<std::string, int> String2Int;
typedef std::vector<synExp*>                 vSynExp;
typedef std::unordered_map<std::string, synExp*> SubMap;

enum synExpT    { synExp_Const,
                  synExp_Symbol,
                  synExp_Compound,
                  synExp_XOR,
                  synExp_String,
                  synExp_Unknown,
                  synExp_Comment,
                  synExp_Macro
};
enum synConstsT { synConst_false,
                  synConst_module,
                  synConst_true
};
enum synOp      { synNot,
                  synAnd,
                  synOr,
                  synImplies,
                  synIfThenElse,
                  synEqual,
                  synXor
};
typedef std::set<synOp> SetSynOp;

class synExp {
    
    public:
    //static void showRemaining();
    static void flattenAnd(synExp* exp, std::unordered_set<synExp*>& ss);
    static void flattenOr(synExp* exp, std::unordered_set<synExp*>& ss);
    synExp() {
        mySymbols = NULL;
#ifdef _DEBUG_
    //store[this] = getStackTrace();
#endif
    }
  
#ifdef _DEBUG_
    
//    std::string getStackTrace() {
//        std::ostringstream ost;
//        void *trace[30];
//        char **messages = (char **)NULL;
//        size_t size;
//        int i = 0;
//
//        // get void*'s for all entries on the stack
//        size = backtrace(trace, 30);
//
//        messages = backtrace_symbols(trace, size);
//          /* skip first stack frame (points here) */
//        int num;
//        std::string file;
//        std::string addr;
//        std::string symbol;
//        std::string plus;
//        int offset = 0;
//        int status = 0;
//          for (i=0; i<size; ++i)
//          {
//              std::istringstream ist(messages[i]);
//              ist >> num >> file >> addr >> symbol >> plus >> offset;
//              char *demangled = abi::__cxa_demangle(symbol.c_str(), 0, 0, &status);
//
//              ost << num << " " << file << " ";
//              if (demangled)
//                  ost << demangled;
//
//              ost << "+" << offset <<  std::endl;
//          }
//        return ost.str();
//        }
#endif
    //bool operator<(const synExp& a, const synExp& b);
    virtual void                       printExpresion();
    virtual synExpT                    get_type()          const;
    virtual const std::string          get_string()        const;
    virtual std::string                getSymbol()         const;
    virtual std::string                getComment()        const;
    virtual std::string                getMacro()          const;
    virtual synOp                      get_op()            const;
    virtual synExp*                    first()             const;
    virtual synExp*                    second()            const;
    virtual synExp*                    third()             const;
    virtual vSynExp                    get_parms()         const;
    virtual const SetSynOp&            giveOps()           const;
    virtual bool                       isCompound()        const;
    virtual bool                       isOr()              const;
    virtual bool                       isAnd()             const;
    virtual bool                       isNot()             const;
    virtual bool                       isImplies()         const;
    virtual bool                       isIf()              const;
    virtual bool                       isEqual()           const;
    virtual bool                       isSymbol()          const;
    virtual bool                       isLiteral()         const;
    bool                               isDisjunctionOfLiterals() const;
    virtual bool                       isXOR()             const;
    virtual bool                       isConst()           const;
    virtual bool                       isString()          const;
    virtual bool                       isUnknown()         const;
    virtual bool                       isComment()         const;
    virtual bool                       isMacro()           const;
    // Does the formula contain the mod constant?
    virtual bool                       hasConstantModule() const;
    virtual synExp*                    copy_exp();
    virtual void                       destroy();
    virtual void                       computeSymbols(sstring *ss);
    virtual int                        opPriority()        const;
    void                               deleteSymbols();
    void                               setSymbols(sstring*ss);
    const sstring&                     giveSymbols();
    const std::unordered_set<int>&     giveSymbolIndices() const;
    void                               computeIndices(String2Int& m);
    friend  std::ostream&              operator<<(std::ostream& os, synExp* ps);
    friend  bool                       equal(synExp *e1, synExp *e2);
     
    int                                giveMax(const vint& var2pos) const;
    int                                giveMaxInd(const vint& q)    const;
    int                                giveMinInd(const vint& q)    const;
    int                                giveMin(const vint& var2pos) const;
    int                                getMax()                     const;
    int                                getMin()                     const;
    int                                getSmartSpan()               const;
    void                               setSmartSpan(int n);
    int                                getLCA()                     const;
    float                              getScore()                   const;
    void                               computeMaxMin(const vint& var2pos);
    void                               setScore(double x);
    int                                computeSpan(const vint& var2pos) const;
    void                               setLCA(int x);
    void                               thisOneIsProcessed();
    static void                        numVars(int s);
    virtual vint                       toDimacs(String2Int& theMap);
    std::list<synExp *>                Tseitin(int& varnum);
    std::list<synExp*>                 Tseitin_rec(synExp* e, int& varnum);
    virtual synExp*                    substitute(SubMap& map);
    virtual void                       print(std::ostream& os) const;
    virtual                           ~synExp();

protected:
    
#ifdef _DEBUG_
    //static std::unordered_map<synExp*, std::string> store;
#endif
    bool   tristate;
    static vint ocSoFar;
    static vint weights;
    static String2Int string2int;
    static std::unordered_map<int, std::string> int2string;
    sstring *mySymbols;
    std::unordered_set<int>         myIndices;
    int max, min, lca, smartspan;
    float score;

};


extern synExp *synTrue;
extern synExp *synModule;
extern synExp *synFalse;
synExp *makeAnd(synExp* e1, synExp* e2);
synExp *makeAnd(std::vector<synExp*>& s);
synExp *makeOr(synExp* e1, synExp* e2, bool assumeBool = false);
synExp *makeOr(std::vector<synExp*>& s);
synExp *makeNot(synExp* e1);
synExp *makeImplies(synExp* e1, synExp* e2);
synExp *makeIfThenElse(synExp* e1, synExp* e2, synExp* e3, bool assumeBool = false);
synExp *makeEqual(synExp* e1, synExp* e2, bool assumeBool = false);

synExp *makeXOR(vSynExp& v);
std::list<synExp *> expandImplication(synExp* e);
std::list<synExp *> toCNF(synExp* e);


class synConst : public synExp {
public:
    synConst();
    static synConst*       getSynFalse();
    static synConst*       getSynModule();
    static synConst*       getSynTrue();
    static void            delConst();
           bool            isConst()           const;
           synExp*         copy_exp();
           void            destroy();
           bool            hasConstantModule() const;
           synOp           get_op()            const;
           const SetSynOp& giveOps()           const;
           int             opPriority()        const;
           synExpT         get_type()          const;
           synConstsT      get_const()         const;
           synExp*         substitute(SubMap& map);

private:

    SetSynOp  myOps;
    static synConst *syncTrue, *syncFalse, *syncModule;
    synConst(synConstsT oneConst) : theConst(oneConst) {};
    synConstsT theConst;
    void print(std::ostream& os) const;

};


class synSymbol : public synExp {
public:
    synSymbol(int num);
    synSymbol(const std::string& s);
    synExpT                         get_type()          const;
    std::string                     getSymbol()         const;
    const SetSynOp&                 giveOps()           const;
    int                             opPriority()        const;
    bool                            isLiteral()         const;
    bool                            isSymbol()          const;
    virtual vint                    toDimacs(String2Int& theMap);
    virtual synExp*                 copy_exp();
    void                            computeSymbols(sstring* ss);
    virtual synOp                   get_op()            const;
    void                            destroy();
    synExp*                         substitute(SubMap& map);

    ~synSymbol();
    
private:
    
    int                   mapNum;
    SetSynOp              myOps;
    bool                  tristate;

    
    void print(std::ostream& os) const;

};


class synCompound : public synExp {
public:
    synCompound(synOp op,
                synExp *e1,
                synExp *e2 = synFalse,
                synExp *e3 = synFalse);
    int                             opPriority()          const;
    synExpT                         get_type()            const;
    synOp                           get_op()              const;
    synExp                          *first()              const;
    synExp                          *second()             const;
    synExp                          *third()              const;
    const SetSynOp&                 giveOps()             const;
    bool                            isCompound()          const;
    bool                            isOr()                const;
    bool                            isAnd()               const;
    bool                            isNot()               const;
    bool                            isImplies()           const;
    bool                            isIf()                const;
    bool                            isEqual()             const;
    bool                            isLiteral()           const;
    bool                            hasConstantModule()   const;
    void                            setFirst(synExp* f);
    void                            setSecond(synExp* s);
    void                            setThird(synExp* t);
    friend std::ostream& operator<<(std::ostream& os, synCompound* ps);
    synExp*                         copy_exp();
    void                            computeSymbols(sstring* ss);
    void                            destroy();
    synExp*                         substitute(SubMap& map);

    ~synCompound();

private:
    
    SetSynOp  myOps;
    synOp   Cop;
    synExp *Ce1, *Ce2, *Ce3;
    void    print(std::ostream& os) const;
    friend  bool equal(synExp *e1, synExp *e2);

};

class synXOR : public synExp {
public:
    
    synXOR(vSynExp v);
    synExpT                         get_type()          const;
    vSynExp                         get_parms()         const;
    void                            set_parms(vSynExp& w);
    void                            clear_parms();
    int                             opPriority()        const;
    bool                            isXOR()             const;
    const SetSynOp&                 giveOps()           const;
    void print(std::ostream& os)                        const;
    friend std::ostream& operator<<(std::ostream& os, synXOR* ps);
    void                            destroy();
    void                            computeSymbols(sstring* ss);
    virtual synOp                   get_op()            const;
    synExp*                         copy_exp();
    bool                            hasConstantModule() const;
    synExp*                         substitute(SubMap& map);

    
private:
    vSynExp v;
    SetSynOp       myOps;

};


class synString : public synExp {
public:
    synString(const std::string& s);
    bool                            isString()          const;
    synExpT                         get_type()          const;
    const std::string               get_string()        const;
    synExp*                         eval();
    int                             opPriority()        const;
    const SetSynOp&                 giveOps()           const;
    synOp                           get_op()            const;
    synExp*                         copy_exp();
    synExp*                         substitute(SubMap& map);
    ~synString();
    friend std::ostream& operator<<(std::ostream& os, synString* ps);

private:
    
    SetSynOp       myOps;
    std::string     st;
    void    print(std::ostream& os) const;
    void    destroy();

};

class synUnknown : public synExp {
public:
    
    synUnknown(const std::string& s);
    synExpT                         get_type()          const;
    const std::string               get_string()        const;
    synExp*                         eval();
    int                             opPriority()        const;
    const SetSynOp&                 giveOps()           const;
    bool                            isUnknown()         const;
    synOp                           get_op()            const;
    synExp*                         copy_exp();
    void computeSymbols(sstring *ss);
    synExp*                         substitute(SubMap& map);
    virtual synExp*                 first()             const;
    ~synUnknown();
    
private:
    SetSynOp       myOps;
    std::string st;
    void print(std::ostream& os) const;
    void    destroy();

};

class synComment : public synExp {
public:
    synComment(const std::string& s);
    synExpT                         get_type()          const;
    std::string                     getComment()        const;
    bool                            isComment()         const;
    vint                            toDimacs(String2Int& theMap);
    friend std::ostream& operator<<(std::ostream& os, synComment* ps);
    int                             opPriority()        const;
    const SetSynOp&                 giveOps()           const;
    synOp                           get_op()            const;
    synExp* copy_exp();
    synExp*                         substitute(SubMap& map);

    ~synComment();
    
    private:
        SetSynOp       myOps;
        std::string comment;
        void print(std::ostream& os) const;
        void destroy();

};

class synMacro : public synExp {
public:
    synMacro(const std::string& s);
    synExpT                         get_type()          const;
    std::string                     getMacro()          const;
    bool                            isMacro()           const;
    vint                            toDimacs(String2Int& theMap);
    friend std::ostream& operator<<(std::ostream& os, synComment* ps);
    int                             opPriority()        const;
    const SetSynOp&                 giveOps()           const;
    synOp                           get_op()            const;
    synExp*                         copy_exp();
    synExp*                         substitute(SubMap& map);

    ~synMacro();
    
    private:
        SetSynOp       myOps;
        std::string macro;
        void print(std::ostream& os) const;
        void destroy();
};

#endif /* defined(__myKconf__synExp__) */
