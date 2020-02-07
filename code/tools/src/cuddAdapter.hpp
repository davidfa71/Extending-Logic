//
//  cuddAdapter.h
//  myKconf
//
//  Created by david on 15/12/14.
//  Copyright (c) 2014 david. All rights reserved.
//

#ifndef __myKconf__cuddAdapter__
#define __myKconf__cuddAdapter__

#include "OneComponent.hpp"
#include "MultiComponents.hpp"
#include "SemanticsCudd.hpp"
#include "humanNums.hpp"
#include "mytime.hpp"
#include "humanNums.hpp"
#include "MenuBlock.hpp"

#include <fstream>
#include <mtr.h>
#include <limits>
#include <cuddObj.hh>
#include <cudd.h>
#include <dddmp.h>
#include <iomanip>
#include <list>
#include <map>
#include <set>

using namespace cudd;


class cuddAdapter {
    public :
    
    cuddAdapter();
    ~cuddAdapter();
    cuddAdapter(double cacheMultiplier);
    void                useComponents();
    void                useComponents(Components* pcomp);
    void                useComponents(std::vector<int> var2pos, std::vector<int> pos2var);
    void                init();
    int                 getNumComponents();
    int                 getMaxComponent();
    float               getComponentEntropy();
    int                 getComponent(int x);
    int                 getCompSize(int x) ;
    int                 getNumVars();
    std::vector<int>    pos2var();
    std::vector<int>    var2pos();
    void                changeOrder(std::vector<int> pos2var);

    DdNode*             getBDD();
    DdNode*             getZero();
    DdNode*             getOne();
    Cudd                getCudd() { return mgr; };
    int                 getLevel(DdNode const * node);
    int                 getLevel(DdNode* node);
    int                 varAtPos(int pos);
    int                 posOfVar(int var);
    void                destroyQuantified(int n);
    void                setValue(std::string, synExp*);
    void                reorderComponent(std::vector<std::string>& ss,  std::string rmethod);
    void                reduce(const std::string& slow, const std::string& shigh);
    int                 getNodes() { return 0; }
    void                setUpVariable(const std::string& var);
    void                printBDD();
    void                reorder(std::string reorderMethod);
    int                 addblock(std::string x);
    int                 addMenublock(MenuBlock *m);
    void                initblocks();
    std::vector<std::string>    giveOrder();
    void                namevar(std::string name, int index);
    void                newVar(std::string var, std::string type);
    void                newVar(std::string var, std::string type, int pos);
    void                moduleImplies(std::string var);
    bool                apply(synExp * s);
    bool                apply(std::string s, synExp * exp);
    std::map<std::string, Cudd_ReorderingType> reorderMap;
    const   int                 nodecount();
    const   int                 nodecount(std::string s);
    
    void    saveBDD(const std::string& base, const std::string &suffix);
    void    saveBDD(const std::string id, const std::string& b, const std::string &s);
    void    readBDD(const std::string& b, const std::string &s);
    void    readBDD(const std::string id, const std::string& b, const std::string &s);
    bool    sameBDD(std::string s1, std::string s2);
    static  int MAXVAR;
    
    Cudd    mgr;
    const   cudd::BDD           process(synExp* exp);
    //int                         getLevelNodes(int level) const;
    int                         getNumComponents()       const;
    void                        shuffle(const std::vector<int>& order);
    void                        shuffle(const std::vector<std::string>& order);
    void                        computeProbabilities();
    void                        existentialQ(const std::set<std::string>& v);
    void                        destroyInternal(const std::set<std::string>& v);
    std::set<std::string>       checkOneSolution();
    std::set<std::string>       checkOneSolutionRec(DdNode* node, int level);
    std::string                 getVarName(int x);
    void                        putInternalAtTheBottom(std::set<std::string> ss);
    void                        reorderComponents(int nreorder);
    void                        checkComponents();
    void 			            checkOrder();
    void                        syncOrder();
    void                        incInternalUsed(synExp *s);
    void                        setXORBlocks(synExp* s);
    void 	                    relateVars(std::set<int>);
    void			            setRelatedVars() { relatedVars = true; }

    
private:
    
    
    void auxXOR(synExp* exp);
    void createXORBlock(std::vector<std::string>& vec);


    void changeUp(int pos, int len, int ind, int lenhigh, int* perm);
    void changeDown(int pos, int len, int ind, int lenlow, int* perm);
    void internalRefs();
    
    std::map<std::string, int>  internalReferenced, internalUsed;
    bool		    relatedVars = false;
    Components*             pcomp;
    std::set<std::string>   quantify;
    
    std::vector<int>        int2extLevel, levelnodes, otherLevel;
    std::set<std::string>   setImplies;
    int                     countVar = 0;
    cudd::BDD               theBDD;
    void                    printVars();
    cudd::BDD               getVar(std::string var);
    std::map<std::string, std::pair<cudd::BDD, cudd::BDD> >     vars;
    std::map<std::string, std::pair<int, int> >                 indices;
    std::vector<std::string>            inVars;
    std::vector<bool>                   startComponent;
    int                                 reorderMin, reorderMax, numComponents;
    std::string minVar, maxVar;
    bool                                emptyTree;
    bool                                withComponents;
    std::pair<int, int>                 findSmallestBlock(int pos, int pos2);
    int                                 numVars;
    std::set<std::pair<int, int> >      currentBlocks, presentBlocks;
    
    std::map<DdNode*, bool>             mark;
    std::set<std::string>               artVars;
    std::map<std::string, cudd::BDD>    storage;
    std::vector<MenuBlock*>             theMenuBlocks;
    int                                 reorderCounter;
    cudd::BDD   auxReadBDD(const std::string& base, const std::string &suffix);

};

#endif /* defined(__myKconf__cuddAdapter__) */
