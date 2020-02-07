
//  cuddAdapter.cpp
//  myKconf
//
//  Created by david on 15/12/14.
//  Copyright (c) 2014 david. All rights reserved.
//


#include "cuddAdapter.hpp"
#include "cuddInt.h"

int cuddAdapter::MAXVAR = 100000;

using namespace cudd;

cuddAdapter::~cuddAdapter() {
    delete pcomp;
    for(MenuBlock* m : theMenuBlocks)
	delete m;
}

DdNode*          cuddAdapter::getBDD()  { return theBDD.getNode();         }
DdNode*          cuddAdapter::getZero() { return mgr.bddZero().getNode();  }
DdNode*          cuddAdapter::getOne()  { return mgr.bddOne().getNode();   }

int                 cuddAdapter::getNumComponents()  { return pcomp->getNumComponents(); }
int                 cuddAdapter::getMaxComponent()   { return pcomp->getMaxLength();     }
int                 cuddAdapter::getComponent(int x) { return pcomp->getComponent(x);    }
int                 cuddAdapter::getCompSize(int x)  { return pcomp->getCompSize(x);     }
int                 cuddAdapter::getNumVars()        { return numVars;                   }
float               cuddAdapter::getComponentEntropy() { return pcomp->computeEntropy();  }

void cuddAdapter::reorder(std::string reorderMethod) {
    reorderCounter++;
    std::cerr << "Reorder #" << reorderCounter << std::endl;
    std::set<std::pair<int, int> > ts;
    ts = pcomp->listChangedComponents();
    for(std::pair<int, int> lcomp : ts) {
        if (lcomp.first+lcomp.second > numVars) {
            std::cerr << "There is a component with start " << lcomp.first << " and end " << lcomp.first+lcomp.second
            << " when there are only " << numVars << " variables." << std::endl;
            throw std::logic_error("");
        }
        
        if (lcomp.second > 1) {
            
            std::cerr  << "Reordering component start "
            << std::setw(4) << lcomp.first
            << " size "  << std::setw(4) << lcomp.second
            << "..." << std::flush;
            
            // Why do we need a vector of strings?
            // Because at this point we do not know
            // if there are any trivalued variables
            
            // They are still just variables
            // but the real length of the component
            // for CUDD will be computed very soon
            std::vector<std::string> compVars;
            int c = lcomp.first;
            for(int w = 0; w < lcomp.second; w++) {
                //std::cerr << c << " ReadInv " << mgr.ReadInvPerm(c) << " inVars "
                //        << inVars[mgr.ReadInvPerm(c)] << std::endl;
                compVars.push_back(inVars[mgr.ReadInvPerm(c++)]);
            }
            
            reorderComponent(compVars, reorderMethod);
            std::cerr << "done " << std::endl;
            // This is only necessary for the first component
            pcomp->setUnchanged(mgr.ReadInvPerm(lcomp.first));
        }
        
    }
    pcomp->changeOrder(pos2var());
    pcomp->sync();
}
//void cuddAdapter::putInternalAtTheBottom(std::set<std::string> ss) {
//    std::vector<std::string> ssorder;
//    // First round: Not internal variables
//    for(int j : pos2var) {
//        std::string s = varList[j];
//        if (ss.find(s) == ss.end()) {
//            ssorder.push_back(s);
//        }
//    }
//    // Second round: internal variables
//    for(int j : pos2var) {
//        std::string s = varList[j];
//        if (ss.find(s) != ss.end()) {
//            ssorder.push_back(s);
//        }
//    }
//    factory->shuffle(ssorder);
//    std::vector<std::string> go = factory->giveOrder();
//    
//    int c = 0;
//    for(std::string & s : go) {
//        var2pos[varMap[s]] = c;
//        pos2var[c++] = varMap[s];
//    }
//    std::cerr << " go size " << go.size() << " c " << c << std::endl;
//}


//void bddBuilder::createBDDVariables() {
//    std::vector<std::string> ssorder;
//    int nn = 0;
//    //First round: Not internal variables
//    for(int var : pos2var) {
//        std::string s = varList[var];
//        if (internal.find(s) == internal.end()) {
//            ssorder.push_back(s);
//            factory->newVar(s, "boolean");
//            nn++;
//        }
//    }
//    // Now the internal ones. We put them at the end (in the index order)
//    // Because it is needed to destroy them after the existential quantification
//    
//    for(int var : pos2var) {
//        std::string s = varList[var];
//        if (internal.find(s) != internal.end()) {
//            ssorder.push_back(s);
//            factory->newVar(s, "boolean");
//            nn++;
//        }
//    }
//    // Now we set the right order (internal and all)
//    //
//    factory->shuffle(ssorder);
//}
//void cuddAdapter::incInternalUsed(synExp *s) {
//    for(auto var : s->giveSymbols())
//        if (internal.find(var) != internal.end()) {
//            internalUsed[var]++;
//            if (internalUsed[var] == internalReferenced[var])
//                quantify.insert(var);
//        }
//}
//void  cuddAdapter::internalRefs() {
//    for(auto exp : consList)
//        for(auto var : exp->giveSymbols())
//            if (internal.find(var) != internal.end()) {
//                internalReferenced[var]++;
//            }
//}

