//
//  configInfo.h
//  myKconf
//
//  Created by david on 28/11/13.
//  Copyright (c) 2013 david. All rights reserved.
//

#ifndef __myKconf__configInfo__
#define __myKconf__configInfo__

#include <iostream>
#include <map>
#include <vector>

#include "varInfo.hpp"
#include "synExp.hpp"
#include "symbolTable.hpp"
#include "eval.hpp"

#include "choiceInfo.hpp"

class configInfo;
class choiceInfo;
class selectHelper {
    public:
    
    synExp*     getExpression();
    synExp*     getDep();
    std::string getVar();
    configInfo* getConfig(); 
  
    
    selectHelper(std::string variable,  // The selector
                 synExp *expression,    // The conditional in the select statement
                 synExp *dep,             // The original dependency
                 configInfo* c          // The selector config
                 ) : variable(variable), expression(expression),  dep(dep), c(c)  {}
    

    
    private:
    
    std::string variable;
    synExp *expression;
    synExp* dep;
    configInfo* c;
    
};

class configInfo : public varInfo {
    
    public :
    static void addAlternative(std::string name, configInfo* val);
    configInfo() {
        choiceMember  =  false;
        firstVar      =  secondVar = pos = 0;
        vartype       =  name      =  promptext = "";
        value         =  NULL;
        visibilityVar =  synTrue;
        promptGuard   =  synFalse;
        prompt        =  synFalse;
        isThereADefaultValue       =  false;
        hasConfig     =  false;
        selects       =  false;
        stringValue   =  false;
        artificial    =  false;
        fromEnvironment = false;
        internal       = false;
        stringDefinedByChoice = false;
        chi           = NULL;
    }
    
    ~configInfo()       {
        firstVar      = secondVar = 0;
        vartype       = name      =  promptext = "";
        value         = NULL;
        visibilityVar = synTrue;
        promptGuard   = synFalse;
        prompt = synFalse;
        }
    
    void clear()        {
        firstVar      = secondVar = 0;
        vartype       = name      =  promptext = "";
        value         = NULL;
        visibilityVar = synTrue;
        promptGuard   = synFalse;
        prompt = synFalse;
    }
    void            setChoiceMember()   { choiceMember = true; }
    bool            isChoiceMember()    { return choiceMember; }
    static std::map<std::pair<std::string, std::string>, int> timesDependsOn;
    void simplifyDependency();

    bool static isSelectable(std::string s)     {
        return (mapSelect.find(s) != mapSelect.end()) && !mapSelect[s].empty();
    }
    void           setFromEnvironment() { fromEnvironment = true;}
    bool           comesFromEnvironment() { return fromEnvironment; }

    void        setPos(int x)                   { pos = x;                      }
    int         getPos()                        { return pos;                   }
    void        activatePrompt()                { prompt = synTrue;             }
    bool        isPrompt()                      { return (prompt == synTrue);   }
    void        addVisibility(synExp* e)        { visibilityVar = makeAnd(visibilityVar,
                                                                             e);
        
    }
    void        push_default(synExp* a, synExp* b) {
        defaultList.push_front(std::pair<synExp*, synExp*>(a->copy_exp(), b->copy_exp())); }
    void            addRange(synExp* first, synExp* second) { range.push_back(first); range.push_back(second);}
    void            processRange();
    void            processRangeMember(synExp* exp);
    void            setName(const std::string& s)  {
                        name         = s;
                        value = new synSymbol(s); }
    void            setPrompText(const std::string& s)    { promptext    = s;          }
    void            setPromptGuard(synExp *e)      { promptGuard  = e;     }

    void            setVartype(const std::string& s)      { vartype      = s; }
    void            setValue(synExp* e)            { value        = e;     }
    void            setFirstVar(int b)             { firstVar     = b;                 }
    void            setSecondVar(int b)            { secondVar    = b;                 }
    void            setHasConfig()                 { hasConfig    = true;              }
    
    synExp*         getVisibility()                { return visibilityVar;             }
    std::string     getPrompText()                 { return promptext;                 }
    synExp*         getPromptGuard()               { return promptGuard;               }
    std::string     getVartype()                   { return vartype;                   }
    synExp*         getValue()                     { return value;                     }
    std::string     getName()                      { return name;                      }
    int             getFirstVar()                  { return firstVar;                  }
    int             getLastVar()                   { return secondVar;                 }
    bool            getHasConfig()                 { return hasConfig;                 }

    
    std::list<synExp*>  giveConstraint();
    std::list<synExp*>  giveLogicalConstraint();
    std::list<synExp*>  giveStringConstraint();
    void processLogicalDefaults();
    choiceInfo* processStringDefaults();

    friend std::ostream& operator<<(std::ostream& os, configInfo& ci);
    void print(std::ostream& os);
    
    void addToMenu(const std::string& s)        { contents.push_back(s);    }
    void setSelects()                           { selects    = true;        }
    bool getSelects()                           { return selects;           }
    
