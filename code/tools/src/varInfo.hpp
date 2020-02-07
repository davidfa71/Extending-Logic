//
//  varInfo.h
//  myKconf
//
//  Created by david on 12/11/13.
//  Copyright (c) 2013 david. All rights reserved.
//

#ifndef __myKconf__varInfo__
#define __myKconf__varInfo__

#include <string>
#include <iostream>
#include <sstream>
#include <list>

#include "synExp.hpp"

class varInfo {
    
    public:
    
    varInfo()       { dependsVar = synTrue; innerDep = synTrue; timesCalledSetDependStack = 0;}
    ~varInfo()      {  }
    void clear()    {  }

    virtual void setPos(int x) {};
    virtual int getPos() { return 0;}
    
    virtual void print(std::ostream& os);
    virtual bool isChoice() { return false; }
    void    addDepends(synExp* dep)  { dependsVar = makeAnd(dependsVar, dep->copy_exp()); };
    virtual void    addVisibility(synExp *e) = 0;
    virtual int     selectSize() { return 0;}
    friend  std::ostream& operator<<(std::ostream& os, varInfo* vi);
    
    virtual bool            isPrompt() = 0;
    virtual std::list<synExp*>         giveConstraint()    = 0;
    virtual void            activatePrompt()
                        { throw std::runtime_error( "called activatePrompt from pure virtual" );}
    virtual void            setChoiceMember() {};
    virtual bool            isChoiceMember() { return false; };
            void            setMenuName(std::string s) { menuName = s;}
    virtual void            setName(const std::string& s) = 0;
    virtual void            setPrompText(const std::string& s) = 0;
    virtual void            setPromptGuard(synExp* g) = 0;
    virtual void            setVartype(const std::string& s) = 0;
            void            setFile(const std::string& s){ file = s; }
    std::string             getFile() { return file; }
    virtual void            setValue(synExp* s) = 0;
    virtual void            setFirstVar(int b) = 0;
    virtual void            setSecondVar(int b) = 0;
    virtual void            setHasConfig() = 0;
    virtual void            push_default(synExp *a, synExp *b) = 0;
   
    virtual synExp*         getVisibility()                { return synTrue; }

            synExp*         getDependsVar()      { return dependsVar; };
    virtual std::string     getVartype()   = 0;
    virtual synExp*         getValue()     = 0;

    virtual std::string     getName()      = 0;
    virtual std::string     getPrompText() = 0;
    virtual synExp*         getPromptGuard()     { return NULL; }
    virtual bool            getHasConfig()       { return false; }
    
    virtual std::string     getAlternative()                      { return ""; }
    virtual void            setAlternative(const std::string& s) {    }
    
    virtual void    addMapSelect(const std::string& x, synExp* e)
                        { throw std::runtime_error( "Error, calling varInfo addMapSelect method.");}
    //virtual void         addIfDeclaration(synExp* e) {};
    
   
    virtual bool        isArtificial()                          { return false;       }
    virtual void        setIsStringValue()                      = 0;
    virtual bool        isStringValue()                         = 0;
    virtual void        setExternalIfGuard(synExp* guard)       { }
    virtual synExp*     getExternalIfGuard()                    { return synTrue;     }
    virtual void        addDeclaration(const std::string& s)    { }
    virtual std::string getCondition()                          { return "";          }
    virtual void        addStringAlternative(std::string string)     = 0;
    void                setDependStack(std::vector<synExp*>& v)  {
        timesCalledSetDependStack++;
        if (timesCalledSetDependStack > 1) {
            std::ostringstream ost;
            ost << "Called " << timesCalledSetDependStack << " times for the same varInfo";
            throw std::logic_error(ost.str());
        }
        
        //std::cerr << "Setting dependStack for " << std::setw(60) << getName() << " file "
        //<< getFile() << std::endl;
        for(synExp* s : v) {
            //std::cerr << "Pushing " << s << std::endl;
            dependStack.push_front(s);
        }
        //std::cerr << "end Setting dependStack" << std::endl;
    }
    std::list<synExp*>  getDepStack()                           { return dependStack;              }
    void                setDepLength(int i)                     { deplength = i;                   }
    int                 getDepLength()                          { return deplength;                }
    void                addInnerDep(synExp* s)                  { innerDep = makeAnd(innerDep, s); };
    void                simplifyDependency();
    private :
    
    std::string             file;
    std::string             menuName;
    int                     deplength, timesCalledSetDependStack;
    
    protected :
    
    std::list<synExp*>    dependStack;
    synExp*               dependsVar, *innerDep;

    
};
#endif /* defined(__myKconf__varInfo__) */