int                 cuddAdapter::getLevel(DdNode const* node) {
    if (Cudd_IsConstant(Cudd_Regular(node))) {
        return (mgr.ReadSize());
    }
    else {
        return (mgr.ReadPerm(Cudd_Regular(node)->index));
    }
}

int cuddAdapter::getLevel(DdNode* node) {
    if (Cudd_IsConstant(Cudd_Regular(node))) {
        return (mgr.ReadSize());
    }
    else {
        return (mgr.ReadPerm(Cudd_Regular(node)->index));
    }
}


void   cuddAdapter::existentialQ(const std::set<std::string>& v) {
    cudd::BDD conjunction = mgr.bddOne();
    for(std::string s : v)
        conjunction = conjunction & getVar(s);
    
    theBDD = theBDD.ExistAbstract(conjunction);
}

void cuddAdapter::destroyInternal(const std::set<std::string>& v) {
    theBDD.destroySubtables(v.size());
    numVars = numVars - v.size();
    std::vector<std::string> auxVars;
    for(int x = 0; x < numVars; x++)
        auxVars.push_back(inVars[x]);
    
    inVars.clear();
    inVars = auxVars;
}

cuddAdapter::cuddAdapter(double cacheMultiplier) :
        mgr(0,0,CUDD_UNIQUE_SLOTS, cacheMultiplier*CUDD_CACHE_SLOTS, 0) {
            withComponents = false;
            pcomp = new OneComponent();
            init();
            reorderCounter = 0;
}

cuddAdapter::cuddAdapter() :
mgr(0,0,CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0) {
    withComponents = false;
    // Problematic as it is because we don't know how many variables we have
    pcomp = new OneComponent();
    init();
    reorderCounter = 0;
}

void cuddAdapter::useComponents(Components* xPcomp) {
    delete pcomp;
    pcomp = xPcomp;
}
void cuddAdapter::useComponents() {
    delete pcomp;
    pcomp = new MultiComponents();
    withComponents = true;
}

void cuddAdapter::useComponents(std::vector<int> var2pos, std::vector<int> pos2var) {
    delete pcomp;
    pcomp = new MultiComponents(var2pos, pos2var);
    withComponents = true;
}

void cuddAdapter::init() {
    SemanticsCudd::setManager(&mgr);
    numVars = 0;
    reorderMap["CUDD_REORDER_NONE"]             = CUDD_REORDER_NONE;
    reorderMap["CUDD_REORDER_SAME"]             = CUDD_REORDER_SAME;
    reorderMap["CUDD_REORDER_RANDOM"]           = CUDD_REORDER_RANDOM;
    reorderMap["CUDD_REORDER_RANDOM_PIVOT"]     = CUDD_REORDER_RANDOM_PIVOT;
    reorderMap["CUDD_REORDER_SIFT"]             = CUDD_REORDER_SIFT;
    reorderMap["CUDD_REORDER_SIFT_CONVERGE"]    = CUDD_REORDER_SIFT_CONVERGE;
    reorderMap["CUDD_REORDER_SYMM_SIFT"]        = CUDD_REORDER_SYMM_SIFT;
    reorderMap["CUDD_REORDER_SYMM_SIFT_CONV"]   = CUDD_REORDER_SYMM_SIFT_CONV;
    reorderMap["CUDD_REORDER_GROUP_SIFT"]       = CUDD_REORDER_GROUP_SIFT;
    reorderMap["CUDD_REORDER_GROUP_SIFT_CONV"]  = CUDD_REORDER_GROUP_SIFT_CONV;
    reorderMap["CUDD_REORDER_WINDOW2_CONV"]     = CUDD_REORDER_WINDOW2_CONV;
    reorderMap["CUDD_REORDER_WINDOW3_CONV"]     = CUDD_REORDER_WINDOW3_CONV;
    reorderMap["CUDD_REORDER_WINDOW4_CONV"]     = CUDD_REORDER_WINDOW4_CONV;
    reorderMap["CUDD_REORDER_ANNEALING"]        = CUDD_REORDER_ANNEALING;
    reorderMap["CUDD_REORDER_GENETIC"]          = CUDD_REORDER_GENETIC;
    reorderMap["CUDD_REORDER_EXACT"]            = CUDD_REORDER_EXACT;
    theBDD = mgr.bddOne();
    reorderMin = std::numeric_limits<int>::max();
    reorderMax = -1;
    minVar = maxVar = "";
    mgr.AutodynDisable();
    mgr.SetSiftMaxSwap(std::numeric_limits<int>::max());
    mgr.SetSiftMaxVar(std::numeric_limits<int>::max());
    mgr.SetMaxGrowth(std::numeric_limits<double>::max());
    //mgr.DisableGarbageCollection();
}
void    cuddAdapter::setValue(std::string s , synExp* e) {
    if (e == synTrue) {
        apply(new synSymbol(s));
        return;
    }
    if (e == synFalse) {
        apply(makeNot(new synSymbol(s)));
        return;
    }
}

std::set<std::string>   cuddAdapter::checkOneSolution() {
    DdNode* n = Cudd_Regular(theBDD.getNode());
    return checkOneSolutionRec(n, 0);
}

