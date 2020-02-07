//
//  symbolTable.hpp
//  myKconf
//
//  Created by david on 02/10/14.
//  Copyright (c) 2014 david. All rights reserved.
//

#ifndef __myKconf__symbolTable__
#define __myKconf__symbolTable__

#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <string>
#include <algorithm>

class configInfo;

class symbolTable {
    
  private:
    
    static std::unordered_map<std::string, configInfo*> table;
    static std::unordered_map<std::string, int>         mapTimesDeclared;
    static std::vector<configInfo*>           ordered;
    static std::unordered_set<std::string>              undefined;

  public:
    
    static std::vector<configInfo*>::iterator getIterator(std::string s);
    
    typedef std::vector<configInfo*>::iterator iterator;
    static int      size()  { return int(ordered.size());  }
    static iterator begin() { return ordered.begin();      }
    static iterator end()   { return ordered.end();        }
    static std::unordered_set<std::string> getUndefined() { return undefined;}
    
    static  int     timesDeclared(std::string s);
    static  void    declareSymbol(std::string s, configInfo* p);
    static  void    addSymbol(std::string s, configInfo* p);
    static  void    deleteSymbol(std::string s);
    //static  configInfo *addString(std::string name, std::string value);
    static  configInfo *getSymbol(std::string s);
    
};

#endif /* defined(__myKconf__symbolTable__) */
