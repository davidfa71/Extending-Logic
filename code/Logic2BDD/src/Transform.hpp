// $Id: driver.h 17 2007-08-19 18:51:39Z tb $ 	
/** \file driver.h Declaration of the kconf::Driver class. */

#ifndef KCONF_TRANSFORM_H
#define KCONF_TRANSFORM_H

#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <tuple>

#include "PermScorer.hpp"
#include "OtherScorer.hpp"
#include "RandomScorer.hpp"
#include "forceBlockScorer.hpp"
#include "SiftScorer.hpp"
#include "forceScorer.hpp"
#include "maxOccSorter.hpp"
#include "minOccSorter.hpp"
#include "MenuBlock.hpp"
#include "minmaxSorter.hpp"
#include "minspanSorter.hpp"
#include "nullSorter.hpp"
#include "RememberSorter.hpp"
#include "expSorter.hpp"
#include "compSorter.hpp"
#include "synExp.hpp"
#include "eval.hpp"
#include "cuddAdapter.hpp"
#include "MultiComponents.hpp"
#include "OneComponent.hpp"
#include "orderHelper.hpp"
#include "synExpDriver.hpp"
#include "bddBuilder.hpp"


namespace kconf {
   

class Transform
{

private:
    
    Components*                     pcomp;
    std::set<std::string>           existentialVars;
    std::vector<synExp*>            expList;
    std::map<std::string, int>      varMap;
    std::vector<std::string>        theVector, variables, ordering;
    std::vector<int>                pos2var, var2pos;
    
    void computeComponents();
    void reorderVariables();
    
  public:
    Transform();

    std::string                     base;
    int                             window            = 0;
    double                          scoreThreshold    = 1.0;
    bool                            static_components = true;
    bool                            dynamic_components= true;
    bool                            expand            = false;
    bool                            doCNF             = false;
    bool                            scoreReorder      = false;
    double                          inflation         = 10;
    bool                            outputComponents  = false;
    bool                            simulate          = false;
    bool                            blocks            = true;
    std::string                     constReorder      = "none";
    std::string                     scoreMethod       = "none";
    bool  	                        relatedVars       = false;
    std::string                     varOrderFile;
    bool                            pickup            = false;
    int                             reorderEvery      = 1;
    int                             reorderInterval   = 0;
    int                             minConstraint     = 0;
    int                             minNodes          = 0;
    int                             lineLength        = 60;
    int                             maxNodes          = std::numeric_limits<int>::max();
    double                          cacheMultiplier   = 1;
    std::vector<MenuBlock*>         menuBlocks;

    int                             slidingWindow;
    std::string                     reorderMethod, suffix;
    std::string                     adapterType = "cudd";
    cuddAdapter                     *adapter;
    constraintSorter*               newCnstOrder(const std::string & s);

    void    readVars(std::string varFile);
    void    readExp(std::string expFile);
  
    void treatVarsAndConstraints();
    std::vector<synExp*> simplifyConstraints(std::vector<synExp*> pendingConstraints);

    void buildBDD();

    bool reorder = true;
    bool verbose;

    // Process default statements
    
};

} // namespace kconf

#endif // KCONF_TRANSFORM_H