    std::vector<std::string>::iterator    getContentBegin()  { return contents.begin();  }
    std::vector<std::string>::iterator    getContentEnd()    { return contents.end();    }
    int                                   getContentSize()   { return contents.size();   }
    // this selects x if e (inside an if g statement or true)
    void    addMapSelect(const std::string& x,  synExp* e, synExp* dep);

    
    std::string     getAlternative()                     { return alternative; }
    void            setAlternative(const std::string& s) { alternative = s;    }
    std::string     getCondition()                       { return condition;   }
    // When the time comes to start issuing constraints, we may find out that
    // selects are outdated, meaning that "x selects y" should really be
    // x selects y_WHEN_CONDITION. Since the map is indexed by the selected
    // variable this can get a bit messy.
    
    //static void fixMapSelect();
    bool            checkRecursiveSelects();
    bool            checkRecursiveDeps();
    bool            checkRecursiveDepsRecur(std::vector<std::string> vec);
    bool            checkRecursiveSelectsRecur(std::vector<std::string> vec);
    std::set<synExp*> intersectionSelectorSelectorDeps(std::string selector);
    synExp*           fixDeps(std::set<synExp*> inter, synExp* deps);

    bool            isArtificial() { return artificial;}
    bool            isInternal()   { return internal;  }
    void            addStringAlternative(std::string newName) {
        stringAlternatives[name].insert(newName);
    }
    configInfo*            addValue(std::string value);
    void              delValue(const std::string& s);

    void            setIsStringValue() { stringValue = true; }
    bool            isStringValue()    { return stringValue; }
    std::string     buildNameValue(std::string s);
    bool            isDummy()          { return internal && simple; }
    synExp*             processSelects();
    void                addIsDependedBy(std::string s) { setDepended.insert(s); }
    void                addDependsOn(std::string s)    { setDepends.insert(s);    }
    std::set<std::string>  getDependsOn()              { return setDepends;     }
    std::set<std::string>  getIsDepended()             { return setDepended;    }
    std::set<std::string>   configsISelect;
    static bool                                          useXOR;
    static std::map<std::string, std::set<std::string>> stringAlternatives;
    static std::map<std::string, std::set<std::string>> stringValues;

    private:
    std::list<synExp*> choicesDefineAll();

    bool processAllDefiningChoices();
    // This controls if the config has default guards that correspond
    // to all the members of one choice
    bool                stringDefinedByChoice;
    choiceInfo*         chi;
    bool                internal;
    bool                simple;
    bool                checkInternal();
    bool                checkSimple();
    synExp*             processDependencies();
    synExp*             processPrompts();
    synExp*             processAllStringDefaults();
    synExp*             processAllLogicalDefaults();
    
    synExp*             allStringValuesFalse();
    void                exactlyOneStringValue(std::string s, std::list<synExp*>& res);
    void                valuesImplyVariable(std::string s, std::list<synExp*>& res);
    configInfo*         defineName(std::string newName);
    
    bool              stringValue, choiceMember;
    

    synExp*             defaultWithString(std::string sval);
    synExp*             defaultWithSymbol(std::string sval);
    void  fixSelect();

    // variable x is selected by variable y if_expr exp
    static std::map<std::string, std::map<std::string, std::vector<selectHelper>>> mapSelect;
    // Whenever the declaration is inside an if, we save the condition of the if
    // If the ifs are nested, the conjunction must be taken
    // If none of the conditions of the respective ifs is met, the config
    // takes the default value (false)
        
    bool                                    artificial;
    static int                              artificialCount;

    std::set<std::string>                   setDepended, setDepends;
    // A map to connect the original name with the alternative configs
    // very useful to fix the select statements.
    static std::map<std::string, std::list<configInfo*> 	> mapAlternative;
    std::string                                     alternative, condition;
    static std::map<std::string, int>               timesProcessed;



    int                                            pos;
    bool                                           selects;
    std::vector<synExp*>                           selectors;
    bool                                           hasConfig;
    int                                            firstVar, secondVar;
    std::string                                    vartype;
    std::string                                    name, originalName;
    std::string                                    promptext;
    synExp                                        *promptGuard, *visibilityVar;
    // A logic formula to express permitted values by default clauses
    std::list<std::pair<synExp*, synExp*> >        defaultList;
    std::vector<synExp*>                           range;
    synExp*                                        logicalDefault;
    synExp*                                        stringDefault;
    // A logic formula to express permitted values input by the user
    synExp*                                        prompt;
    // The logic variable associated with this variable
    synExp*        value;
    bool                    fromEnvironment;
    // Is there a default value that can be assigned to the config or not
    bool              isThereADefaultValue;
    
    // Used for menus. The first and the last config/menuconfig of the menu.
    // Contents holds all the configs between first and last, inclusive.
    // We use this to set up block sifting
    std::vector<std::string> contents;

    
};
#endif /* defined(__myKconf__configInfo__) */