std::set<std::string> cuddAdapter::checkOneSolutionRec(DdNode* node, int level) {
    std::set<std::string> res, resT, resE;
    int index;
    DdNode *TNode, *ENode;
    TNode = cuddT(node);
    ENode = Cudd_Regular(cuddE(node));
    mark[node] = !mark[node];
    index = mgr.ReadPerm(node->index);
    // If there is a level jump...
    if (index != level)
        for(int x = level; x < index; x++)
            res.insert(inVars[x]);
    
    if (!cuddIsConstant(TNode) && !cuddIsConstant(ENode))
        res.insert(inVars[index]);
    
    if (mark[node] != mark[TNode] && !cuddIsConstant(TNode))
        resT = checkOneSolutionRec(TNode, index+1);
    if (mark[node] != mark[ENode] && !cuddIsConstant(ENode))
        resE = checkOneSolutionRec(ENode, index+1);
    
    if (cuddIsConstant(TNode) && cuddIsConstant(ENode) && index != mgr.ReadSize()-1)
        for(int x = index+1; x < mgr.ReadSize(); x++)
            res.insert(inVars[x]);
    
    res.insert(resT.begin(), resT.end());
    res.insert(resE.begin(), resE.end());
    
    return res;
}
// Supposedly, the ordering in vector ss is the correct one already.
void   cuddAdapter::reorderComponent(std::vector<std::string>& ss, std::string rmethod) {
    bool trivaluated = false;
    int  length = 0;
    int startPos = mgr.ReadPerm(indices[ss.front()].first);
    for(std::string& s : ss) {
      // std::cerr << "s is *" << s << "*" << " index " << indices[s].first << " pos " << mgr.ReadPerm(indices[s].first) <<  " length " << length << std::endl;
        if (indices.at(s).second == 0) {
            length++;
        }
        else {
            trivaluated = true;
            length += 2;
        }
    }
    
    //int groupStart = mgr.ReadPerm(indices[ss.front()].first);
     int groupStart = indices[ss.front()].first;
    int groupLength = length;
    // The first argument to Mtr_InitGroupTree is an index. Internally in cudd
    // it is converted to a position and then it is a position in tree->lower.
    // position, not an index.
    // The proof is shown in this snippet from cuddGroup.c:
    //if (tempTree) {
    //	table->tree = Mtr_InitGroupTree(0,table->size);
    //	table->tree->index = table->invperm[0];
    //}
    mgr.SetTree(Mtr_InitGroupTree(groupStart, length));
    //std::cerr << "SetTree " << groupStart << ", " << length << std::endl;
    for(const std::string& s : ss) {
        //std::cerr << "Blocking variable " << std::setw(60) << s
        //<< "pos first " << std::setw(5) << indices[s].first
        //<< "pos second " << std::setw(5) << indices[s].second << std::endl;
        if (indices[s].second !=  0)
            mgr.MakeTreeNodeFix(mgr.ReadPerm(indices[s].first), 2, MTR_FIXED);
    }
    for(MenuBlock *m : theMenuBlocks) {
	//std::cerr << "Menublock " << std::endl;
        int xmin = std::numeric_limits<int>::max();
        int length = 0;
        for(const std::string& s : m->getContents())
            if (indices.find(s) != indices.end()){
  		//std::cerr << "found " << s << " in menublock, index " << indices.at(s).first << " pos " << mgr.ReadPerm(indices.at(s).first) << std::endl;
                length++;
                int pos = mgr.ReadPerm(indices.at(s).first);
                if (pos < xmin)
                    xmin = pos;
            }
        //std::cerr << "end Menublock, xmin " << xmin << " startPos " << startPos << " length " << length << " groupLength " << groupLength << std::endl;

	// We can't allow reordering groups because they get split by static scoring such as perm and are not contiguous anymore
        if (xmin >= startPos && xmin+length <= startPos + groupLength) {
            //std::cerr << "Small group begin " << std::setw(4) << xmin << " length " << length << std::endl;
            //if (m->getDescription() == "XOR")
            //    mgr.MakeTreeNodeFix(xmin, m->getLength(), MTR_FIXED);
            //else
            //    mgr.MakeTreeNodeFix(xmin, m->getLength(), MTR_DEFAULT);
            
            //if (mgr.MakeTreeNode(xmin, m->getLength(), MTR_DEFAULT) == NULL) {
            //    std::cerr << "Error in MakeTreeNode" << std::endl;
            //    exit(-1);
            //}
        }
        //else
        //    std::cerr << "Small group discarded " << std::setw(4) << xmin << " length " << length << " startPos " << startPos << " groupLength " << groupLength << std::endl;
    }
    
    mgr.SetSiftMaxVar(ss.size());
    mgr.SetSiftMaxSwap(std::numeric_limits<int>::max());
    if (relatedVars)
        mgr.SetRelatedVars(); 


    Cudd_ReorderingType crt;
    if (reorderCounter % 2 == 1)
        crt = CUDD_REORDER_SIFT;
    else
        crt = CUDD_REORDER_WINDOW4_CONV;
   
    if (reorderMap.find(rmethod) != reorderMap.end())
        crt = reorderMap.at(rmethod);
    
    mgr.ReduceHeapMinMax(startPos, startPos+length-1, crt, 0);
};

