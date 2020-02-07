//
//  OneComponent.hpp
//  myKconf
//
//  Created by david on 08/07/16.
//  Copyright © 2016 david. All rights reserved.
//

#ifndef OneComponent_hpp
#define OneComponent_hpp

#include <stdio.h>
#include <set>

#include "Components.hpp"

class OneComponent : public Components {
    public :
    
    OneComponent() { num = 0; };
    OneComponent(std::vector<int>& var2pos, std::vector<int>& pos2var);
    OneComponent(int num);
    std::vector<int> getOrder();

    void    changeOrder(std::vector<int>  x);
    void    newVariable();
    void    replayConstraints();
    int     getComponent(int x) ;
    int     getStart(int x);
    int     getCompSize(int x);
    int     getNumComponents();
    int     getMaxLength();
    int     getMaxReorder(int x);
    bool    join(synExp* s, bool changeOrder);
    void    reorder();
    bool    isOutOfSync();
    void    sync();
    int     find(int x);
    void    showInfo(int x);
    void    setUnchanged(int x);                            
    std::set<std::pair<int, int> >              listComponents();
    std::set<std::pair<int, int> >       listChangedComponents();
    void                                        checkLengths();

    void                                    printComponents();

    float   computeEntropy();
    bool    makeUnion(int x, int y, bool changeOrder = true);

    protected :
    std::vector<int> pos2var;
    int num;
};


#endif /* OneComponent_hpp */
