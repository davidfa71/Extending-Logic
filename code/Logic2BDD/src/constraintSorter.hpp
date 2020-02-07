//
//  constraintSorter.h
//  myKconf
//
//  Created by David on 23/06/15.
//  Copyright (c) 2015 david. All rights reserved.
//

#ifndef __myKconf__constraintSorter__
#define __myKconf__constraintSorter__

#include <stdio.h>
#include <vector>
#include <string>

#include "synExp.hpp"
#include "Components.hpp"
#include "cuddAdapter.hpp"

class constraintSorter {
    
protected:
    cuddAdapter*             adapter;
    std::vector<synExp*>    c;
    
public:
    
    constraintSorter(cuddAdapter* adapter,
                     std::vector<synExp*> c) : adapter(adapter), c(c) {};
 
  
    virtual void reorderConstraints(std::vector<synExp*>::iterator beg, std::vector<synExp*>::iterator end, bool lastReorder) =0;
    
    virtual std::string     giveInfo(synExp *s)             { return "constraintSorter Error"; }
    virtual bool    operator()(const synExp *a, const synExp *b) = 0;
    

    virtual void thisOneIsProcessed(synExp *s) { return; }
    virtual ~constraintSorter() {};
    
};

#endif /* defined(__myKconf__constraintSorter__) */