void    cuddAdapter::shuffle(const std::vector<std::string>& extOrder) {
    // If there are no variables...
    if (extOrder.empty())
        return;
    if (extOrder.size() != numVars) {
        std::ostringstream ost;
        ost << "Shuffle size " << extOrder.size() << " != " << numVars << " numVars" << std::endl;
        if (numVars == 0) ost << "Maybe you are reading a dddmp file in an old format (without variable names)";
        throw std::logic_error(ost.str());
    }
    int intOrder[numVars], cont = 0;
    std::set<int> check;
    //std::cerr << std::endl << "Starting shuffle" << std::endl;
    for(const std::string& s : extOrder)
        if (indices[s].second == 0) {
            //std::cerr << std::setw(80) << s << " -> " << indices[s].first << " cont " << cont << std::endl;
            intOrder[cont++] = indices[s].first;
            check.insert(indices[s].first);
        }
        else {
            //std::cerr << std::setw(80) << s << " -> " << indices[s].first << std::endl;
            //std::cerr << std::setw(80) << s << " -> " << indices[s].second << std::endl;
            check.insert(indices[s].first);
            check.insert(indices[s].second);
            
            intOrder[cont++] = indices[s].first;
            intOrder[cont++] = indices[s].second;
        }
    //std::cerr << " cont " << cont << " extOrder size " << extOrder.size() << " numVars " << numVars << " check size " << check.size() << std::endl;
    if (cont != numVars || numVars != check.size()) {
        std::cerr << "Not enough variables for shuffling: cont "
        << cont << " numVars " << numVars << " check size " << check.size() <<  std::endl;
        exit(-1);
    }
    for(int a = 0; a < numVars; a++)
        if (check.find(a) == check.end()) {
            std::cerr << a << " not found in check set" << std::endl;
            exit(-1);
        }
    //    std::cerr << std::endl << "Calling shuffle... ";
    //    for(int x : intOrder)
    //        std::cerr << x << " ";
    //    std::cerr << std::endl;
    mgr.SetTree(Mtr_InitGroupTree(mgr.ReadInvPerm(0), numVars));
    mgr.ShuffleHeap(intOrder);
    //std::cerr << " done" << std::flush << std::endl;
};

void    cuddAdapter::shuffle(const std::vector<int>& extOrder) {
    // If there are no variables...
    if (extOrder.empty())
        return;
    int intOrder[numVars], cont = 0;
    std::set<int> check;
    //std::cerr << std::endl << "Starting shuffle" << std::endl;
    for(const int index : extOrder) {
            intOrder[cont++] = index;
            check.insert(index);
        }
    
    if (cont != numVars || numVars != check.size()) {
        std::cerr << "Not enough variables for shuffling: cont "
        << cont << " numVars " << numVars << " check size " << check.size() <<  std::endl;
        exit(-1);
    }
    for(int a = 0; a < numVars; a++)
        if (check.find(a) == check.end()) {
            std::cerr << a << " not found in check set" << std::endl;
            exit(-1);
        }

    mgr.SetTree(Mtr_InitGroupTree(mgr.ReadInvPerm(0), numVars));
    mgr.ShuffleHeap(intOrder);
};

void cuddAdapter::changeUp(int pos, int len, int ind, int lenhigh, int* perm) {
    for(int l = lenhigh-1; l >= 0; l--)
        perm[pos-lenhigh+len+l] = perm[pos-lenhigh+l];
    
    for(int l = 0; l < len; l++)
        perm[pos-lenhigh+l] = ind+l;
}

void cuddAdapter::changeDown(int pos, int len, int ind, int lenlow, int* perm) {
    for(int l = 0; l < lenlow; l++)
        perm[pos+l] = perm[pos+len+l];
    
    for(int l = 0; l < len; l++)
        perm[pos+lenlow+l] = ind+l;
}

void cuddAdapter::reduce(const std::string& slow, const std::string& shigh) {
    int order[numVars+1], lenlow, lenhigh, lenother, inhigh, inlow, poslow, poshigh, oldnodes, nodes;
    bool keeplow, keephigh;
    keeplow = keephigh = true;
    for(int x = 0; x < numVars; x++) {
        order[x] = mgr.ReadInvPerm(x);
    }
    oldnodes        = theBDD.nodeCount();
    nodes           = oldnodes;
    inlow           = indices.at(slow).first;
    inhigh          = indices.at(shigh).first;
    std::cerr << slow << " index " << inlow << " " << shigh << " index " << inhigh << std::endl;
    if (indices[slow].second == 0)
        lenlow = 1;
    else
        lenlow = 2;
    
    if (indices[shigh].second == 0)
        lenhigh = 1;
    else
        lenhigh = 2;
    
    poslow  = mgr.ReadPerm(inlow);
    poshigh = mgr.ReadPerm(inhigh);
    
    while (poslow+1 < poshigh && (keeplow || keephigh)) {
        if (keeplow) {
            try {
                if (indices.at(inVars[mgr.ReadInvPerm(poslow+lenlow)]).second == 0)
                    lenother = 1;
                else
                    lenother = 2;
                changeDown(poslow, lenlow, inlow, lenother, order);
            }
            catch(std::exception e) {
                std::cerr << "Error looking for key  " << std::setw(50) << inVars[mgr.ReadInvPerm(poslow+lenlow)] << std::endl;
                exit(-1);
            }
            
            poslow += lenother;
            mgr.ShuffleHeap(order);
            nodes    = theBDD.nodeCount();
            if (nodes > oldnodes) {
                keeplow = false;
                changeUp(poslow, lenlow, inlow, lenother, order);
                mgr.ShuffleHeap(order);
                poslow -= lenother;
            }
        }
        oldnodes = nodes;
        if (keephigh && poslow+1 < poshigh) {
            if (indices.find(inVars[mgr.ReadInvPerm(poshigh-1)]) != indices.end())
                lenother = 1;
            else
                lenother = 2;
            
            changeUp(poshigh, lenhigh, inhigh, lenother, order);
            poshigh -= lenother;
            mgr.ShuffleHeap(order);
            nodes    = theBDD.nodeCount();
            if (nodes > oldnodes) {
                keephigh = false;
                changeDown(poshigh, lenhigh, inhigh, lenother, order);
                mgr.ShuffleHeap(order);
                poshigh += lenother;
            }
        }
        oldnodes = nodes;
        
        
    }
    
    return;
}
int     cuddAdapter::getNumComponents() const { return numComponents; }



