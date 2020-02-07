//
//  SemanticsBuddy.hpp
//  myKconf
//
//  Created by david on 14/11/13.
//  Copyright (c) 2013 david. All rights reserved.
//

#ifndef __myKconf__SemanticsBuddy__
#define __myKconf__SemanticsBuddy__

#include <iostream>
#include <string>

#include <bdd.h>

using namespace buddy;

class SemanticsBuddy {
    
    private:
    
        std::string                        strValue;
        std::pair<const bdd, const bdd>  logicValue;
        bool isStr;

    public:
    
    SemanticsBuddy() : logicValue(std::pair<const buddy::bdd&, const buddy::bdd&>(bddfalse, bddfalse))
    { isStr = false;
    }
    SemanticsBuddy(std::string s);
    SemanticsBuddy(const bdd& a, const bdd& b);
    
    SemanticsBuddy& operator=(const SemanticsBuddy& t) {
        std::cout << "SemanticsBuddy const copy constructor" << std::endl;
        exit(-1); }
    ~SemanticsBuddy() {}
    
    bool   getIsStr()     { return isStr; }
    void   setStr(bool b) { isStr = b;    }
    const  bdd&      main()   const { return logicValue.first; }
    const  bdd&      module() const { return logicValue.second;}
            std::string     getStr() const { return strValue;         }
    
    friend          std::ostream&   operator<<(std::ostream& os, const SemanticsBuddy& t);
    friend const    SemanticsBuddy  operator&&(const SemanticsBuddy& e1, const SemanticsBuddy& e2);
    friend const    SemanticsBuddy  operator||(const SemanticsBuddy& e1, const SemanticsBuddy& e2);
    friend const    SemanticsBuddy  operator==(const SemanticsBuddy& e1, const SemanticsBuddy& e2);
    friend const    SemanticsBuddy  operator!=(const SemanticsBuddy& e1, const SemanticsBuddy& e2);
    friend const    SemanticsBuddy  operator!(const SemanticsBuddy& e);
    friend const    SemanticsBuddy  operator<(const SemanticsBuddy& e1, const SemanticsBuddy& e2);
    friend const    SemanticsBuddy  operator>(const SemanticsBuddy& e1, const SemanticsBuddy& e2);
    friend          bool            equal(const SemanticsBuddy& e1, const SemanticsBuddy& e2);
    friend          bool            different(const SemanticsBuddy& e1, const SemanticsBuddy& e2);
    
    //SemanticsBuddy operator&&=(SemanticsBuddy& e1, SemanticsBuddy& e2);

    
};
#endif /* defined(__myKconf__SemanticsBuddy__) */
