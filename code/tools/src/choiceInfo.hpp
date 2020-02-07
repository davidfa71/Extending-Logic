//
//  choiceInfo.h
//  myKconf
//
//  Created by david on 25/11/13.
//  Copyright (c) 2013 david. All rights reserved.
//

#ifndef __myKconf__choiceInfo__
#define __myKconf__choiceInfo__

#include <iostream>
#include <sstream>
#include <unordered_set>
#include "varInfo.hpp"
#include "configInfo.hpp"
class MenuBlock;

class choiceInfo : public varInfo {
    
    public:
    
         choiceInfo();
        ~choiceInfo();

        std::vector<MenuBlock*> createBlocks();
        void clear();
        std::list<synExp*>  giveConstraint();

        friend  std::ostream& operator<<(std::ostream& os, choiceInfo& vi);
        void        print()             {   std::cout << *this << std::endl;        }
        void        activatePrompt()    {   prompt = synTrue ;                      }
        bool        isPrompt()          {   return (prompt == synTrue);             }
        synExp*     orMemberDeps();
        bool        isChoice() { return true; }
        void        setSelectable() {};
        void        processDefaults();
        void        setValue(synExp* s){};
        void        setFirstVar(int b){};
        void        setSecondVar(int b){};
        void        push_default(synExp* a, synExp* b) {
                    defaultList.push_front(std::pair<synExp*, synExp*>(a,
                                                                       b));
            
        }
        void        addVisibility(synExp *e){ visibilityVar = makeAnd(visibilityVar,
                                                                      e); }
        bool        checkDeps(std::set<std::string> s, std::string member);
        void        push_symbol(configInfo* ci);
    
        void        setName(const std::string& s)       { name          = s;    }
        void        setPrompText(const std::string& s)  { promptext     = s;    }
        void        setPromptGuard(synExp* e)           { promptGuard   = e;    }
        void        setVartype(const std::string& s)    { vartype       = s;    }
        void        setOptional()                       { optional      = true; }
        void        setHasConfig()                      { hasConfig     = true; }
        void        addToMenu(const std::string& s)     { contents.push_back(s);}
    std::vector<std::string>::iterator     getContentBegin()           { return contents.begin();  }
    std::vector<std::string>::iterator     getContentEnd()             { return contents.end();    }
    int                                    getContentSize()            { return contents.size();   }


        synExp*         getVisibility()                 { return visibilityVar; }
        std::string getPrompText()                      { return promptext;     }
        synExp*     getPrompGuard()                     { return promptGuard;   }

        std::string         getVartype()                { return vartype;       }
        synExp*             getValue()                  { return value;         }
        synExp*             getRegularValue()               {
            throw std::logic_error("Calling getRegularValue from choiceInfo\n");
        }
    void        addStringAlternative(std::string string)  {
        throw std::logic_error("Calling addStringAlternative from choiceInfo\n");
        }
        std::string         getName()                   { return name;          }
        bool            getHasConfig()                  { return hasConfig;     }   
        void            setIsStringValue() {};
    bool            isStringValue() { return false; }
    
    static bool        isConfigChoiceMember(std::string s) {
        return member2name.find(s) != member2name.end();
    }
    static std::string getChoiceName(std::string m)     { return member2name[m];         }
    static int         getNumMembers(std::string n)     { return name2numerofmembers[n]; }
    static choiceInfo* getName2choice(std::string n)    { return name2choice[n];         }
    static choiceInfo* getChoiceFromMember(configInfo* ci) {
        return member2choice[ci];
    }
    std::set<std::string> getMemberNames();
    static bool useXOR;
    private:
    //std::vector<MenuBlock*>                             blocks;
    //static std::map<std::string, std::set<std::string> > name2members;
    static std::map<std::string, std::string>            member2name;
    static std::map<std::string, int>                    name2numerofmembers;
    static std::map<std::string, choiceInfo*>            name2choice;
    static std::map<configInfo*, choiceInfo*>            member2choice;
    // A logic formula to express permitted values by default clauses
    std::list<std::pair<synExp*, synExp*> >        defaultList;
    synExp*                                        xdefault;
    bool                                hasConfig;
    std::list<configInfo*>              symbolList;
    bool                                optional;
    std::vector<std::string>            contents;
    std::string                         vartype;
    std::string                         name;
    std::string                         promptext;
    synExp                              *promptGuard;
    synExp                              *prompt, *visibilityVar;
    // The BDDs of the logic variables associated with this variable
    synExp*                             value;


    // Return an expression meaning exactly one of the choices is chosen as true.
        synExp* giveExactlyOne();
        synExp* allFalse();
        synExp* allModules();
        synExp* noMoreThanOneTrue();
        synExp* atLeastOneTrue();
        synExp* noBiggerThan(synExp* that);
        synExp* atLeastOneMoreThanFalse();
    
};
#endif /* defined(__myKconf__choiceInfo__) */