void   cuddAdapter::setUpVariable(const std::string& var) {
    int pos = mgr.ReadPerm(indices[var].first);
    moduleImplies(var);
    if (pos < reorderMin) {
        reorderMin = pos;
        minVar     = var;
    }
    if (pos > reorderMax) {
        reorderMax = pos;
        maxVar     = var;
    }
}

void   cuddAdapter::printBDD() {
};

int    cuddAdapter::posOfVar(int var) {
    return mgr.ReadPerm(var);
}

int    cuddAdapter::varAtPos(int pos) {
    return mgr.ReadInvPerm(pos);
}


void  cuddAdapter::changeOrder(std::vector<int> pos2var) {
    pcomp->changeOrder(pos2var);
}

std::vector<int> cuddAdapter::pos2var() {
    std::vector<int> res;
    for(int x = 0; x < numVars; x++)
        res.push_back(mgr.ReadInvPerm(x));
    return res;
}

std::vector<int> cuddAdapter::var2pos() {
    std::vector<int> res;
    for(int x = 0; x < numVars; x++)
        res.push_back(mgr.ReadPerm(x));
    return res;
}



void         cuddAdapter::initblocks() {
    //mgr.SetTree(Mtr_InitGroupTree(mgr.ReadInvPerm(0), numVars - 1));
    mgr.SetTree(Mtr_InitGroupTree(mgr.ReadInvPerm(0), numVars));
    return;
}

std::pair<int, int> cuddAdapter::findSmallestBlock(int pos, int pos2) {
    int min = -1;
    std::pair<int, int> block(0,0);
    for(std::set<std::pair<int, int> >::iterator its = currentBlocks.begin();
        its != currentBlocks.end(); its++)
        if (pos >= its->first && pos2 <= its->second &&
            presentBlocks.find(*its) == presentBlocks.end() &&
            its->second - its->first < min) {
            min = its->second - its->first;
            block = *its;
        }
    return block;
}


int   cuddAdapter::addblock(std::string var) {
    int start = indices[var].first;
    
    
    if (indices[var].second ==  0) {
        //        if (mgr.MakeTreeNode(start, 1, MTR_DEFAULT) == NULL) {
        //            std::cerr   << "There was an error setting block for boolean variable "
        //                        << var << std::endl;
        //            exit(-1);
        //        }
    }
    else {
        if (mgr.MakeTreeNode(start, 2, MTR_FIXED) == NULL) {
            std::cerr   << "There was an error setting block for trivalued variable "
            << var << " with index " << start << " and pos " << mgr.ReadPerm(start) << std::endl;
            Mtr_PrintGroups(mgr.ReadTree(), 0);
            exit(-1);
        }
    }
    return 0;
}

int   cuddAdapter::addMenublock(MenuBlock *m)                    {
    std::string first = m->getFirst();
    std::string last  = m->getLast();
    int start = indices[first].first;
    int finish;
    int type;
    if (m->getType() == "choice")
        type = MTR_FIXED;
    else
        type = MTR_DEFAULT;
    
    if (indices[last].second ==  0) {
        finish = mgr.ReadPerm(indices[last].first);
        if (mgr.MakeTreeNode(start,
                             finish -  mgr.ReadPerm(indices[first].first) + 1,
                             type) == NULL) {
            std::cerr   << "1 There was an error setting a menublock between "
            << first << " and " << last
            << " position first "     << mgr.ReadPerm(indices[first].first)
            << " position last "      << finish
            << " description '"        << m->getDescription() << "'"
            << " and length "   << finish - start + 1
            << std::endl;
            Mtr_PrintGroups(mgr.ReadTree(), 0);
            exit(-1);
        }
    }
    else {
        finish =  mgr.ReadPerm(indices[last].second);
        if (mgr.MakeTreeNode(start,
                             finish -  mgr.ReadPerm(indices[first].first) + 1,
                             type) == NULL) {
            std::cerr   << "2 There was an error setting a menublock between "
            << first << " and " << last
            << " position "     << start
            << " and length "   << finish - start + 1
            << std::endl;
            exit(-1);
        }
    }
    return 0;
}


