//
//  ifInfo.h
//  myKconf
//
//  Created by david on 28/11/13.
//  Copyright (c) 2013 david. All rights reserved.
//

#ifndef __myKconf__ifInfo__
#define __myKconf__ifInfo__

#include <iostream>

#include "varInfo.hpp"

class ifInfo : public varInfo {
    public:
    
        ifInfo() { visibilityVar = synTrue; };

        void                setGuard(synExp *g)  { guard = g;    }
        synExp*             getGuard()           { return guard; }
        bool                isPrompt()           { return false; }
        std::string         getVartype()         { return "";    }
        synExp*             getValue()           { return synTrue;  }
        synExp*             getRegularValue()               {
        throw std::logic_error("Calling getRegularValue from IfInfo\n");
    }
        std::string         getName()            { return "";    }
        std::string         getPrompText()       { return "";    }
    
    

        void                addVisibility(synExp *e)            {
            visibilityVar = makeAnd(visibilityVar, e);
        }
        void                setName(const std::string& s)       {}
        void                setPrompText(const std::string& s)  {}
        void                setPromptGuard(synExp* g)           {}
        void                setVartype(const std::string& s)    {}
        void                setHasConfig()                      {}
        void                setSelectable()                     {}
        void                processDefaults()                   {}
        void                setValue(synExp* s)                 {}
        void                setFirstVar(int b)                  {}
        void                setSecondVar(int b)                 {}
        void                push_default(synExp *a, synExp *b)  {}
        synExp*             getVisibility()                     { return visibilityVar; }

    std::list<synExp*>      giveConstraint();
    void                addStringAlternative(std::string string)  {
        throw std::logic_error("Calling addStringAlternative from ifInfo\n");
    }
        
    void            setIsStringValue() {};
    bool            isStringValue() { return false; }

private:
    
    synExp*                 guard, *visibilityVar;
    std::vector<varInfo*>   infos;

};
#endif /* defined(__myKconf__ifInfo__) */
