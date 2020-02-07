//
//  TravInfo.hpp
//  myKconf
//
//  Created by david on 16/05/2018.
//  Copyright © 2018 david. All rights reserved.
//

#ifndef TravInfo_hpp
#define TravInfo_hpp

#include <stdio.h>
#include <iostream>
#include <map>
#include <list>
#include <cudd.h>
#include <thread>
#include <mutex>

template <typename T>
class TravInfo {
    private:
    
    int contRef, contVisit;
    bool mark, usedOften;
    T* pt;
    std::mutex                            visitMutex;
    std::list<std::mutex*>                parentSems;
    std::pair<std::mutex, std::mutex>     childSem;

    public:
    
    TravInfo() {
        contRef   = 0;
        contVisit = 0;
        pt        = NULL;
        mark      = false;
        usedOften = false;
    }
    
    void        setUsedOften(){ usedOften = true;       }
    bool        getUsedOften(){ return usedOften;       }
    void        lockVisit()   { visitMutex.lock();      }
    void        unlockVisit() { visitMutex.unlock();    }

    void        lockThenSem() { childSem.first.lock();  }
    void        lockElseSem() { childSem.second.lock(); }

    void        addParentThenSem(TravInfo<T>& ti)  { parentSems.push_back(&ti.childSem.first); }
    void        addParentElseSem(TravInfo<T>& ti)  { parentSems.push_back(&ti.childSem.second); }

    void        setResMP(T t)   { pt = new T(t);
        for(std::mutex* m : parentSems)
            m->unlock();
    }
    
    void        setRes(T t)   { pt = new T(t);
                              }
    T*          getRes()      { return pt;          }
    int         getRef()      { return contRef;     }
    void        incRef()      { contRef++;          }
    int         getVisit()    { return contVisit;   }
    void        incVisit()    { contVisit++;        }
    bool        getMark()     { return mark;        }
    void        setMark()     { mark = true;        }
 
};
#endif /* TravInfo_hpp */