void cuddAdapter::printVars() {
    //for(const std::pair<std::string, std::pair<cudd::BDD, cudd::BDD> >& p : vars)
    //    std::cerr   << std::setw(50) << p.first << " "
    //                << std::setw(6) << p.second.first << " " << p.second.second << std::endl;
}
std::vector<std::string> cuddAdapter::giveOrder() {
    std::vector<std::string> res;
    for(int x = 0; x < numVars; x++)
        res.push_back(inVars.at(mgr.ReadInvPerm(x)));
                      
//    int2extLevel.clear();
//    int x = 0;
//    int z = 0;
//    while (x < numVars) {
//        int y = mgr.ReadInvPerm(x);
//        std::string var = inVars.at(y);
//        res.push_back(var);
//        std::cerr << "x " << x << " invperm " << y
//                 << " inVars " << var << " z " << z << std::endl;
//        int2extLevel.push_back(z);
//        try {
//            if (indices.at(var).second !=  0) {
//                x += 2;
//                int2extLevel.push_back(z);
//            }
//            else x++;
//        }
//        catch(std::exception e) {
//            std::cerr << "Error in reordering. x = " << std::setw(5) << x
//            << std::setw(5) << y << " var " << std::setw(50) << var
//            << " " << mgr.ReadInvPerm(x+1) << std::setw(50) << inVars.at(mgr.ReadInvPerm(x+1))
//            << std::endl;
//            Mtr_PrintGroups(mgr.ReadTree(), 0);
//            for(int x = 0; x < numVars; x++) {
//                std::cerr << "x " << x << " invperm " << mgr.ReadInvPerm(x)
//                << " inVars " << inVars.at(mgr.ReadInvPerm(x)) << std::endl;
//            }
//
//            exit(-1);
//        }
//        z++;
//    }
//    for(int q = 0; q < numVars; q++)
//        if (q != int2extLevel[q]) {
//            std::cerr << "int2extLevel error: " << q << " != " << int2extLevel[q] << std::endl;
//            exit(-1);
//        }
//
    return res;
}


void   cuddAdapter::namevar(std::string name, int index) {
    mgr.pushVariableName(name);
}

void   cuddAdapter::newVar(std::string var, std::string type) {
    //pcomp->newVariable();
    countVar++;
    vars[var] = std::pair<BDD, BDD>(mgr.bddVar(), mgr.bddZero());
    indices[var] = std::pair<int, int>(numVars, 0);
    inVars.push_back(var);
    mgr.pushVariableName(var);
    numVars++;
    //std::cerr << "var " << var << " indices[var] " << indices[var].first << std::endl;
}
std::string        cuddAdapter::getVarName(int x) { return inVars[x]; }

void         cuddAdapter::newVar(std::string var, std::string type, int pos) {
    countVar++;
    vars[var] = std::pair<BDD, BDD>(mgr.bddNewVarAtLevel(pos), mgr.bddZero());
    indices[var] = std::pair<int, int>(numVars, 0);
    inVars.push_back(var);
    mgr.pushVariableName(var);
    numVars++;
}
void cuddAdapter::moduleImplies(std::string var) {
    if (setImplies.find(var) == setImplies.end()) {
        setImplies.insert(var);
        if (vars.at(var).second != mgr.bddZero()) {
            // theBDD &= (!vars[var].first | vars[var].second) &
            //(!vars[var].first | vars.at("MODULES").first);
            theBDD &= (!vars[var].first | vars[var].second);
        }
    }
}

bool cuddAdapter::apply(std::string s, synExp * exp) {
    if (storage.find(s) != storage.end())
        storage[s] = storage[s] & process(exp);
    else
        storage[s] = process(exp);
    return     pcomp->join(exp, false);
}

bool cuddAdapter::apply(synExp * s) {
    theBDD = theBDD & process(s);
    bool res = pcomp->join(s, false);
    //std::cerr << std::endl;
    //for(std::string s : giveOrder())
    //    std::cerr << std::setw(3) << s << " ";
    //std::cerr << std::endl;
    //std::cerr << std::endl << "after join ";
    //pcomp->printComponents();
    //std::cerr << std::endl;
    return res;
}
void cuddAdapter::checkOrder() {
 int min = std::numeric_limits<int>::max();
 int max = -1;
 bool err = false;
 std::set<int> check;
 for(int i : pcomp->getOrder()) {
        check.insert(i);
        if (i < min) min = i;
        if (i > max) max = i;
    }
    if (check.size() != numVars) {
        std::cerr << "checkOrder error, check size " << check.size() << " numVars " << numVars;
        err = true;
    }
    if (min != 0 || max != numVars-1) {
	std::cerr << "checkOrder, check size " << check.size() << " numVars " << numVars;
    	std::cerr << "min is " << min << " and max is " << max << std::endl;
	 for(int i : pcomp->getOrder()) std::cerr << i << " "; std::cerr << std::endl;
	err = true;
    }
    if (err) exit(-1);
}
void cuddAdapter::syncOrder() {
    int nn = nodecount();
    if (withComponents) {
        //std::cerr << "Checking before pcomp reorder" << std::endl;
        checkOrder();
        //std::cerr << std::endl << "Before reorder ";
        //pcomp->printComponents();
        //std::cerr << std::endl;
        
        pcomp->reorder();

        //std::cerr <<std::endl   << "After  reorder ";
        //pcomp->printComponents();
        //std::cerr << std::endl;
        
        int theOrder[numVars];
        int c = 0;
        
        //std::cerr << std::endl << "Before syncing ";
        //saveBDD("before", "");
        //for(std::string s : giveOrder())
        //    std::cerr << std::setw(3) << s << " ";
        //std::cerr << std::endl;
        std::set<int> check;
        for(int i : pcomp->getOrder()) {
            theOrder[c++] = i;
        }
        //std::cerr << "Checking after pcomp reorder" << std::endl;
        checkOrder();
        //checkComponents();

        //std::cerr << "1 MtrGroupTree(" << mgr.ReadInvPerm(0) << ", " << numVars << ")" << std::endl;
        mgr.SetTree(Mtr_InitGroupTree(mgr.ReadInvPerm(0), numVars));
        mgr.ShuffleHeap(theOrder);

        //std::cerr << "After  syncing ";
        //saveBDD("after", "");

        //for(std::string s : giveOrder())
        //    std::cerr << std::setw(3) << s << " ";
        //std::cerr << std::endl;
        
        //checkComponents();
        
    }
    int nnow = nodecount();
    if (nnow > nn) {
        std::cerr << "Before syncing " << nn << " nodes, after syncing " << nnow << std::endl;
        mgr.DebugCheck();
    }


};

