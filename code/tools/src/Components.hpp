//
//  Components.hpp
//  myKconf
//
//  Created by David on 01/12/15.
//  Copyright ?? 2015 david. All rights reserved.
//

#ifndef Components_hpp
#define Components_hpp

#include <stdio.h>
#include <vector>
#include <set>
#include <sstream>

#include "synExp.hpp"
#include "humanNums.hpp"
#include "orderHelper.hpp"

class Components {
    
    public  :
        
    virtual void    newVariable()                                   = 0;
    virtual void    changeOrder(std::vector<int>  x)                = 0;
    virtual std::vector<int> getOrder()                             = 0;
    //virtual void    replayConstraints()                             = 0;
    virtual int     getComponent(int x)                             = 0;
    virtual int     getStart(int x)                                 = 0;
    virtual int     getCompSize(int x)                              = 0;
    virtual int     getNumComponents()                              = 0;
    virtual int     getMaxLength()                                  = 0;
    virtual bool    join(synExp* s, bool changeOrder)               = 0;
    virtual void    reorder()                                       = 0;
    virtual bool    isOutOfSync()                                   = 0;
    virtual void    sync()                                          = 0;
    virtual int     find(int x)                                     = 0;
    virtual void    showInfo(int x)                                 = 0;
    virtual void    setUnchanged(int x)                             = 0;
    virtual void    checkLengths()                                  = 0;

    // first is component start, second is component size;
    virtual std::set<std::pair<int, int> >   listComponents()       = 0;
    virtual std::set<std::pair<int, int> >    listChangedComponents()       = 0;
    virtual void printComponents() = 0;

    virtual float   computeEntropy()                                 = 0;
    virtual bool    makeUnion(int x, int y, bool changeOrder = true) = 0;
    virtual ~Components() {};
};
#endif /* Components_hpp */
