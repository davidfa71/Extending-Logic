//
//  variableSorter.h
//  myKconf
//
//  Created by David on 27/07/15.
//  Copyright (c) 2015 david. All rights reserved.
//

#ifndef __myKconf__variableSorter__
#define __myKconf__variableSorter__

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

class variableSorter {
    
    protected :
    
    std::map<std::string, int> occurrences;
    
public:
    
    variableSorter(const std::map<std::string, int>& occ) : occurrences(occ) {}

    virtual bool    operator()(const std::string& a, const std::string& b)  {
                        std::cerr << "Error. Using variableSorter operator." << std::endl;
                        return false;
                    }
    virtual void    mysort(std::vector<std::string>::iterator beg, std::vector<std::string>::iterator end) {
    std::sort(beg, end, *this);
}
};

#endif /* defined(__myKconf__variableSorter__) */