cudd::BDD  cuddAdapter::getVar(std::string var) {
    if (vars.find(var) != vars.end())
        return vars.at(var).first;
    if (storage.find(var) != storage.end())
        return storage[var];
    
    throw std::logic_error("Looking for "+var+" not a previously created variable or bdd");
}

const cudd::BDD cuddAdapter::process(synExp* exp) {
    switch (exp->get_type()) {
        case synExp_Const : {
            if (exp == synTrue)   {
                return mgr.bddOne();
            }
            if (exp == synFalse) {
                return mgr.bddZero();
            }
            if (exp == synModule) {
                return mgr.bddOne();
            }
            std::cerr << "Error in const synExp " << exp << std::endl; break;
        }
        case synExp_Symbol : {
            std::string var = exp->getSymbol();
            return getVar(var);
        }
        case synExp_Compound :
        {
            switch (exp->get_op()) {
                case synNot        : {
                    return !process(exp->first()) ;
                }
                case synAnd        : {
                    return process(exp->first()) & process(exp->second());
                }
                case synOr         : {
                    return process(exp->first()) | process(exp->second());
                }
                case synImplies    : {
                    return  !process(exp->first()) | process(exp->second());
                }
                case synIfThenElse : {
                    cudd::BDD condPart = process(exp->first());
                    cudd::BDD thenPart = process(exp->second());
                    cudd::BDD elsePart = process(exp->third());
                    
                    return condPart.Ite(thenPart, elsePart);
                }
                case synEqual      : {
                    const BDD x  = process(exp->first());
                    const BDD y  = process(exp->second());
                    return ((x & y) | (!x & !y)); }
                case synXor : {
                        throw std::logic_error("synXor could not be here");
                }
            }

        }
        case synExp_String : { std::cerr << "What is a string doing here? *" << exp->get_string() << "* "
            << std::endl; return mgr.bddZero(); }
        case synExp_XOR : {
            std::vector<DdNode*> theVector;
            // USE ONLY VAR VARIABLES or CONSTS
            for(synExp* var : exp->get_parms()) {
                if (var->isSymbol()) {
                    //std::cerr << "Var is " << var << std::endl;
                    theVector.push_back(process(var).getNode());
                    }
                else
                    if (var == synTrue)
                        theVector.push_back(mgr.bddOne().getNode());
                    else
                        if (var == synFalse)
                            theVector.push_back(mgr.bddZero().getNode());
                        else {
                            std::ostringstream ost;
                            ost << "Error, calling XOR with non-symbol argument" << var << std::endl;
                            throw std::logic_error(ost.str());
                        }
            }

            return BDD(mgr, mgr.onlyOne(theVector));
        }
        case synExp_Unknown : break;
    }
    return mgr.bddZero();
}

const int cuddAdapter::nodecount(std::string s) {
    return Cudd_DagSize(storage[s].getNode());
}

const int cuddAdapter::nodecount() {
    //std::cerr << "the node " << theBDD.getNode() << std::endl;
    return Cudd_DagSize(theBDD.getNode());
}

cudd::BDD   cuddAdapter::auxReadBDD(const std::string& base, const std::string &suffix)
{
    std::string filename;
    if (suffix == "")
        filename = base + ".dddmp";
    else
        filename = base + "-" + suffix + ".dddmp";
    
    // The variables must be created in the manager before we read the file
    // So we scan the header for the info we need and create them in case
    // they don't exist yet.
    
    if (mgr.ReadSize() == 0) {
        int nvars = 0;
        // First we read the header
        std::ifstream dump(filename);
        if (!dump.good()) {
            throw std::invalid_argument("Couldn't open file "+filename);
        }
        
        std::string word;
        std::vector<std::string> shuf;
        for(;;) {
            if (dump >> word) {
                if (word == ".nvars") {
                    dump >> nvars;
                }
                // Now it's not support variable names, it's ALL variable names
                else if (word == ".varnames")
                    for(int x = 0; x < nvars; x++) {
                        dump >> word;
                        newVar(word, "boolean");
                    }
                else if (word == ".orderedvarnames") {
                          for(int x = 0; x < nvars; x++) {
                               dump >> word;
                              shuf.push_back(word);
                          }
                    shuffle(shuf);
                }
                else if (word == ".nodes")
                    break;
            }
        }
        dump.close();
    }
    if (pcomp != NULL) delete pcomp;
    pcomp = new OneComponent(numVars);
    Dddmp_VarMatchType  varMatchMode = DDDMP_VAR_MATCHPERMIDS;
    FILE *fp = fopen(filename.c_str(), "r");
    int mode = DDDMP_MODE_TEXT;
    return cudd::BDD(mgr,
                     Dddmp_cuddBddLoad(mgr.getManager(), varMatchMode, NULL, NULL, NULL, mode, (char*)filename.c_str(), fp));
}
                            
