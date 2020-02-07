//
//  bddBuilder.hpp
//  myKconf
//
//  Created by david on 11/10/14.
//  Copyright (c) 2014 david. All rights reserved.
//

#ifndef __myKconf__bddBuilder__
#define __myKconf__bddBuilder__

#include <string>
#include <set>
#include <list>
#include <map>
#include <vector>
#include <iomanip>
#include <tuple>
#include <fstream>
#include <sstream>
#include <iostream>

#include "MenuBlock.hpp"
#include "synExp.hpp"
#include "configInfo.hpp"
#include "humanNums.hpp"
#include "mytime.hpp"
#include "cuddAdapter.hpp"
#include "constraintSorter.hpp"
#include "minspanSorter.hpp"
#include "expSorter.hpp"
#include "compSorter.hpp"
#include "Components.hpp"
#include "RememberSorter.hpp"

class bddBuilder {
    
    public:
    
    bddBuilder(std::vector<synExp*>&        consList) :
                                    consList(consList)
                                    {
                                        reorder         = true;
                                        minNodes        = 0;
                                        maxNodes        =
                                        std::numeric_limits<int>::max();
                                        minConstraint   = -1;
                                        reorderEvery    = 1;
                                        blocks          = false;
                                    };
    
    void setAdapter(cuddAdapter* f)              { adapter              = f;    }
    void setVarFileName(std::string s)          { varFileName          = s;    }
    void setStatsFileName(std::string s)        { statsFileName        = s;    }
    void setTempBDDName(std::string s)          { tempBDDName          = s;    }
    void setAppliedExpFilename(std::string s)   { appliedExpFilename   = s;    }
    void setUnappliedExpFileName(std::string s) { unappliedExpFilename = s;    }
    void setReorderMethod(std::string s)        { reorderMethod        = s;
                            if (s == "noreorder") reorder = false;             }
    void setReorderInterval(int x)              { reorderInterval      = x;    }
    void setMinNodes(int x)                     { minNodes             = x;    }
    void setMaxNodes(int x)                     { maxNodes             = x;    }
    void setMinConstraint(int x)                { minConstraint        = x;    }
    void setReorderEvery(int x)                 { reorderEvery         = x;    }
    void setBlocks(bool b)                      { blocks               = b;    }
    void setLineLength(int l)                   { lineLength           = l;    }
    
    void build(double inflation);
    bool syncOrder();
    void printPartOne(synExp* pconst, int counter);
    void printPartTwo(int nn);
    void reorderComponents(int n);
    bool isReorderTime(int counter, int lastnodes, int nn, int constLastReorder, double inflation);
    void reorderProtocol(std::vector<synExp*>::iterator itl);
    void setConstraintSorter(constraintSorter* cs) { myOrder = cs; };

    
    private:
        
    constraintSorter*           myOrder;
    template<class T> void      fastReorderConstraints(T beg, T end);
    std::set<std::string>       internal;
    bool                        blocks, reorder;
    void                        writeNameOrder(const std::vector<std::string>& ord);
    std::string                 varFileName, statsFileName, reorderMethod, tempBDDName,
                                appliedExpFilename, unappliedExpFilename;
    int                         reorderInterval, reorderEvery, reorderFromConst = 0;
    int                         minNodes, maxNodes, minConstraint;
    int                         lineLength = 95;
    cuddAdapter*                 adapter;
    std::ofstream               varFile, statsFile;
    std::vector<synExp*>&       consList;
    
    void                        namevar(std::string name, int index);
    void                        printOrder(std::vector<int> ord);
    void                        readVarOrder();
    
};
#endif /* defined(__myKconf__bddBuilder__) */
