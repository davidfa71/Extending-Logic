//
//  orderHelper.hpp
//  myKconf
//
//  Created by david on 18/07/16.
//  Copyright © 2016 david. All rights reserved.
//

#ifndef orderHelper_hpp
#define orderHelper_hpp

#include <stdio.h>

class orderHelper {
    public :
    orderHelper(int id, int pos, int compstart, float avg) : id(id), pos(pos), compstart(compstart), avg(avg){};
    int id, pos, compstart;
    float avg;
    bool operator()(orderHelper& a, orderHelper& b) {
        if (a.avg != b.avg)
            return a.avg < b.avg;
        if (a.compstart != b.compstart)
            return a.compstart < b.compstart;
        
        return a.pos < b.pos;
    }
    
};

#endif /* orderHelper_hpp */