void    cuddAdapter::readBDD(const std::string id, const std::string& base, const std::string &suffix) {
    storage[id] = auxReadBDD(base, suffix);

}

void    cuddAdapter::readBDD(const std::string& base, const std::string &suffix) {
    theBDD = auxReadBDD(base, suffix);
}

void    cuddAdapter::saveBDD(const std::string& base, const std::string &suffix)  {
    char *pvars[inVars.size()+1];
    int  mode = DDDMP_MODE_TEXT;
    Dddmp_VarInfoType varInfoType = DDDMP_VARPERMIDS;
    char **ppvars = pvars;
    for(const std::string& s : inVars) {
        *ppvars++ = (char*)s.c_str();
    }
    *ppvars = NULL;
    std::string filename;
    if (suffix == "")
        filename = base + ".dddmp";
    else
        filename = base + "-" + suffix + ".dddmp";
    //std::cout << "Writing to file " << filename << std::endl;
    FILE *fp = fopen(filename.c_str(), "w");
    // My own version of the function that writes ALL the variable names, not just variables in the support set
    Dddmp_cuddBddStore(mgr.getManager(), (char*)base.c_str(), theBDD.getNode(), pvars, NULL, mode, varInfoType, (char*)filename.c_str(), fp);
    fclose(fp);
    return;
}

void    cuddAdapter::saveBDD(const std::string id, const std::string& base, const std::string &suffix)          {
    char *pvars[inVars.size()+1];
    int  mode = DDDMP_MODE_TEXT;
    Dddmp_VarInfoType varInfoType = DDDMP_VARPERMIDS;
    //Dddmp_VarInfoType varInfoType = DDDMP_VARIDS;
    //Dddmp_VarInfoType varInfoType = DDDMP_VARNAMES;
    char **ppvars = pvars;
    for(const std::string& s : inVars) {
        *ppvars++ = (char*)s.c_str();
        
    }
    *ppvars = NULL;
    std::string filename;
    if (suffix == "")
        filename = base + ".dddmp";
    else
        filename = base + "-" + suffix + ".dddmp";
    std::cerr << "Writing to file " << filename << std::endl;
    FILE *fp = fopen(filename.c_str(), "w");
    Dddmp_cuddBddStore(mgr.getManager(), (char*)base.c_str(), storage[id].getNode(), pvars, NULL, mode, varInfoType, (char*)filename.c_str(), fp);
    return;
}

bool     cuddAdapter::sameBDD(std::string s1, std::string s2) {
    return storage[s1] == storage[s2];
}

void cuddAdapter::setXORBlocks(synExp* s) {
    if (s->isConst())
        return;
    
    if (s->isSymbol())
        return;
    if (s->isNot()) {
        setXORBlocks(s->first());
        return;
    }
    if (s->isAnd() || s->isOr() || s->isImplies() || s->isEqual()) {
        setXORBlocks(s->first());
        setXORBlocks(s->second());
        return;
    }
    if (s->isIf()) {
        setXORBlocks(s->first());
        setXORBlocks(s->second());
        setXORBlocks(s->third());
        return;
    }
    if (s->isXOR()) {
        auxXOR(s);
        return;
    }
    std::cerr << "s is " << s << std::endl;
}

void cuddAdapter::auxXOR(synExp* exp) {
    // Here we create groups for group sifting
    std::set<int> theindices;
    // USE ONLY VAR VARIABLES or CONSTS
    for(synExp* var : exp->get_parms()) {
        if (var->isSymbol()) {
            //std::cerr << "Var is " << var << std::endl;
            theindices.insert(indices[var->getSymbol()].first);
        }
    }
    bool first = true;
    std::vector<std::string> vnames;
    int lastPos = 0;
    //std::cerr << "theindices size " << theindices.size() << std::endl;
    for(int i : theindices) {
        if (first) {
            //std::cerr << "X putting " << inVars[i] << " in vector for "
            //<< " index " << i << " pos " << mgr.ReadPerm(i) << std::endl;
            vnames.push_back(inVars[i]);
            first = false;
            lastPos = mgr.ReadPerm(i);
        }
        else {
            int p =  mgr.ReadPerm(i);
            if (p == lastPos+1) {
                //std::cerr << "Y putting " << inVars[i] << " in vector for "
                //<< " pos " << p << std::endl;
                vnames.push_back(inVars[i]);
                lastPos++;
            }
            else {
                createXORBlock(vnames);
                first = true;
                vnames.push_back(inVars[mgr.ReadInvPerm(p)]);
            }
        }
    }
    createXORBlock(vnames);
}


void cuddAdapter::createXORBlock(std::vector<std::string>& vec) {
    if (!vec.empty()) {
        theMenuBlocks.push_back(new MenuBlock(vec.begin(), vec.end(), "XOR", "XOR"));
        vec.clear();
    }
}

void cuddAdapter::relateVars(std::set<int> s) {
    //std::cerr << "relateVars ";
    //for(int x : s)
    //	std::cerr << x << " ";

    //std::cerr << std::endl;
    if (relatedVars)
        mgr.relateVars(s);
}

